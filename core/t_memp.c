#include "t_common.h"
#include "t_memp.h"

struct t_memp{
	struct t_memp* next;
};

static  uint8_t memp_memory[T_MEMP_MEMORY_SIZE];
static struct t_memp * memp_tab[T_MEMP_MAX];

static const uint16_t memp_sizes[T_MEMP_MAX]={
	sizeof(struct t_tcpip_msg),
};

static const uint16_t memp_num[T_MEMP_MAX] = {
	T_MEMP_NUM_TCPIP_MSG,
};

void t_memp_init(void)
{
	struct t_memp *m,*mp;

	mp = (struct t_memp*)&memp_memory[0];

	for (int i=0;i<T_MEMP_MAX;i++){
		int size= memp_sizes[i] + sizeof(struct t_memp);
		memp_tab[i] = mp;
		m = mp;

		for (int j=0;j<memp_num[i]-1;j++){
			m->next = (struct t_memp*)((uint8_t*)m + size);
			m = m->next;
		}
		m->next = NULL;
		mp = (struct t_memp*)((uint8_t*)mp + size * memp_num[i]);
	}

}


void* t_memp_malloc(t_memp_t type)
{
	struct t_memp* p = NULL;
	uint16_t offset = type;

	TASKSUSPENDALL();

	struct t_memp* memp;
	memp = (struct t_memp*)memp_tab[offset];

	if (memp != NULL){
		p = memp;
		memp_tab[offset]= memp->next;
		p->next = NULL;
	}

	if (p != NULL){
		p = (struct t_memp*)((uint8_t*)p + sizeof(struct t_memp));
	}

	TASKRESUMEALL();

	return (void*)p;
}

void t_memp_free(t_memp_t type,void* mem)
{
	struct t_memp* p;
	struct t_memp* mp;
	TASKSUSPENDALL();
	
	mp = (struct t_memp*)((uint8_t*)mem - sizeof(struct t_memp));

	if (memp_tab[type] != NULL){
		p = memp_tab[type];
		memp_tab[type] = mp;
		mp->next = p;
	}else{
		memp_tab[type] = mp;
	}

	TASKRESUMEALL();
}

#ifdef T_TEST_MALLOC
int main()
{
	t_memp_init();

#if 0
	for (int i=0;i<100;i++){
		void* data = t_memp_malloc(T_MEMP_TCPIP_MSG);
		if (data == NULL){
			ERROR(("malloc failed"));
			return -1;
		}
		t_memp_free(T_MEMP_TCPIP_MSG,data);
	}
#endif

	for (int i=0;i<100;i++){
		void* d1 = t_memp_malloc(T_MEMP_TCPIP_MSG);
		void* d2 = t_memp_malloc(T_MEMP_TCPIP_MSG);
		if (d1 == NULL || d2 == NULL){
			ERROR(("d1 or d2 == NULL"));
			return -1;
		}
		t_memp_free(T_MEMP_TCPIP_MSG,d1);
		t_memp_free(T_MEMP_TCPIP_MSG,d2);
	}

	return 0;
}
#endif
