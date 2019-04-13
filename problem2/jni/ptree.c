#include <stdio.h>
#define N 10000
struct prinfo{
	pid_t parent_pid; 		    // process id of parent
	pid_t pid;			        // process id
	pid_t first_child_pid;	    // pid of youngest child
	pid_t next_sibling_pid; 	// pid of older sibling
	long state;			        // current state of process
	long uid;			        // user id of process owner
	char comm[64];		        // name of program executed
	int deep;
};

int main(){
	int i,j;
	struct prinfo buf[N];
	int nr;
	int z = syscall(356,buf,&nr);
	printf("The number of task is %d!\n",nr);
	printf("Print start\n");
	for (i = 0; i < nr; i++)
	{
		for(j = 0; j < buf[i].deep; j++){
			printf("\t");
		}
		printf("%s, %d, %ld, %d, %d, %d, %ld\n", buf[i].comm, buf[i].pid, buf[i].state, buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
	}
	printf("Print end\n");

	return 0;
}
