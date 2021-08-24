#ifndef SOURCE_T_API__H
#define SOURCE_T_API__H

enum t_netconn_type{
	T_NETCONN_UDP
};

enum t_netconn_evt{
	T_NETCONN_EVT_RCVPLUS
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

struct t_netbuf {
  struct t_pbuf *p, *ptr;
  struct t_ip_addr *fromaddr;
  u16_t fromport;
  err_t err;
};

struct
t_netconn *t_netconn_new(enum t_netconn_type t);
void *
t_netbuf_alloc(struct t_netbuf *buf, u16_t size);
void
t_netbuf_free(struct t_netbuf *buf);
void
t_netbuf_ref(struct t_netbuf *buf, void *dataptr, u16_t size);
u16_t
t_netbuf_len(struct t_netbuf *buf);
err_t
t_netbuf_data(struct t_netbuf *buf, void **dataptr, u16_t *len);
u8_t
t_netbuf_next(struct t_netbuf *buf);
void
t_netbuf_first(struct t_netbuf *buf);
void
t_netbuf_copy_partial(struct t_netbuf *buf, void *dataptr, u16_t len, u16_t offset);
void
t_netbuf_copy(struct t_netbuf *buf, void *dataptr, u16_t len);
struct t_ip_addr *
t_netbuf_fromaddr(struct t_netbuf *buf);
u16_t
t_netbuf_fromport(struct t_netbuf *buf);
struct
t_netbuf *t_netbuf_new(void);
void t_netbuf_delete(struct t_netbuf *buf);


struct
t_netconn *t_netconn_new_with_callback(enum t_netconn_type t,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len));

struct
t_netconn *t_netconn_new_with_proto_and_callback(enum t_netconn_type t, u16_t proto,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len));

#endif
