#ifndef SOURCE_T_SYS__H
#define SOURCE_T_SYS__H

#include <semaphore.h>

typedef int t_sys_mbox_t;
typedef sem_t* t_sys_sem_t;

#define T_SYS_MBOX_NULL  -1
#define T_SYS_SEM_NULL	NULL

t_sys_mbox_t t_sys_mbox_new(int count);

int t_sys_mbox_post(t_sys_mbox_t mbox, void * msg);

int t_sys_thread_new(void (*task)(void* arg),uint8_t* name,int priority,int stacksize);

int t_sys_mbox_fetch(t_sys_mbox_t mbox, void** msg,int timeout);

void TASKSUSPENDALL();

void TASKRESUMEALL();

#endif
