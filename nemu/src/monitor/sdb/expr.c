#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

static int divzeroflag = 0;
extern int divzeronum;
extern word_t paddr_read(paddr_t addr, int len);
extern const char *regs[];
extern word_t isa_reg_str2val(const char *s, bool *success);
static int flag = 0;
enum {
  TK_NOTYPE = 256, TK_EQ,
  NUMBER, NEGATIVE, HEX, REG, TK_NEQ, TK_AND, POINTER, 
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},     // spaces
  {"\\+", '+'},          // plus
  {"==", TK_EQ},         // equal
  {"-", '-'},            // minus
  {"\\*", '*'},           // multiplication
  {"/", '/'},            // division
  {"\\(", '('},           // Left parenthesis
  {"\\)", ')'},           // Right parenthesis
  {"\\b[0-9]+\\b", NUMBER}, // number
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"0[xX][0-9a-fA-F]+", HEX},
  {"\\$[a-zA-Z0-9]+", REG},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
	      case '+':
			  tokens[nr_token].type='+';
			  strncpy(tokens[nr_token].str, substr_start, substr_len);
			  nr_token++;
			  break;
		  case '-':
              tokens[nr_token].type='-';
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
			  nr_token++;                                                                                                          
              break;
		  case '*':
              tokens[nr_token].type='*';
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;
		  case '/':
              tokens[nr_token].type='/';
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;
     	  case '(':
              tokens[nr_token].type='(';
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;  
          case ')':
              tokens[nr_token].type=')';
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;  
		  case NUMBER:
			  tokens[nr_token].type=NUMBER;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
			  nr_token++;
			  break;
		  case TK_EQ:
			  tokens[nr_token].type=TK_EQ;
			  strncpy(tokens[nr_token].str, substr_start, substr_len);
              nr_token++;
              break;
		  case TK_NEQ:
			  tokens[nr_token].type=TK_NEQ;
			  strncpy(tokens[nr_token].str, substr_start, substr_len);
			  nr_token++;
			  break;
		  case TK_AND:
			  tokens[nr_token].type=TK_AND;
			  strncpy(tokens[nr_token].str, substr_start, substr_len);
			  nr_token++;
			  break;
		  case HEX:
			  tokens[nr_token].type=NUMBER;
			  strncpy(tokens[nr_token].str, substr_start+2, substr_len-2);
			  int dec;
//			  printf("%s\n", tokens[nr_token].str);
			  sscanf(tokens[nr_token].str, "%x", &dec);
			  sprintf(tokens[nr_token].str, "%u", dec);
			  nr_token++;
			  break;
		  case REG:
			  tokens[nr_token].type=NUMBER;
			  strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
			  int ret;
			  bool success = true;
			  ret = isa_reg_str2val(tokens[nr_token].str, &success);
			  if(success == false){
				  printf("Can't find the register!\n");
				  return false;
			  }
			  sprintf(tokens[nr_token].str, "%d", ret);
			  nr_token++;
//			  printf("%s\n", tokens[nr_token].str);
			  break;
          default: TODO();
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  int tail = nr_token;
  for(int k = 0; k < tail; k++){
	  if(tokens[k].type == '-')
		  if(k == 0 || tokens[k-1].type == '+' || tokens[k-1].type == '-' || tokens[k-1].type == '*' || tokens[k-1].type == '/' || tokens[k-1].type == '(' || tokens[k-1].type == TK_EQ || tokens[k-1].type == TK_NEQ || tokens[k-1].type == TK_AND){
			  tokens[k].type = NEGATIVE;
		  }
/*	  if(tokens[k].type == NEGATIVE){
		  if(tokens[k+1].type == '('){
			  //printf("success\n");
			  for(int a = k+1; a < nr_token; a++){
				  //printf("success\n");
				  if(a == k+2){
					  int b = -1 * atoi(tokens[a].str);
					  sprintf(tokens[a].str,"%d",b);
				  }
				  if(tokens[a].type == '+')
					  tokens[a].type = '-';
				  else if(tokens[a].type == '-')
					  tokens[a].type = '+';
				  else if(tokens[a].type == ')')
					  break;
			  }
		  }
		  tokens[k].type = tokens[k+1].type;
		  int ne = -1 * atoi(tokens[k+1].str);
		  sprintf(tokens[k].str,"%d",ne);
		  for(int m = k+1; m < nr_token; m++){
			  tokens[m].type = tokens[m+1].type;
			  int n = atoi(tokens[m+1].str);
		      sprintf(tokens[m].str,"%d",n);
		  }
		  nr_token--;
	  }*/
	  if (tokens[k].type == '*' && (k == 0 || tokens[k-1].type == '+' || tokens[k-1].type == '-' || tokens[k-1].type == '*' || tokens[k-1].type == '/' || tokens[k-1].type == '(' || tokens[k-1].type == TK_EQ || tokens[k-1].type == TK_NEQ || tokens[k-1].type == TK_AND)) {
		  tokens[k].type = POINTER;
	  }
  }
  //for (int j = 0; j < nr_token; j++)
//	  printf("type=%d, str=%s\n", tokens[j].type, tokens[j].str);
  return true;
}

bool check_parentheses(int p, int q){
	int i = p, left = 0, right = 0;
//	flag = 0;
	if( tokens[p].type != '(' || tokens[q].type != ')' )
		return false;
	else{
		for( ; i < q; i++){
			if(left == right + 1 && tokens[i].type == ')')
				return false;
			if(tokens[i].type == '(')
				left++;
			else if(tokens[i].type == ')')
				right++;
			else continue;
		}
		//i--;
		if(i == q && left == right + 1)
			return true;
		else
			printf("Evaluator is invalid!\n");
		    //flag = 1;	
			return false;
	}
}

unsigned int eval(int p, int q) {
//	printf ("%d\t%d\n", p, q);
  if (p > q) {
	  printf("Evaluator is invalid!\n");
	  assert(0);
    /* Bad expression */
  }
  else if (p == q) {
	  if(tokens[p].type == NUMBER){
//		  unsigned int i = 0;
//		  sprintf(tokens[nr_token].str, "%u", i);
		  int i = atoi(tokens[p].str);
		  return i;
	  }
	  assert(0);
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else if (check_parentheses(p, q) == true && flag == 1){
	  printf("Evaluator is invalid!!\n");
	  assert(0);
  }
  else {
	  int j = p, operator = p, num = 0;
	  for( ; j != q; j++){
		  if (tokens[j].type == '(')
			  num++;
		  if (tokens[j].type == ')')
			  num--;
		  if (num == 0){
			  if(tokens[j].type == TK_AND){
				  operator = j;
			  }
			  if(tokens[j].type == TK_EQ || tokens[j].type == TK_NEQ){
				  if(operator == p || tokens[operator].type == '+' || tokens[operator].type == '-' || tokens[operator].type == '*' || tokens[operator].type == '/' || tokens[operator].type == POINTER || tokens[operator].type == NEGATIVE || tokens[operator].type == TK_EQ || tokens[operator].type == TK_NEQ || tokens[operator].type == HEX || tokens[operator].type == REG){
					  operator = j;
				  }
			  }
			  if (tokens[j].type == '+' || tokens[j].type == '-'){
				  if(operator == p || tokens[operator].type == '+' || tokens[operator].type == '-' || tokens[operator].type == '*' || tokens[operator].type == '/' || tokens[operator].type == POINTER || tokens[operator].type == NEGATIVE || tokens[operator].type == HEX || tokens[operator].type == REG)
					  operator = j;
			  }
			  if(tokens[j].type == '*' || tokens[j].type == '/'){
				  if(tokens[operator].type == NEGATIVE || tokens[operator].type == POINTER || operator == p || tokens[operator].type == '*' || tokens[operator].type == '/' || tokens[operator].type == HEX || tokens[operator].type == REG){
					  operator = j;
				  }
		      }
			  if(tokens[j].type == NEGATIVE){
				  if(operator == p || tokens[operator].type == NEGATIVE || tokens[operator].type == POINTER || tokens[operator].type == HEX || tokens[operator].type == REG)
					  operator = j;
			  }
			  if(tokens[j].type == POINTER){
				  if(operator == p || tokens[operator].type == POINTER || tokens[operator].type == HEX || tokens[operator].type == REG){
					  operator = j;
				  }
			  }
			  if(tokens[j].type == REG){
				  if(operator == p || tokens[operator].type == HEX)
					  operator = j;
			  }
			  if(tokens[j].type == HEX){
				  if(operator == p)
					  operator = j;
			  }
	      }
		  //printf("op = %d", operator);
	  }

	  int op = operator;
//	  printf("operator = %d\n", op);
	  int val1 = 0;
	  if(tokens[op].type != NEGATIVE && tokens[op].type != POINTER && tokens[op].type != HEX && tokens[op].type != REG){
//	  printf("operator = %d\n", op);
		val1 = eval(p, op - 1);
	  }
	  int val2 = eval(op + 1, q); 
/*	  if(tokens[op].type == NEGATIVE){
		  if(op != 0){
		  int val1 = eval(p, op - 2);
		  int val2 = eval(op + 1, q);
		  }
	  }*/

      switch (tokens[op].type) {
/*		case REG:{
					 int val;
					 char s[4];
					 char *ss;
					 ss = s;
					 strcpy(ss, regs[val2]);
					 printf("%s\n", s);
					 val = isa_reg_str2val(s, NULL);
					 return val;
				 }*/
//		case HEX: return val2;
		case NEGATIVE: return -1*val2;
		case POINTER: {
						  return paddr_read(val2, 4);
					  }
		case TK_AND: return val1 && val2;
		case TK_EQ: return val1 == val2;
		case TK_NEQ: return val1 != val2;
        case '+': return val1 + val2;
        case '-': return val1 - val2;
        case '*': return val1 * val2;
        case '/': 
				  if(val2 == 0){
					  printf("division by zero\n");
					  if (divzeroflag == 0){
						  divzeroflag = 1;
						  divzeronum++;
					  }
					  return 0;
				  }
				  return val1 / val2;
        default: assert(0);
	}
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  divzeroflag = 0;
  unsigned int result;
  //size = sizeof(e);
  //printf("size = %d\n", size);
  result = eval(0, nr_token-1);
  printf("result = %u\n", result);
  for(int i = 0; i < nr_token; i++){
	  memset(tokens[i].str, 0, sizeof(tokens[i].str));
	  tokens[i].type = 0;
  }
  return result;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
