#include "t_common.h"

static void do_newconn(struct t_api_msg_msg *msg)
{
#if 0
	if(msg->conn->pcb.tcp != NULL) {
   /* This "new" connection already has a PCB allocated. */
   /* Is this an error condition? Should it be deleted? 
      We currently just are happy and return. */
     sys_mbox_post(msg->conn->mbox, NULL);
     return;
   }
#endif

	msg->conn->err = ERR_OK;

	/* Allocate a PCB for this connection */
	switch(msg->conn->type) {
	case T_NETCONN_UDP:
		msg->conn->pcb.udp = t_udp_new();
		if(msg->conn->pcb.udp == NULL) {
			msg->conn->err = -1;
			break;
		}
		t_udp_recv(msg->conn->pcb.udp, t_recv_udp, msg->conn);
		break;
	}
	t_sys_mbox_post(msg->conn->mbox, NULL);
}

typedef void (* api_msg_decode)(struct t_api_msg_msg *msg);
static api_msg_decode decode[T_API_MSG_MAX] = {
  do_newconn,
#if 0
  do_delconn,
  do_bind,
  do_connect,
  do_disconnect,
  do_listen,
  do_accept,
  do_send,
  do_recv,
  do_write,
  do_close
#endif
  };

void
t_api_msg_input(struct t_api_msg *msg)
{
  decode[msg->type](&(msg->msg));
}

void t_api_msg_post(struct t_api_msg *msg)
{
	t_tcpip_apimsg(msg);
}
