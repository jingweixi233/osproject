Test your ptree system call in problem1

Problem 2

Write a simple C program which calls ptree

Print the entire process tree (in DFS order) using tabs to indent children with respect to their parents.
The output format of every process is:

printf(/* correct number of \t */); printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);

The detailed information is in ppt.
