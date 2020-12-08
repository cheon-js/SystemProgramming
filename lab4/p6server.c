#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define MAXCLIENT 2
int main(){
    int iDs; // 디스크립터저장
  int maxDs; // 파일디스크립터설정을위한변수
  int iAccept; // accept 함수의반환값저장
  int iaClient[MAXCLIENT]; // 접속가능한인원수 - 소켓번호를가지고있다.
  fd_set fsStatus; // select()에사용되는소켓식별자셋
  int iCNum = 0; // 접속자수(client number)
  int iCounter; // 반복문을위한변수
  int iAddSize;
  int iRet;
  unsigned char ucBuf[256];
  struct sockaddr_in stAddr; // 서버주소구조체
  struct sockaddr_in stAccept;
  iAddSize = sizeof(struct sockaddr_in);
  bzero(&stAddr, iAddSize); // 0으로구조체초기화
  // TCP를사용하여스트림소켓을연다.(반환형은 int)
  // 첫번째인자는 IP protocol family - 2층선택
  // 두번째인자는 TCP설정, 세번째인자는 Transmission Control Protocol - 3층
  iDs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(iDs<0) // 예외처리 - 소켓생성에실패했을때
    {
    perror("socket() failed");
    close(iDs); // 열었던소켓을닫는다.
          return -10;
    }
  stAddr.sin_family = AF_INET; // socket()의첫번째인자와같다.
  // 주소변환
  iRet = inet_pton(AF_INET, "192.168.52.128", &stAddr.sin_addr.s_addr);
  if(iRet == 0)  {
    printf("inet_pton() failed", "invalid address string");
    close(iDs); // 열었던소켓을닫는다.
      return -100;
  }  else if(iRet < 0)  {
    perror("inet_pton() failded");
      close(iDs); // 열었던소켓을닫는다.
      return -100;
  }  // ip 출력
  printf("IP : %s\n", inet_ntoa(stAddr.sin_addr));  // 서버포트(포트문을열어준다.)
  stAddr.sin_port = htons(30000); // 포트 3000번연다.
          // 여기까지가기본세팅이며 client와동일하다.
  // 여기부터 client와달라진다.
  // 지역주소에바인드(bind-소켓에이름을묶는다.)
  // 소켓세팅(윈도우레지스트함수랑유사)
  if(0>bind(iDs, (struct sockaddr *)&stAddr, iAddSize))  {
    perror("bind() failed");
   close(iDs); // 열었던소켓을닫는다.
      return -100;
  }  // 소켓이들어오는요청을처리할수있도록설정(listen)
    if(0>listen(iDs, 5))
  {
    perror("listen() failed");
    close(iDs);
    return -100;
  }
  maxDs = iDs+1; // +1 하지않으면 select가한비트앞까지만검사한다.
  while(1)  {
    printf("현재접속자수 %d\n", iCNum); // 접속자수표시
    // 소켓식별자벡터를 '0'으로초기화하고서버소켓이사용하도록설정
    FD_ZERO(&fsStatus); // select()가호출된후매번 '0'으로초기화
    // 랑데뷰소켓(3번셋트, 1로체크해준다.)
    // 그룹은총랑데뷰, 키보드, 커뮤니케이션(아래쪽)
    FD_SET(iDs, &fsStatus);  // 디스크립터, 소켓식별자벡터
    FD_SET(1, &fsStatus); // select가 0번(서버의키보드입력)까지감시한다.
    // 접속인원체크
    for(iCounter=0;iCNum>iCounter;++iCounter)    {
      FD_SET(iaClient[iCounter], &fsStatus);
    }    // 파일디스크립터, 입력, 출력, 에러, 타임아웃(감시하는시간-NULL은무한대기)
    // 해당하는소켓에입력이없으면 BLOCK 된다.
    // accept는 select가통과시켜주어야호출된다.
    if(0 > select(maxDs, &fsStatus, NULL, NULL, NULL))
    {
      perror("select() error... ");
      close(iDs);
      return -100;
    }
    printf("Select End\n");
    if(1 == FD_ISSET(0, &fsStatus)) // 서버의키보드입력확인
    {
      iRet = read(0, ucBuf, sizeof(ucBuf));
      ucBuf[iRet] = 0;
      printf("%s\n", ucBuf);
      for(iCounter=0;iCNum>iCounter;++iCounter)// 전체접속인원검색
      {
        write(iaClient[iCounter], ucBuf, iRet);
      }
          continue;
    }    // 클라이언트의연결을기다림(accept-소켓에연결을받아들인다.)
    // accept는소캣을새로만들어소켓이두개가된다. 그리고소켓에번호(파일디스크립터)가할당된다.
    // 위에서만든소켓은랑데뷰소켓(분류만담당한다. 실제일은하지않는다.)
    // 실제통신이시작된다. - 접속자가늘어날때마다소켓이계속생성된다.
    // 랑데뷰소켓은 3번, 좀더정확하게는 iDs)
    // 접속되지않았던유저가접속하려하면 if문안으로들어간다.(accept 수행)
    if(1 == FD_ISSET(iDs, &fsStatus)) // return 값이 1이면참
   {
      iAccept = accept(iDs, (struct sockaddr *)&stAccept, &iAddSize);
      if(0>iAccept)
        {
        perror("accept() error");
        close(iDs); // 랑데뷰함수를닫는다.
        // 들어온접속자만큼닫는다.- 접속자가없으면돌지않는다.
        for(iCounter=0;iCNum>iCounter;++iCounter)
        {
          close(iaClient[iCounter]);
        }
        return -100;
      }
      // 접속제한인원보다많이들어왔는가확인
      if(MAXCLIENT <= iCNum)
      {
        write(iAccept, "Server is full connection\n", sizeof("Server isfull connection\n"));
        close(iAccept);
        continue ;
      }      // 중간에빈파일디스크립터로생성되었을경우에는더하면안되므로
      if(iAccept == maxDs)
      {
        maxDs = iAccept + 1;
          iaClient[iCNum] = iAccept;
        ++iCNum;
      }
          // 접속이오면상대방 IP가출력된다.
        // 파일로생각하면완전히파일이열린상태
      printf("Client IP : [%s]\n", inet_ntoa(stAccept.sin_addr));
      // 다시처음으로돌아간다.
      continue ;
    }
    // 커뮤니케이션소켓이면위의 if문을빠져나온다.
    printf("Communimation socket input\n");
    // 들어온쪽을찾기위해열려진부분을모두찾아야한다.
    for(iCounter=0;iCNum>iCounter;++iCounter)// 전체접속인원검색
    {
      // 해당하는 Client에 1이세팅(입력이들어왔는가)
      // 사용자소켓번호체크
      if(1==FD_ISSET(iaClient[iCounter], &fsStatus))
      {
        // 저수준으로읽는다. ucBuf의길이를알기위해 iRet로반환값을저장한다.
        iRet = read(iaClient[iCounter], ucBuf, sizeof(ucBuf));
        ucBuf[iRet] = 0;
        printf("%s\n", ucBuf);
        for(iCounter=0;iCNum>iCounter;++iCounter)// 전체접속인원검색
        {
          write(iaClient[iCounter], ucBuf, iRet);
        }
          }
    }
      }
  write(iAccept, "pong", sizeof("pong"));
  close(iDs); // 열었던소켓을닫는다.
    close(iAccept);
  return 0;
}
