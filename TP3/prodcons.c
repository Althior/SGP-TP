#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int main() {
	char* buf;
	int shmid = shmget(IPC_PRIVATE, 5*sizeof(int), 0660);
	if( shmid < 0 )
		exit(-1);

	buf = (int*) shmat(shmid, 0, SHM_RND);
	
	int semid = semget(IPC_PRIVATE, 2, 0660);
	if( semid < 0 )
		exit(-1);

	if( semctl(semid,0,SETVAL,5) < 0 ) exit(-1);
	if( semctl(semid,1,SETVAL,0) < 0 ) exit(-1);
	
	struct sembuf sops[] = {
		{0,-1, 0},
		{0, 1, 0},
		{1,-1, 0},
		{1, 1, 0}
	};

	buf[0] = 1;

	int pid = fork();
	if( pid != 0 ) {
		// producteur
		int i = 0;
		while(1) {
			semop(semid, sops, 1);
			buf[i%5] = i;
			semop(semid, sops+3, 1);
			i++;
			i = i % 100;
		}
	} else {
		//consommateur
		int i = 0;
		while(1){

			semop(semid, sops+2, 1);
			
			printf("Consommée: %d\n", buf[i]);
			i++;
			i = i % 5;

			semop(semid, sops+1, 1);
		}
	}

}
