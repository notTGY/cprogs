#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/sem.h>


int main() {
	key_t key = ftok("./1.c", 0);
	key_t key_sem = ftok("./1.c", 2);
	int semid = semget(key_sem, 2, 0666);
	struct sembuf op;
	op.sem_num = 1;
	op.sem_op = 1;
	op.sem_flg = 0;
	semop(semid, &op, 1);

	int id = shmget(key, 4096, 0666);
	while(1) {
		op.sem_num = 0;
		op.sem_op = -1;
		op.sem_flg = 0;
		semop(semid, &op, 1);
		
		char *array = shmat(id, NULL, 0);
		printf("%s", array);
		shmdt(array);

		op.sem_num = 1;
		op.sem_op = 1;
		op.sem_flg = 0;
		semop(semid, &op, 1);
	}
	return 0;
}
