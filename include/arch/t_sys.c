#include <pthread.h>
#include <errno.h>

#include "t_common.h"

int t_sys_thread_new(void (*task)(void* arg),uint8_t* name,int priority,int stacksize)
{
	pthread_t tid;
	int ret;


	void* (*thrd)(void*);
	thrd = (void*(*)(void*))task;
	if (pthread_create(&tid,NULL,thrd,NULL)){
		perror("pthread_create");
		ret = -1;
	}

	return ret;
}

t_sys_mbox_t t_sys_mbox_new(int count)
{
	static key_t key = 100;
	key += 1;
	int msgid = msgget(key,IPC_CREAT | 0666);

	if (msgid < 0){
		perror("msgget");
		ERROR(("t_sys_mbox_new failed"));
		msgid = T_SYS_MBOX_NULL;
	}
	return msgid;
}

int t_sys_mbox_post(t_sys_mbox_t mbox, void  * msg)
{
	int ret = 0;
	int sendsize = sizeof(void*);
	static int temp =0;
	static int* temp2= &temp;

	if ( mbox == T_SYS_MBOX_NULL){
		ret = -1;
		ERROR(("mbox_post:mbox is empty"));

	}else if (msgsnd(mbox,(msg == NULL ? (void*)&temp2: (&msg)),sendsize,0)){
		ret = -1;
		perror("msgsnd");
		ERROR(("msgsnd failed %p",msg));
	}

	return ret;
}

int t_sys_mbox_fetch(t_sys_mbox_t mbox, void** msg,int timeout)
{
	int ret = 0;

	int recvsize = sizeof(void*);
	static int temp;
	static int* temp2 = &temp;

	if ( mbox == T_SYS_MBOX_NULL){
                ret = -1;
                ERROR(("mbox_fetch:mbox is empty"));

        }else if (msgrcv(mbox,(msg == NULL?(void*)&temp:msg),recvsize,0,0) !=recvsize){
		ret = -1;
		perror("msgrcv");
		ERROR(("msgrcv failed"));
	}
	return ret;
}

static  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void TASKSUSPENDALL()
{

	if (pthread_mutex_lock(&lock)){
		perror("pthread_mutex_lock");
		ERROR(("pthread_mutex_lock failed"));
	}
}

void TASKRESUMEALL()
{
	if (pthread_mutex_unlock(&lock)){
		perror("pthread_mutex_unlock");
		ERROR(("pthread_mutex_unlock failed"));
	}
}
