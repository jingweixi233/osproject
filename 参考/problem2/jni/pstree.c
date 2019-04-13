#include <stdio.h>
#define MAXBUF 60000
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
  struct prinfo buf[MAXBUF];
  int nr;
  int z = syscall(287,buf,&nr);
  printf("The process number is %d!\n",nr);

  int i = 0, j;
  printf("Let's print:\n");
  for (i = 0; i < nr; ++i)
  {
	j = 0;
	while((j++) < buf[i].deep)
 	    printf("\t");
	printf("%s, %d, %ld, %d, %d, %d, %ld\n", buf[i].comm, buf[i].pid, buf[i].state,
	buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
  }
  printf("End the print!\n");
  return 0;
}
