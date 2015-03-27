#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>

int main() {

	int msqid = msgget(85,  0);
	long buf[1024];

	while(2) {
		if( msgrcv(msqid, buf,1024*sizeof(long),0,0) >= 0 ) {
			printf("msgrcv: (type: %d, val:\"%s\")\n",buf[0],(char*)(buf+1));
}
	}

	return -3;
}
