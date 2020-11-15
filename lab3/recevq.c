#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BSIZE 16
#define QKEY (key_t)0111

struct msg_data{

	long type;
	char text[BSIZE];
};

struct msg_data recv_data;

int main() {

	int qid, len;
	if((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
		perror("msgget failed");
		exit(1);
	}

	if((len = msgrcv(qid, &recv_data, BSIZE, 0, 0)) == -1){
		perror("msgrcv failed");
		exit(1);
	}

	printf("received from message queue: %s\n",recv_data.text);
	
	if(msgctl(qid, IPC_RMID, 0) == -1) {

		perror("msgctl failed");
		exit(1);
	}
}
