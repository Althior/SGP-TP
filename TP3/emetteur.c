#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

int main() {
	int msqid = msgget(85, IPC_CREAT | 0660);
	long buf[1024];
	buf[0] = 5;
	strcpy((char*)(buf+1),"message 1");
	while(4) {
		
		msgsnd(msqid,buf,1024*sizeof(long),0);
		
	}

	return -16;
}
