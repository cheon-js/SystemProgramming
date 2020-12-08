#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFSIZE 100// 수신된메시지를전체 client에게전송하며, 연결이끊어지면 client hanle을소멸시킨다.

void * clnt_connection(void *arg);// 메시지를모든 client에게전송한다.
void send_message(char * message, int len);// 에러발생에대한처리
void error_handling(char *message);
int clnt_number = 0;        // 연결된 client의개수를저장할변수선언
int clnt_socks[10];         // 연결된모든 client 모두에게메시지를전송하여야하기때에                            // 모든 client 소켓의정보를전역변수로선언함문
pthread_mutex_t     mutx;   // mutex 동기화하기위한 mutex선언
int main(int argc, char *argv[]){
    int     serv_sock;                  // server socket handle 선언
    int     clnt_sock;                  // cleint socket handle 선언
    struct  sockaddr_in     serv_addr;  // server socket address 구조체선언
    struct  sockaddr_in     clnt_addr;  // client socket address 구조체선언
    int     clnt_addr_size;             // client socket address 구조체의크기
    pthread_t   thread;                 // pthread_t 자료형선언(thread ID)
    void     *thread_rtn;
    // port번호인자값확인
    if(argc!=2){
        printf("Usage : %s <port> \n", argv[0]); 
	exit(1);
    }
    // mutex 초기화
    if(pthread_mutex_init(&mutx, NULL))
        error_handling("pthread_mutex_init() error");
    // server socket handle 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    // server socket address 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons((unsigned short)atoi(argv[1]));
    // server socket handle에  server address 정보 bind
    if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) ==-1)
        error_handling("bind() error");    // server socket handle의연결대기상태로변경
    if(listen(serv_sock, 10) == -1)
        error_handling("listen() error");
    while(1){
        // 연결요청시사용될 client socket address의크기설정
        clnt_addr_size = sizeof(clnt_addr);
        // 연결을수락하여 client socket handle을생성한다.
        // 이때 client socket address 정보도함께받아진다.
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
        // 전역으로선언된변수가임계영역으로다른 thread의영향을받으므로 mutexlock을건다.
        pthread_mutex_lock(&mutx);
        // 연결되어생성된 client handle을 client handle소켓배열에저장한다.
        clnt_socks[clnt_number++]=clnt_sock;
        // mutex unlock으로해제한다.
        pthread_mutex_unlock(&mutx);
        // 연결된 client 소켓의 handle을 clnt_connection함수호출시인자로넘겨주어 thread를생성한다.
        pthread_create(&thread, NULL, clnt_connection, (void *)clnt_sock);
        printf("New client connected, IP: %s \n",inet_ntoa(clnt_addr.sin_addr));
    }
    pthread_join(thread, &thread_rtn);
    // mutex를해제한다.
    pthread_mutex_destroy(&mutx);
    return EXIT_SUCCESS;
}
void * clnt_connection(void * arg){
    int clnt_sock = (int)arg;
   // 인자로넘겨받은 client handle을형변환을하여새로운변수로저장한다.
    int str_len = 0;            // client로부터수신한메시지의길이를저장하는변수선언
    char message[BUFSIZE];      // client로부터수신된메시지를저장하는변수선언
    int i;
    // client로부터메시지를수신받는다.
    // 수신된메시지의길이가 0인경우는 client의연결종료를의미하며, 루프를빠져나온다.
    while( (str_len=read(clnt_sock, message, sizeof(message))) != 0)
        send_message(message, str_len);  // 수신된메시지를모든클라이언트에전송한다.
    // 전역으로선언된변수가임계영역으로다른 thread의영향을받으므로 mutexlock을건다 
   // lock의위치를 for문밖에서선언하고있다.
    // for문안에선언시수시로반복되기때문에실행속도에영향을미친다.
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_number; i++){
        // 연결종료된 client handle값을 clent배열의값과같으면
        if(clnt_sock == clnt_socks[i]){
            // 그위치를시작점으로하여뒤의소켓정보로한개씩앞으로이동시킨다.
            // 즉, 현재연결종료된 client의 handle 정보는삭제가된다.
            for( ; i<clnt_number-1; i++)
                    clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_number--;    // mutex unlock으로해제한다.
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return 0;
}
// 현재연결되어있는모든 client에게메시지를전송한다.
void send_message(char * message, int len){
    int i;
    // 단순히참조하는과정에서도전역변수로선언된임계영역의변수는변경될수있기때문에
    // mutex lock을설정하여야한다.
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_number; i++)        // 연결된 client socket handle수만큼반복하여
        // 모든 client에게같은메시지를전송한다.
        write(clnt_socks[i], message, len);
    pthread_mutex_unlock(&mutx);
}
void error_handling(char * message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
