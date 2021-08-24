#include "t_common.h"

#define NUM_SOCKETS T_MEMP_NUM_NETCONN

struct t_lwip_socket {
  struct t_netconn *conn;
  struct t_netbuf *lastdata;
  u16_t lastoffset;
  u16_t rcvevent;
  u16_t sendevent;
  u16_t  flags;
  int err;
};

static struct t_lwip_socket sockets[NUM_SOCKETS];

static t_sys_sem_t socksem = T_SYS_SEM_NULL;

static struct t_lwip_socket *
t_get_socket(int s)
{
  struct t_lwip_socket *sock = NULL;

  if ((s < 0) || (s > NUM_SOCKETS)) {
  }else{
  	sock = &sockets[s];
  }
  return sock;
}
static int
t_alloc_socket(struct t_netconn *newconn)
{
  int i;

  if (!socksem)
      socksem = t_sys_sem_new(1);

  /* Protect socket array */
  t_sys_sem_wait(socksem);

  /* allocate a new socket identifier */
  for(i = 0; i < NUM_SOCKETS; ++i) {
    if (!sockets[i].conn) {
      sockets[i].conn = newconn;
      sockets[i].lastdata = NULL;
      sockets[i].lastoffset = 0;
      sockets[i].rcvevent = 0;
      sockets[i].sendevent = 1; /* TCP send buf is empty */
      sockets[i].flags = 0;
      sockets[i].err = 0;
      t_sys_sem_signal(socksem);
      return i;
    }
  }
  t_sys_sem_signal(socksem);
  return -1;
}

static void event_callback(struct t_netconn* con, enum t_netconn_evt evt, u16_t len)
{
}

err_t t_netconn_delete(struct t_netconn* conn)
{
	return 0;
}

err_t t_netconn_peer(struct t_netconn* conn,struct t_ip_addr* addr,uint16_t* port)
{
	switch(conn->type){
		case T_NETCONN_UDP:
			break;
	}

	return conn->err == 0;
}

err_t
t_netconn_connect(struct t_netconn *conn, struct t_ip_addr *addr,
       u16_t port)
{
  struct t_api_msg *msg;

  if (conn == NULL) {
    ERROR(("t_netconn_connect conn is NULL"));
    return -1;
  }


  if (conn->recvmbox == T_SYS_MBOX_NULL) {
    if ((conn->recvmbox = t_sys_mbox_new(0xff)) == T_SYS_MBOX_NULL) {
    ERROR(("t_netconn_connect recvmbox malloc NULL"));
      return -1;
    }
  }

  if ((msg = t_memp_malloc(T_MEMP_API_MSG)) == NULL) {
    ERROR(("t_netconn_connect conn is NULL"));
    return -1;
  }
  msg->type = T_API_MSG_CONNECT;
  msg->msg.conn = conn;
  msg->msg.msg.bc.ipaddr = addr;
  msg->msg.msg.bc.port = port;
  t_api_msg_post(msg);
  t_sys_mbox_fetch(conn->mbox, NULL,0);
  t_memp_free(T_MEMP_API_MSG, msg);
  return conn->err;
}

int
t_lwip_send(int s, void *data, int size, unsigned int flags)
{
  struct t_lwip_socket *sock;
  struct t_netbuf *buf;
  err_t err;

  sock = t_get_socket(s);
  if (!sock) {
    return -1;
  }

  switch (sock->conn->type) {
  case T_NETCONN_UDP:
    /* create a buffer */
    buf = t_netbuf_new();

    if (!buf) {
      return -1;
    }

    /* make the buffer point to the data that should
       be sent */
    t_netbuf_ref(buf, data, size);

    /* send the data */
    //err = t_netconn_send(sock->conn, buf);

    /* deallocated the buffer */
    t_netbuf_delete(buf);
    break;
  default:
    err = -1;
    break;
  }
  if (err != ERR_OK) {
    return -1;
  }

  return size;
}

int
t_lwip_sendto(int s, void *data, int size, unsigned int flags,
       struct sockaddr *to, socklen_t tolen)
{
  struct t_lwip_socket *sock;
  struct t_ip_addr remote_addr, addr;
  u16_t remote_port, port;
  int ret,connected;

  sock = t_get_socket(s);
  if (!sock) {
    return -1;
  }

  /* get the peer if currently connected */
  //connected = (t_netconn_peer(sock->conn, &addr, &port) == ERR_OK);

  remote_addr.addr = ((struct sockaddr_in *)to)->sin_addr.s_addr;
  remote_port = ((struct sockaddr_in *)to)->sin_port;


  t_netconn_connect(sock->conn, &remote_addr, htons(remote_port));

  ret = t_lwip_send(s, data, size, flags);

  return ret;
#if 0
  /* reset the remote address and port number
     of the connection */
  if (connected)
    netconn_connect(sock->conn, &addr, port);
  else
  netconn_disconnect(sock->conn);
  return ret;
#endif
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


	if (netcon == NULL){
		ERROR(("t_lwip_socket netcon is NULL"));
		i= -1;
	}else{
		i = t_alloc_socket(netcon);
		if (i == -1) {
			t_netconn_delete(netcon);
			ERROR(("t_lwip_socket alloc_scoket failed"));
		}else{
			netcon->socket = i;
		}


	}

	return i;
}
