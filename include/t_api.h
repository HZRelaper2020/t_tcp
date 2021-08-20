#ifndef SOURCE_T_API__H
#define SOURCE_T_API__H

enum t_netconn_type{
	T_NETCONN_UDP
};

enum t_netconn_evt{
	aaa
};

enum t_netconn_state{
	T_NETCONN_NONE
};
struct t_netconn{
  enum t_netconn_type type;
  enum t_netconn_state state;
  union {
    struct t_tcp_pcb *tcp;
    struct t_udp_pcb *udp;
    struct t_raw_pcb *raw;
  } pcb;
  err_t err;
  t_sys_mbox_t mbox;
  t_sys_mbox_t recvmbox;
  t_sys_mbox_t acceptmbox;
  t_sys_sem_t sem;
  int socket;
  u16_t recv_avail;
  void (* callback)(struct t_netconn *, enum t_netconn_evt, u16_t len);
};

struct
t_netconn *t_netconn_new(enum t_netconn_type t);


struct
t_netconn *t_netconn_new_with_callback(enum t_netconn_type t,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len));

struct
t_netconn *t_netconn_new_with_proto_and_callback(enum t_netconn_type t, u16_t proto,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len));

#endif
