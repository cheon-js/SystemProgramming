#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PASSWORDSIZE 12
#define SIZE 3

int main(void) {
 int fd;
 int nread, cnt=0, errcnt=0;
 char ch;
 char* text[SIZE] = {"I like coding, but i don't know how to coding.","Good!!","Coding is happy"};
 int i=0;
 time_t start_time, end_time;
 float res_time;
 int typing_count = 0;

struct termios init_attr, new_attr;
 fd = open(ttyname(fileno(stdin)), O_RDWR);
 tcgetattr(fd, &init_attr);
 new_attr = init_attr;
 new_attr.c_lflag &= ~ICANON;
 new_attr.c_lflag &= ~ECHO; 
 new_attr.c_cc[VMIN] = 1;
 new_attr.c_cc[VTIME] = 0;
 if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
   fprintf(stderr, "터미널속성을 설정할 수 없습니다.\n");
 }
 printf("다음문장을그대로입력하세요.\n");
 time(&start_time);
 for(i=0; i<SIZE; i++){
   cnt = 0;
   printf("%s\n", text[i]);
   while ((nread=read(fd, &ch, 1)) > 0 && ch != '\n') {
	if (ch == text[i][cnt++])
		write(fd, &ch, 1);
	else {write(fd, "*", 1);
		errcnt++;}typing_count++;
   }
	printf("\n");
 }
 printf("\n타이핑오류의횟수 : %d\n", errcnt);
 time(&end_time);
 res_time = (float)(end_time-start_time);
 printf("올바른타이핑횟수 : %d회\n", typing_count);
 printf("총걸린시간 : %f 초\n", res_time);
 printf("평균분당타자수 : %.2f타\n", (float)((60*(typing_count-errcnt))/res_time));
 tcsetattr(fd, TCSANOW, &init_attr); close(fd);
}









