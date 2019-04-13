#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

char myID[10] = "5130309210";
int main()
{
	pid_t pid;
	pid = fork();
	if (pid == 0)
	{
		printf("%s Child PID=%d\n", myID, getpid());
		execl("/data/misc/pstreeARM", "pstreeARM", NULL); 
	}
	else if(pid > 0)
	{
		wait(NULL);
		printf("%s Parent PID=%d\n", myID, getpid());
	}
	else 
	{
		fprintf(stderr, "fork failed");
		return 1;
	}
	return 0;
}
