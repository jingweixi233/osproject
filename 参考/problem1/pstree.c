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
#define MAXBUF 1024
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

int size = 0;		// the size of tasks
struct prinfo tasks[MAXBUF];

void copy_pstree(struct prinfo *buf, struct prinfo *tasks, int i)
{
    strcpy(buf[i].comm, tasks[i].comm);
    buf[i].pid = tasks[i].pid;
    buf[i].state = tasks[i].state;
    buf[i].parent_pid = tasks[i].parent_pid;
    buf[i].first_child_pid = tasks[i].first_child_pid;
    buf[i].next_sibling_pid = tasks[i].next_sibling_pid;
    buf[i].uid = tasks[i].uid;
    buf[i].deep = tasks[i].deep;
}

// copy data from task_struct to prinfo with deep d 
void copyTask(struct task_struct *task, struct prinfo *target, int depth)
{
    struct task_struct *now_task;
    struct list_head *tl; 

    if (task->comm == NULL)
	return;
    size++; 

    target->parent_pid = task->parent->pid;
    target->pid = task->pid;
    target->state = task->state;
    target->uid = task->cred->uid;
    target->deep = depth;
    get_task_comm(target->comm, task);
    
    if (!list_empty(&task->children))
    {
        now_task = list_entry((&task->children)->next, struct task_struct, sibling);
        if (now_task == NULL)
            target->first_child_pid = 0;
        else
            target->first_child_pid = now_task->pid;
    }
    else
        target->first_child_pid = 0;
    target->next_sibling_pid = 0;
    if (list_empty(&task->sibling))
    {
        list_for_each(tl, &task->parent->children)
        {
            now_task = list_entry(tl->next, struct task_struct, sibling);
            if (now_task->pid == task->pid)
            {
                now_task = list_entry(tl->next, struct task_struct, sibling);
                if (now_task == NULL)
                    target->next_sibling_pid = 0;		
                else
                    target->next_sibling_pid = current->pid;
                break;
            }
        }
    }
}

void dfs(struct task_struct *start, int deep)
{
    struct task_struct *now_task;
    struct list_head *tl; 
    if (start == NULL) return;
    copyTask(start, &tasks[size], deep);
    if (list_empty(&start->children))
	return;
    list_for_each(tl, &start->children)
    {
        now_task = list_entry(tl, struct task_struct, sibling);
        dfs(now_task, deep + 1);
    }
    return;
}

static int (*oldcall)(void);
static int pstree(struct prinfo *buf, int *nr)
{
    int i = 0;
    read_lock(&tasklist_lock);
    printk("Travell the ptree!!\n");
    dfs(&init_task, 0);
    printk("Ptree finish!!\n");
    read_unlock(&tasklist_lock);
    *nr = size;
    for (i = 0; i < size; ++i)
	copy_pstree(buf, tasks, i);
    return 0;
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
