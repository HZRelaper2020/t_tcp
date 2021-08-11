#include <pthread.h>
#include <errno.h>

#include "t_common.h"


static t_sys_mbox_t mbox_list[100];

t_sys_mbox_t t_sys_mbox_new(int count)
{
	static int num =-1;
	num += 1;

	sem_t* sem = NULL;

	if (num >= sizeof(mbox_list)/sizeof(sem_t)){
		ERROR(("too much mbox_new"));
	}else{
		sem =  &mbox_list[num];
		if (sem_init(sem,1,count)){
			perror("sem_init");
			sem = NULL;
		}
	}

	return *sem;
}

static uint16_t msg_count = 0;
static uint32_t* msg_list[10];

int t_sys_thread_new(void (*task)(void* arg),int priority,int stacksize)
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

int t_sys_mbox_post(t_sys_mbox_t* mbox, struct t_tcpip_msg * msg)
{
	int ret = 0;
	if (sem_post(mbox)){
		perror("sem_post");
		ret = -1;
	}else{
		msg_list[msg_count] = (uint32_t*)msg;
		msg_count += 1;
	}
	return ret;
}

int t_sys_mbox_fetch(t_sys_mbox_t* mbox, struct t_tcpip_msg** msg,int timeout)
{
	int ret = 0;
	struct timespec tm = {timeout/1000,timeout%1000*1000};
	if (clock_gettime(CLOCK_REALTIME,&tm)){
		perror("clock_gettime");
		ret = -1;
	}

	tm.tv_sec += timeout/1000;
	tm.tv_nsec += timeout%1000*1000;

	//if (sem_timedwait(mbox, &tm)){
	if (sem_wait(mbox)){
		perror("sem_timewait");
		ret = -1;
	}else{
		if (msg_count > 0){
			*msg = (struct t_tcpip_msg*)msg_list[0];
			msg_count -= 1;
			for (int i=0;i<msg_count;i++){
				msg_list[i] = msg_list[i+1];
			}
		}else{
			ret = -1;
		}
	}
	return ret;
}

void TASKSUSPENDALL()
{
}

void TASKRESUMEALL()
{
}
