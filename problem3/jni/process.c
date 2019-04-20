//xi jingwei
//oprating system problem3

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

char myID[20] = "517030910116";
int main(){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		printf("%s Child pid = %d\n", myID, getpid());
		execl("/data/misc/ospro/ptreeARM", "ptreeARM", NULL);
	}
	else{
		printf("%s Parent pid = %d\n", myID, getpid());
	}
	return 0;
}
