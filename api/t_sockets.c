#include "t_common.h"


static void event_callback(struct t_netconn* con, enum t_netconn_evt evt, u16_t len)
{
}

int t_lwip_socket(int domain,int type,int protocol)
{
	int i;
	int ret = 0;

	struct t_netconn * netcon = NULL;

	switch(type){
		case SOCK_DGRAM:
			netcon = t_netconn_new_with_callback(T_NETCONN_UDP,event_callback);
			break;

		default:
			ERROR(("t_lwip_socket:not supported type:%d",type));
			ret = -1;
			break;
	}


	return i;
}
