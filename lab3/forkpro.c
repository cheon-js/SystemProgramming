#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{

	pid_t pid;
	printf("Call fork\n");
	pid = fork();

	if(pid == 0)
		printf("나는 자식 프로세스\n");
	else if(pid >0)
		printf("나는 부모 프로세스\n");
	else
		printf("fork failed\n");
}
