#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>         // for task_struct
#include<linux/unistd.h>
#include<linux/list.h>
#include<linux/string.h>
#include<linux/syscalls.h>
#include<linux/slab.h>          // for memory allocation
#include<linux/uaccess.h>       // for copy to user
MODULE_LICENSE("Dual BSD/GPL");
#define __NR_pstree 287
const int N = 1000;

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

struct prinfo tasks[N];
int k = 0;

void dfs(struct tast_struct *node, int deep){
    struct task_struct *temptask;
    struct list_head *temp;
    struct prinfo *temp_prinfo;
    
    if(node == NULL){
        return;
    }

    temp_prinfo = &task[k];
    temp_prinfo -> parent_pid = node -> parent -> pid;
    temp_prinfo -> pid = node -> pid;
    temp_prinfo -> state = node -> state;
    temp_prinfo -> uid = node -> cred -> uid;
    get_task_comm(temp_prinfo -> comm, node);
    temp_prinfo -> deep = deep;
    temp_prinfo -> first_child_pid = 0;
    temp_prinfo -> next_sibling_pid = 0;

    if(!list_empty(&node -> sibling)){
        list_for_each(temp, node -> sibling){
            temptask = list_entry(temp, task_struct, sibling);
            if(temptask -> pid = node -> pid){
                temptask = list_entry(temp -> next, task_struct, sibling);
                temp_prinfo -> next_sibling_pid = temptask -> pid;
            }
        }
    }

    if(!list_empty(&node -> children)){
        temp_prinfo -> first_child_pid = node -> children -> pid;
        list_for_each(temp, node -> children){
            temptask = list_entry(temp, task_struct, sibling);
            dfs(temptask, deep + 1);
        }
    }

    k++;
    
}

int ptree(struct prinfo *buf, int *nr){

    int i;
    printf("start to dfs\n");
    dfs(&init_task, 0);
    printf("dfs end\n");

    *nr = k;
    for(i = 0; i < k; i++){
        strcpy(buf[i].comm, tasks[i].comm);
        buf[i].pid = tasks[i].pid;
        buf[i].state = tasks[i].state;
        buf[i].parent_pid = tasks[i].parent_pid;
        buf[i].first_child_pid = tasks[i].first_child_pid;
        buf[i].next_sibling_pid = tasks[i].next_sibling_pid;
        buf[i].uid = tasks[i].uid;
        buf[i].deep = tasks[i].deep;
    }
}

static int addsyscall_init(void)
{
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_pstree]);
    syscall[__NR_pstree] = (unsigned long)pstree;
    printk(KERN_INFO "module load!\n");
    return 0;
}
static void addsyscall_exit(void)
{
    long *syscall = (long*)0xc000d8c4;
    syscall[__NR_pstree] = (unsigned long)oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);