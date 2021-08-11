#ifndef SOURCE_T_SYS__H
#define SOURCE_T_SYS__H

#include <semaphore.h>

typedef sem_t t_sys_mbox_t;

t_sys_mbox_t t_sys_mbox_new(int count);

int t_sys_mbox_post(t_sys_mbox_t* mbox, struct t_tcpip_msg * msg);

int t_sys_thread_new(void (*task)(void* arg),int priority,int stacksize);

int t_sys_mbox_fetch(t_sys_mbox_t* mbox, struct t_tcpip_msg** msg,int timeout);

void TASKSUSPENDALL();

void TASKRESUMEALL();

#endif
