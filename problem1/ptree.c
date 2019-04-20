//Xi Jingwei
// emulator -avd os1 -kernel /usr/lib/oskernel/kernel/goldfish/arch/arm/boot/zImage -show-kernel

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
#define __NR_ptree 356
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

struct prinfo tasks[N];
int k = 0;

static int (*oldcall)(void);
void dfs(struct task_struct *node, int deep){
    struct task_struct *temptask;
    struct list_head *temp;
    struct prinfo *temp_prinfo;
    
    if(node == NULL || node -> comm == NULL){
        return;
    }

    temp_prinfo = &tasks[k];

    temp_prinfo -> parent_pid = node -> parent -> pid;
    temp_prinfo -> pid = node -> pid;
    temp_prinfo -> state = node -> state;
    temp_prinfo -> uid = node -> cred -> uid;
    get_task_comm(temp_prinfo -> comm, node);
    temp_prinfo -> deep = deep;
    temp_prinfo -> first_child_pid = 0;
    temp_prinfo -> next_sibling_pid = 0;

    k++;

    if(!list_empty(&node -> sibling)){
	temp = (&node -> sibling) -> next;
	temptask = list_entry(temp, struct task_struct, sibling);
	temp_prinfo -> next_sibling_pid = temptask -> pid;
/*
        list_for_each(temp, &node -> sibling){
            temptask = list_entry(temp, struct task_struct, sibling);
            if(temptask -> pid == node -> pid){
                temptask = list_entry(temp -> next, struct task_struct, sibling);
                temp_prinfo -> next_sibling_pid = temptask -> pid;
            }
        }
*/
    }

    if(!list_empty(&node -> children)){
	temptask = list_entry((&node -> children) -> next, struct task_struct, sibling);
        temp_prinfo -> first_child_pid = temptask -> pid;
        list_for_each(temp, &node -> children){
            temptask = list_entry(temp, struct task_struct, sibling);
            dfs(temptask, deep + 1);
        }
    }

    
    
}

int ptree(struct prinfo *buf, int *nr){

    int i;
    printk("start to dfs\n");
    dfs(&init_task, 0);
    printk("dfs end\n");

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
    return 0;
}

static int addsyscall_init(void)
{
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree] = (unsigned long)ptree;
    printk(KERN_INFO "module load!\n");
    return 0;
}
static void addsyscall_exit(void)
{
    long *syscall = (long*)0xc000d8c4;
    syscall[__NR_ptree] = (unsigned long)oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
