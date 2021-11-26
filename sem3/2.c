#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>


int main() {
	key_t key = ftok("./1.c", 0);
	int id = shmget(key, 4096, IPC_CREAT | 0666);
	char *array = shmat(id, NULL, 0);
	snprintf(array, 8, "Hello!\n");
	shmdt(array);
	return 0;
}
