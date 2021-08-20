#include "t_common.h"

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
