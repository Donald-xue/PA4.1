#include "sdb.h"

#define NR_WP 32

extern word_t expr(char *e, bool *success);

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[65536];
  unsigned int result;
  int hittime;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *ex){
	if(free_ == NULL){
		printf("No room for new watchpoint!\n");
		assert(0);
	}
	bool success = true;
	WP *wp = NULL;
	wp = head;
	unsigned int res;
	res = expr(ex, &success);
	if(success == false){
		wp = NULL;
		return wp;
	}
	int i = 1;
	if(wp == NULL){
		wp = free_;
		free_ = free_->next;
		wp->next = NULL;
		head = wp;
	}
	else{
		WP * test = head;
		if(!strcmp(test->expr, ex)){
			printf("Already watch this expression!\n");
			return test;
		}
		while(test->next){
			if(strcmp(test->expr, ex) == 0){
				printf("Already watch this expression!\n");
				return test;
			}
			test = test->next;
		}
		i++;
		while (wp->next != NULL){
			wp = wp->next;
			i = wp->NO + 1;
		}
		wp->next = free_;
	    free_ = free_->next;
	    wp = wp->next;
	    wp->next = NULL;
	}
//	head -> next = wp;
	wp->NO = i;
	strcpy(wp->expr, ex);
	wp->result = res;
	return wp;
}

void free_wp(WP *wp){
	if(head == NULL){
		printf("No watchpoint needed to be free!\n");
	}
	WP *p = head, *q = head;
	while(p != NULL){
		if(p -> NO == wp -> NO){
			if(p == head){
				head = p->next;
				p->next = free_->next;
				free_ = p;
				free_ -> result = 0;
				memset(free_->expr, 0, 65536);
				free_->hittime = 0;
			}
			else{
				q->next = p->next;
				p->next = free_->next;
				free_ = p;
				free_ -> result = 0;
				memset(free_->expr, 0, 65536);
				free_->hittime = 0;
			}
			break;
		}
		q=p;
		p=p->next;
	}
}

int checkwp(){
	WP *wp;
	wp = head;
	int flag = 0;
	unsigned int i; 
	bool success = true;
/*	if(wp != NULL){
		i = expr(wp->expr, &success);
		if(i != wp->result){
			if(flag == 0)
				printf("watchpoint No.\texpression\tresult\n");
			printf("\t%d\t%s\told: %u new: %u", wp->NO, wp->expr, wp->result, i);
			wp->result = i;
			flag = 1;
		}
	}*/
	while(wp != NULL){
		i = expr(wp->expr, &success);
		if(i != wp->result){
			if(flag == 0)
				printf("watchpoint No.\texpression\tresult\n");
			printf("   \t%d\t%s\told: %u new: %u\n", wp->NO, wp->expr, wp->result, i);
			wp->hittime++;
			printf("Hit %d times!\n", wp->hittime);
			wp->result = i;
			flag = 1;
		}
		wp = wp->next;
	}
	if(flag == 1){
		flag = 0;
		return 1;
	}
	return 0;
}

void insertwp(char *arg){
	WP *wp;
	wp = new_wp(arg);
	if(wp == NULL){
		printf("Invaild expression!\n");
		return;
	}
	printf("Set watchpoint No. %d, expr = %s, result = %u\n", wp->NO, wp->expr, wp->result);
}

void displaywp(){
	if(head == NULL){
		printf("No watchpoint!");
		return;
	}
	WP *wp, *q;
	wp = head;
	q = head;
	printf("watchpoint No.\texpression\tresult\n");
	printf("   \t%d\t%s\t%u\n", wp->NO, wp->expr, wp->result);
	wp = head -> next;
	while(q->next != NULL){
		printf("   \t%d\t%s\t%u\n", wp->NO, wp->expr, wp->result);
		q = wp;
		wp = wp->next;
	}
}

void deletewp(int No){
	WP * wp;
	wp = head;
	while(wp != NULL){
		if(No == wp->NO){
			printf("Delete watchpoint No. %d.\n", No);
			free_wp(wp);
		}
		wp = wp->next;
	}
}
