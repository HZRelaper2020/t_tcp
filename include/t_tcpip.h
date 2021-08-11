#ifndef SOURCE_T_TCPIP_H_
#define SOURCE_T_TCPIP_H_


enum t_tcpip_msg_type{
	T_TCPIP_MSG_API,
	T_TCPIP_MSG_INPUT,
	T_TCPIP_MSG_CALLBACK
};

struct t_tcpip_msg{
	enum t_tcpip_msg_type type;
	union{
		struct{
			struct t_pbuf *p;
			struct t_netif *netif;	
		}inp;
	}msg;
};

int t_tcpip_init();

t_err_t t_tcpip_input(struct t_pbuf*p,struct t_netif* inp);

#endif
