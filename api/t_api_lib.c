#include "t_common.h"

struct
t_netbuf *t_netbuf_new(void)
{
  struct t_netbuf *buf;

  buf = t_memp_malloc(T_MEMP_NETBUF);
  if (buf != NULL) {
    buf->p = NULL;
    buf->ptr = NULL;
    return buf;
  } else {
    return NULL;
  }
}

void
t_netbuf_delete(struct t_netbuf *buf)
{
  if (buf != NULL) {
    if (buf->p != NULL) {
      t_pbuf_free(buf->p);
      buf->p = buf->ptr = NULL;
    }
    t_memp_free(T_MEMP_NETBUF, buf);
  }
}

void *
t_netbuf_alloc(struct t_netbuf *buf, u16_t size)
{
  /* Deallocate any previously allocated memory. */
  if (buf->p != NULL) {
    t_pbuf_free(buf->p);
  }
  buf->p = t_pbuf_alloc(T_PBUF_TRANSPORT, size, T_PBUF_RAM);
  if (buf->p == NULL) {
     return NULL;
  }
  buf->ptr = buf->p;
  return buf->p->payload;
}

void
t_netbuf_free(struct t_netbuf *buf)
{
  if (buf->p != NULL) {
    t_pbuf_free(buf->p);
  }
  buf->p = buf->ptr = NULL;
}

void
t_netbuf_ref(struct t_netbuf *buf, void *dataptr, u16_t size)
{
  if (buf->p != NULL) {
    t_pbuf_free(buf->p);
  }
  buf->p = t_pbuf_alloc(T_PBUF_TRANSPORT, 0, T_PBUF_REF);
  buf->p->payload = dataptr;
  buf->p->len = buf->p->tot_len = size;
  buf->ptr = buf->p;
}

#if 0
void
t_netbuf_chain(struct t_netbuf *head, struct t_netbuf *tail)
{
  pbuf_chain(head->p, tail->p);
  head->ptr = head->p;
  memp_free(MEMP_NETBUF, tail);
}
#endif

u16_t
t_netbuf_len(struct t_netbuf *buf)
{
  return buf->p->tot_len;
}

err_t
t_netbuf_data(struct t_netbuf *buf, void **dataptr, u16_t *len)
{
  if (buf->ptr == NULL) {
    return -1;
  }
  *dataptr = buf->ptr->payload;
  *len = buf->ptr->len;
  return ERR_OK;
}

u8_t
t_netbuf_next(struct t_netbuf *buf)
{
  if (buf->ptr->next == NULL) {
    return -1;
  }
  buf->ptr = buf->ptr->next;
  if (buf->ptr->next == NULL) {
    return 1;
  }
  return 0;
}

void
t_netbuf_first(struct t_netbuf *buf)
{
  buf->ptr = buf->p;
}

void
t_netbuf_copy_partial(struct t_netbuf *buf, void *dataptr, u16_t len, u16_t offset)
{
  struct t_pbuf *p;
  u16_t i, left;

  left = 0;

  if(buf == NULL || dataptr == NULL) {
    return;
  }

  /* This implementation is bad. It should use bcopy
     instead. */
  for(p = buf->p; left < len && p != NULL; p = p->next) {
    if (offset != 0 && offset >= p->len) {
      offset -= p->len;
    } else {
      for(i = offset; i < p->len; ++i) {
  ((char *)dataptr)[left] = ((char *)p->payload)[i];
  if (++left >= len) {
    return;
  }
      }
      offset = 0;
    }
  }
}

void
t_netbuf_copy(struct t_netbuf *buf, void *dataptr, u16_t len)
{
  t_netbuf_copy_partial(buf, dataptr, len, 0);
}

struct t_ip_addr *
t_netbuf_fromaddr(struct t_netbuf *buf)
{
  return buf->fromaddr;
}

u16_t
t_netbuf_fromport(struct t_netbuf *buf)
{
  return buf->fromport;
}

struct
t_netconn *t_netconn_new(enum t_netconn_type t)
{
  return t_netconn_new_with_proto_and_callback(t,0,NULL);
}

struct
t_netconn *t_netconn_new_with_callback(enum t_netconn_type t,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len))
{
  return t_netconn_new_with_proto_and_callback(t,0,callback);
}

struct
t_netconn *t_netconn_new_with_proto_and_callback(enum t_netconn_type t, u16_t proto,
                                   void (*callback)(struct t_netconn *, enum t_netconn_evt, u16_t len))
{
	struct t_netconn* conn;

	conn = t_memp_malloc(T_MEMP_NETCONN);
	if (conn == NULL){
		ERROR(("t_netconn_new_with_proto_and_callback:  malloc conn is NULL"));
	}else{
		conn->err = 0;
		conn->type = t;
		conn->pcb.tcp = NULL;

		conn->mbox = t_sys_mbox_new(0xff);
		if (conn->mbox == T_SYS_MBOX_NULL){
			t_memp_free(T_MEMP_NETCONN,conn);
			conn = NULL;
		}else{
			conn->recvmbox = T_SYS_MBOX_NULL;
			conn->acceptmbox = T_SYS_MBOX_NULL;
			conn->sem = T_SYS_SEM_NULL;
			conn->state = T_NETCONN_NONE;
			conn->socket = 0;
			conn->callback = callback;
			conn->recv_avail = 0;

			struct t_api_msg* msg = t_memp_malloc(T_MEMP_API_MSG);
			if (msg == NULL){
				ERROR(("t_netconn_new_with_proto_and_callback msg is NULL"));
				t_memp_free(T_MEMP_NETCONN,conn);
				conn = NULL;
			}else{
				msg->type = T_API_MSG_NEWCONN;
				msg->msg.msg.bc.port = proto; /* misusing the port field */
				msg->msg.conn = conn;
				t_api_msg_post(msg);
				t_sys_mbox_fetch(conn->mbox, NULL,0);
				t_memp_free(T_MEMP_API_MSG, msg);

				if ( conn->err != 0 ) {
					t_memp_free(T_MEMP_NETCONN, conn);
					conn = NULL;
			  	}

			}

		}
	}

	return conn;
}
