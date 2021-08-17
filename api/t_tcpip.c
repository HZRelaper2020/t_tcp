#include "t_common.h"

static t_sys_mbox_t mbox;

//#define t_sys_mbox_fetch(a,b,c) t_tcpip_timeout_mbox_fetch(a,b,c) 

static int t_tcpip_timeout_mbox_fetch(t_sys_mbox_t* mbox,struct t_tcpip_msg** msg ,int timeout)
{
	return t_sys_mbox_fetch(mbox,msg,timeout);
}

static void t_tcpip_task(void* arg)
{


	while(1){
		struct t_tcpip_msg *msg=NULL;
		if (!t_tcpip_timeout_mbox_fetch(&mbox,&msg,1000)){
			switch(msg->type){
				case T_TCPIP_MSG_API:
					break;
				case T_TCPIP_MSG_INPUT:
					// judge arp or ip
					t_raw_input(msg->msg.inp.p,msg->msg.inp.netif);
					break;
				case T_TCPIP_MSG_CALLBACK:
					break;
			}
		}
		if (msg){
			t_memp_free(T_MEMP_TCPIP_MSG,msg);
		}
	}
}

int t_tcpip_init()
{
	mbox = t_sys_mbox_new(0xffff);
	t_sys_thread_new(t_tcpip_task,1,4096);
	return 0;
}

t_err_t t_tcpip_input(struct t_pbuf*p,struct t_netif* inp)
{
	int ret = 0;
	struct t_tcpip_msg * msg = t_memp_malloc(T_MEMP_TCPIP_MSG);
	if (msg == NULL){
		t_pbuf_free(p);
		ERROR(("t_tcpip_input: no memory msg"));
		ret = -1;
	}

	if (!ret){
		msg->type = T_TCPIP_MSG_INPUT;
		msg->msg.inp.p = p;
		msg->msg.inp.netif = inp;
		if (t_sys_mbox_post(&mbox,msg)){
			ERROR(("t_sys_mbox_post failed"));
			ret = -1;
		}
	}

	return ret;
}
