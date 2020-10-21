#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

struct Student {

	char s_name[10];
	int m_score;
	int f_score;
};

struct Student *root = NULL;

int compare(const void *cp1, const void *cp2) {
	return strcmp(((struct Student*)cp1)-> s_name,((struct Student*)cp2)
		->s_name);
}

void print_node(const void *, VISIT, int);

int main()
{
	int i, s_number;
	struct Student **ret;

	printf("학생 수를 입력하세요 : ");
	scanf("%d", &s_number);

	if(s_number <=0 || s_number == NULL)
		abort();

	struct Student *tree[s_number];
	for(i = 0; i< sizeof(tree) / sizeof(struct Student*); i++){
		tree[i] = malloc(sizeof(struct Student *));
		printf("추가할 사용자 정보를 입력해주세요(이름, 중간점수, 기말점수)\n");
		scanf("%s %d %d", tree[i]->s_name, &tree[i]->m_score, &tree[i]->f_score);

		ret = (struct Student **) tsearch((void *) tree[i], (void**) &root, compare);
		printf("\"%s\"이(가)", (*ret)->s_name);

		if(*ret == tree[i])
			printf("트리에 추가되었습니다.\n");
		else
			printf("이미 존재합니다.\n");

	}
	twalk((void *) root, print_node);

	for(int i=0; i<sizeof(tree) / sizeof(struct Student *); i++)
	free(tree[i]);
}
	void print_node(const void *nodeptr, VISIT order, int level){
	
	if(order == preorder || order == leaf)
	printf("이름 = %-10s, 중간 점수 = %d  기말 점수 = %d\n",(*(struct Student **)nodeptr)->s_name, (*(struct Student **)nodeptr)->m_score, (*(struct Student **)nodeptr)->f_score);
}
