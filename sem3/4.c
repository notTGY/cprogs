#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<errno.h>
#define BS 4096


int main(int argc, char **argv) {
	key_t key = ftok("./1.c", 0);
	key_t key_sem = ftok("./1.c", 2);
	int fd;
	int semid = semget(key_sem, 2, IPC_CREAT | 0666);
	struct sembuf op;

	int id = shmget(key, BS, IPC_CREAT | 0666);
	char *array = shmat(id, NULL, 0);
	char *buf = malloc(BS * sizeof(char));

	if (argc != 2) {
		printf("use %s [filename]", argv[0]);
		return 0;
	}
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perror("file desc open");
		exit(errno);
	}
	while(read(fd, buf, BS)) {
		op.sem_num = 1;
		op.sem_op = -1;
		op.sem_flg = 0;
		semop(semid, &op, 1);

		memcpy(array, buf, BS);

		op.sem_num = 0;
		op.sem_op = 1;
		op.sem_flg = 0;
		semop(semid, &op, 1);
	}
	shmdt(array);
	return 0;
}
