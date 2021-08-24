#include "t_common.h"

static void
t_recv_udp(void *arg, struct t_udp_pcb *pcb, struct t_pbuf *p,
   struct t_ip_addr *addr, u16_t port)
{
  struct t_netbuf *buf;
  struct t_netconn *conn;

  conn = arg;

  if (conn == NULL) {
    t_pbuf_free(p);
    return;
  }
  if (conn->recvmbox != T_SYS_MBOX_NULL) {
    buf = t_memp_malloc(T_MEMP_NETBUF);
    if (buf == NULL) {
      t_pbuf_free(p);
      return;
    } else {
      buf->p = p;
      buf->ptr = p;
      buf->fromaddr = addr;
      buf->fromport = port;
    }

  conn->recv_avail += p->tot_len;
    /* Register event with callback */
    if (conn->callback)
        (*conn->callback)(conn, T_NETCONN_EVT_RCVPLUS, p->tot_len);
    t_sys_mbox_post(conn->recvmbox, buf);
  }
}

void
t_udp_recv(struct t_udp_pcb *pcb,
   void (* recv)(void *arg, struct t_udp_pcb *upcb, struct t_pbuf *p,
           struct t_ip_addr *addr, u16_t port),
   void *recv_arg)
{
  /* remember recv() callback and user data */
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
}

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

static void do_connect(struct t_api_msg_msg *msg)
{
	switch(msg->conn->type){
		case T_NETCONN_UDP:
			break;
	}
}

typedef void (* api_msg_decode)(struct t_api_msg_msg *msg);
static api_msg_decode decode[T_API_MSG_MAX] = {
  do_newconn,
  do_connect,
#if 0
  do_delconn,
  do_bind,
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
