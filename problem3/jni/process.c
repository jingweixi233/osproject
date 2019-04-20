//xi jingwei
//oprating system problem3

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>


int main(){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		printf("Child pid = %d\n", getpid());
		execl("/data/misc/ospro/ptreeARM", "ptreeARM", NULL);
	}
	else{
		printf("Parent pid = %d\n", getpid());
	}
	return 0;
}
