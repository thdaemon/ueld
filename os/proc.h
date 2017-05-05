#ifndef __OS_PROC_H_
#define __OS_PROC_H_

int ueld_os_for_each_process(pid_t* pid);
int ueld_os_next_process(pid_t* pid);
int ueld_os_end_each_process();

#endif
