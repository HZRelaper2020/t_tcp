#ifndef SOURCE_MEMPY__H
#define SOURCE_MEMPY__H

#ifndef T_MEMP_MEMORY_SIZE
#define T_MEMP_MEMORY_SIZE		4096
#endif

#ifndef T_MEMP_NUM_TCPIP_MSG
#define T_MEMP_NUM_TCPIP_MSG		50	
#endif

#ifndef T_MEMP_NUM_NETCONN
#define T_MEMP_NUM_NETCONN		4
#endif

#ifndef T_MEMP_NUM_API_MSG
#define T_MEMP_NUM_API_MSG		8
#endif

#ifndef T_MEMP_NUM_UDP_PCB
#define T_MEMP_NUM_UDP_PCB		2
#endif

#ifndef T_MEMP_NUM_NETBUF
#define T_MEMP_NUM_NETBUF		8
#endif

typedef enum{
	T_MEMP_TCPIP_MSG = 0,
	T_MEMP_NETCONN,
	T_MEMP_API_MSG,
	T_MEMP_UDP_PCB,
	T_MEMP_NETBUF,
	T_MEMP_MAX
}t_memp_t;

void t_memp_init(void);

void* t_memp_malloc(t_memp_t type);

void t_memp_free(t_memp_t type,void* mem);

#endif
