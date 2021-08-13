#include "t_common.h"
#include "t_pbuf.h"

static uint8_t pbuf_pool_memory[T_PBUF_POOL_SIZE * (T_PBUF_BUFSIZE + sizeof(struct t_pbuf))];

static struct t_pbuf *pbuf_pool = NULL;

void t_pbuf_init()
{
	struct t_pbuf *p,*q;

	pbuf_pool = (struct t_pbuf*)&pbuf_pool_memory[0];

	p = pbuf_pool;
	for (int i = 0;i<T_PBUF_POOL_SIZE;i++){
		p->next = (struct t_pbuf*)((uint8_t*)p + T_PBUF_BUFSIZE + sizeof(struct t_pbuf));
		p->len = p->tot_len = T_PBUF_BUFSIZE;
		p->payload = (uint8_t*)p + sizeof(struct t_pbuf);
		p->flags = T_PBUF_FLAG_POOL;
		q = p;
		p = p->next;
	}
	q->next = NULL;
}


static void t_pbuf_add_tail(struct t_pbuf*p, struct t_pbuf* q)
{
	if ( p != NULL){
		do{
			p = p->next;
		}while(p->next != NULL);
		p->next = q;
	}
}

struct t_pbuf* t_pbuf_pool_alloc()
{
	struct t_pbuf* p = pbuf_pool;
	if (p){
		pbuf_pool = p->next;
		p->next = NULL;
	}

	int count = 0;
	struct t_pbuf* q = pbuf_pool;
	while(q != NULL){
		q = q->next;
		count +=1 ;
	}
	PRINT(("pbuf alloc %p remain:%d\n",(void*)p,count));
	return p;
}

struct t_pbuf* t_pbuf_alloc(uint16_t offset,uint16_t length,uint16_t flags)
{
	uint16_t rem_len = length + offset;
	struct t_pbuf* p=NULL, * q=NULL;

	TASKSUSPENDALL();
	if (rem_len > T_PBUF_BUFSIZE){
		ERROR(("t_pbuf_alloc:length too big %d",rem_len));
	}else{
		switch (flags){
			case T_PBUF_FLAG_POOL:
				p=t_pbuf_pool_alloc();
				if (p == NULL){
					ERROR(("t_pbuf_alloc T_PBUF_FLAG_POOL failed"));
				}else{
					p->next = NULL;
					p->ref = 1;
					p->payload = (uint8_t*)p + sizeof(struct t_pbuf) + offset;
					p->tot_len = length;
					p->len = length;
				}
				break;
			default:
				ERROR(("pbuf_alloc:not supported flags"));
		}
	}
	TASKRESUMEALL();

	return p;
}

void t_pbuf_ref(struct t_pbuf* p)
{
	while( p!= NULL){
		p->ref += 1;
		p = p->next;
	}
}

/*
 *
 * @return freed t_pbuf count 
 */
int t_pbuf_free(struct t_pbuf*p)
{
	int count = 0;

	TASKSUSPENDALL();

	while(p != NULL){
		count += 1;
		p->ref -= 1;
		struct t_pbuf *next= p->next;

		if (p->ref == 0){
			switch(p->flags){
				case T_PBUF_FLAG_POOL:
					p->next = pbuf_pool;
					pbuf_pool=p;
					PRINT(("pbuf free  %p\n",(void*)p));
					break;
			}
		}

		p = next;
	}

	TASKRESUMEALL();

	return count;
}

int t_pbuf_copy_data(struct t_pbuf*p,uint8_t* data,int size)
{
	int ret = 0;
	if (size > p->tot_len){
		ERROR(("t_pbuf_copy_data:can not copy too long %d %d",size,p->tot_len));
		ret = -1;
	}else{
		memcpy(p->payload,data,size);
	}
	return ret;
}

int t_pbuf_header(struct t_pbuf *p,int increment)
{
	int ret = -1;

	if (p->flags == T_PBUF_FLAG_POOL){
		void* payload = (void*)p->payload + increment;
		if  (payload>=(void*)p+sizeof(struct t_pbuf)
			&& payload < (void*)p + T_PBUF_BUFSIZE-1){
			p->payload = payload;
			p->tot_len -= increment;
			p->len -= increment;
		}else{
			ret = -1;
			ERROR(("t_pbuf_header:exceed limit"));
		}
	}else{
		ret = -1;
		ERROR(("t_pbuf_header:not supported other flags"));
	}
	
	return ret;
}
