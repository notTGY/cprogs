#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<errno.h>
#include<pthread.h>
#define S 8

struct mbuf { long mtype; };

struct ip { int i; };

int qid;
struct ip pdata[S];

void* threader(void *data) {
	int i = ((struct ip*)data)->i;
	struct mbuf buf;
	printf("th %d waiting\n", i);
	if (msgrcv(qid, &buf, 0, i, 0) < 0) {
		perror("failed to read msg");
		exit(errno);
	}
	buf.mtype = i + 1;
	printf("th %d running, next %ld\n", i, buf.mtype);
	fflush(stdout);
	if (msgsnd(qid, &buf, 0, 0) == -1) {
		perror("failed to send msg from slave");
		exit(errno);
	}
	pthread_exit(NULL);
}

int main() {
	unsigned long thid[S];
	struct mbuf buf;
	qid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
	if (qid == -1) {
		perror("failed to get msg queue");
		exit(errno);
	}
	for (int i = 0; i < S; i++) {
		pdata[i].i = i + 1;
		if (pthread_create(&thid[i], NULL, threader, (void*)&pdata[i]) != 0) {
			perror("failed to create thread");
			exit(errno);
		}
	}
	buf.mtype = 1;
	if (msgsnd(qid, &buf, 0, 0) == -1) {
		perror("failed to send msg from master");
		exit(errno);
	}
	
	printf("master thread waits\n\n");
	if (msgrcv(qid, &buf, 0, S + 1, 0) < 0) {
		perror("failed to read msg from master");
		exit(errno);
	}
	printf("threads finished, received %d\n", S + 1);

	for (int i = 0; i < S; i++) {
		if (pthread_join(thid[i], NULL) != 0) {
			perror("failed to join thread");
			exit(errno);
		}
	}
	return 0;
}
