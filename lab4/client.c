#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFSIZE 100     // 메시지전송최대길이
#define NAMESIZE 50     // 사용자명최대길이
void * send_message(void *arg);         // 메시지전송 thread 함수
void * recv_message(void *arg);         // 메시지수신 thread 함수
void   error_handling(char *message);   // 에러처리함수
char name[NAMESIZE] = "";   // 사용자명변수선언
char message[BUFSIZE];      // 송/수신할메시지변수선언
int main(int argc, char *argv[]){
    int sock;                           // 접속할서버서켓핸들
    struct sockaddr_in serv_addr;       // 접속할서버서켓의주소구조체
    pthread_t snd_thread, rcv_thread;   // 전송 thread ID, 수신할 thread ID생성(thread_t 자료형선언)
    void *thread_result;                // thread 함수에서반환되는반환값변수선언
    // 실행시입력인자값체크
    if(argc!=4){
        printf("Usage : %s <IP> <PORT> <name>\n", argv[0]);
        exit(1);
    }
    // 사용자명을복사
    strcpy(name, argv[3]);
    // 서버소켓핸들생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");
    // 서버소켓주소구조체초기화및설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    // 서버주소구조체정보를사용하여서버에연결요청
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
    // 메시지전송 / 메시지수신 thread 함수호출과함께 thread 생성
    // 인자값으로서버소켓핸들을전달하기때문에, 함수내에서 send / recv할수있다
    pthread_create(&snd_thread, NULL, send_message, (void *)sock);
    pthread_create(&rcv_thread, NULL, recv_message, (void *)sock);
    // 메시지전송 / 메시지수신 thread가끝났때까지대기함
    pthread_join(snd_thread, &thread_result);
    pthread_join(rcv_thread, &thread_result);
    //서버소켓종료함
    close(sock);
    return EXIT_SUCCESS;
}
// 메시지전송 thread 함수
void * send_message(void * arg){
    // 서버소켓핸들값을 int형으로형변환하여변수로선언.
    int sock = (int)arg;
    // 사용자명과메시지를저장할전송메시지내용을변수로선언
    char name_message[NAMESIZE+BUFSIZE];
    // 전송할메시지를입력을반복시킨다.
    while(1){
        // 전송할메시지입력을받아들인다.
        fgets(message, BUFSIZE, stdin);
        // 입력값이 "q\n"일경우 thread함수를빠져나와 실질적으로 main함수로돌아간다.
        if(!strcmp(message, "q\n")){
            close(sock);
             exit(0);
        }
        // 입력한메시지와사용자이름을조합하여전송할메시지를완성한다.
        strcpy(name_message, "[");
        strcat(name_message, name);
        strcat(name_message, "] ");
        strcat(name_message, message);
        // 조합된메시지를전송한다.
        write(sock, name_message, strlen(name_message));
    }
}
// 메시지수신 thread 함수
void * recv_message(void * arg){
    // 서버소켓핸들값을 int형으로형변환하여변수로선언
    int sock = (int)arg;
    // 사용자명과메시지를저장할수신메시지내용을변수로선언
    char name_message[NAMESIZE+BUFSIZE];
    int str_len;    // 수신메시지의길이를저장할변수선언
    int rtn;        // 연결종료후 thread함수를빠져나갈시반환되는값을변수로선언
    // 메시지읽기시도를계속반복한다.
    while(1){
        // 메시지를읽어들인다.
        str_len = read(sock, name_message, NAMESIZE+BUFSIZE-1);
        // 메시지의길이가 -1(연결종료)이면 thread함수를빠져나간다.
        if(str_len==-1)
        {
            rtn = 1;    // thread함수의반환값을 1로설정한다.
            // thread함수의반환형이 void* 형이기때문에 (void*)형으로현변환을한다.
            return (void *)rtn;
        }
        name_message[str_len] = 0;  // 수신된메시지의끝에 NULL종료문자를지정한다.
        fputs(name_message, stdout);    // 수신된메시지를표준출력(모니터)에출력한다.
    }
}
// 에러발생시처리할메시지처리함수
void error_handling(char * message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
