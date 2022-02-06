#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  //'+', '-', '*', '/', '(', ')',
  NUMBER,
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
  {"\\b[0-9]\\b", NUMBER}, // number
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

static Token tokens[32] __attribute__((used)) = {};
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
			  tokens[nr_token].type=5;
			  strncpy(tokens[nr_token].str, substr_start, substr_len);
			  nr_token++;
			  break;
		  case '-':
              tokens[nr_token].type=6;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
			  nr_token++;                                                                                                          
              break;
		  case '*':
              tokens[nr_token].type=3;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;
		  case '/':
              tokens[nr_token].type=4;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;
     	  case '(':
              tokens[nr_token].type=1;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;  
          case ')':
              tokens[nr_token].type=2;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
              nr_token++;
              break;  
		  case NUMBER:
			  tokens[nr_token].type=7;
              strncpy(tokens[nr_token].str, substr_start, substr_len); 
			  nr_token++;
			  break;
          default: TODO();
        }
        //printf("type=%d, str=%s\n", tokens[nr_token-1].type, tokens[nr_token-1].str);
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q){
	int i = p, left = 0, right = 0;
	if( tokens[i].type != '(' || tokens[i].type != ')' )
		return false;
	else{
		for( ; i != q; i++){
			if(left == right || tokens[i].type == ')')
				return false;
			if(tokens[i].type == '(')
				left++;
			else if(tokens[i].type == ')')
				right++;
			else continue;
		}
		if(i == q && left == right)
			return true;
		else 
			return false;
	}
}

int eval(int p, int q) {
  if (p > q) {
	  printf("wrong arguement for eval(p, q)\n");
	  assert(0);
    /* Bad expression */
  }
  else if (p == q) {
	  if(tokens[p].type == NUMBER){
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
  else {
	  int j = p, operator = -1, num = 0;
	  for( ; j != q; j++){
		  if (tokens[j].type == '(')
			  num++;
		  if (tokens[j].type == ')')
			  num--;
		  if (num == 0){
			  if (tokens[j].type == '+' || '-' || '*' || '/'){
				  if(tokens[j].type == '+' || '-')
					  operator = j;
				  else
					  if(operator == -1 || tokens[operator].type == '*' || tokens[operator].type == '/')
					  operator = j;
		      }
	      }
	  }

	  int op = operator;
      int val1 = eval(p, op - 1);
      int val2 = eval(op + 1, q);

      switch (tokens[op].type) {
        case '+': return val1 + val2;
        case '-': return val1 - val2;
        case '*': return val1 * val2;
        case '/': return val1 / val2;
        default: assert(0);
	}
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  int size, result;
  size = sizeof(e);
  result = eval(0, size-1);
  printf("result = %d\n", result);
  return result;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
