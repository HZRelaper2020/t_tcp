#ifndef SOURCE_MEMPY__H
#define SOURCE_MEMPY__H

#ifndef T_MEMP_MEMORY_SIZE
#define T_MEMP_MEMORY_SIZE		4096
#endif

#define T_MEMP_NUM_TCPIP_MSG		8

typedef enum{
	T_MEMP_TCPIP_MSG = 0,
	T_MEMP_MAX
}t_memp_t;

void t_memp_init(void);

void* t_memp_malloc(t_memp_t type);

void t_memp_free(t_memp_t type,void* mem);

#endif
