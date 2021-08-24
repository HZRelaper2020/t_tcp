#ifndef SOURCE_T_API_MSG__H
#define SOURCE_T_API_MSG__H

enum t_api_msg_type {
  T_API_MSG_NEWCONN,
  T_API_MSG_CONNECT,
  T_API_MSG_DELCONN,

  T_API_MSG_BIND,
  T_API_MSG_DISCONNECT,

  T_API_MSG_LISTEN,
  T_API_MSG_ACCEPT,

  T_API_MSG_SEND,
  T_API_MSG_RECV,
  T_API_MSG_WRITE,

  T_API_MSG_CLOSE,

  T_API_MSG_MAX
};

struct t_api_msg_msg {
  struct t_netconn *conn;
  enum t_netconn_type conntype;
  union {
    struct t_pbuf *p;
    struct  {
      struct t_ip_addr *ipaddr;
      u16_t port;
    } bc;
    struct {
      void *dataptr;
      u16_t len;
      unsigned char copy;
    } w;
    t_sys_mbox_t mbox;
    u16_t len;
  } msg;
};

struct t_api_msg {
  enum t_api_msg_type type;
  struct t_api_msg_msg msg;
};

void t_api_msg_input(struct t_api_msg *msg);
void t_api_msg_post(struct t_api_msg *msg);

#endif
