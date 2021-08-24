#include <stdio.h>
#include "t_common.h"


#include "port/for_linux.h"

static struct t_netif netif0;
struct t_ip_addr ip,mask,gw;
FILE* fd = NULL;

static t_err_t etherif0_init(struct t_netif* netif)
{
	linux_ether_init();
	return 0;
}

static t_err_t  ether0_output(struct t_netif* netif,struct t_pbuf *p)
{
	linux_send_data(p->payload,p->tot_len);
	return 0;
}

static void ether0_recv_data(const uint8_t* data,int len)
{
	if (fd != NULL){
		fwrite(&len,1,4,fd);
		fwrite(data,1,len,fd);
		fflush(fd);
	}
	struct t_pbuf *p = t_pbuf_alloc(0,len,T_PBUF_FLAG_POOL);
	if (p == NULL){
		ERROR(("ether0_recv_data:no memory to alloc"));
	}else{
		t_pbuf_copy_data(p,(uint8_t*)data,len);
		//print_hex(p->payload,p->tot_len);
		t_tcpip_input(p,&netif0);
	}
}

static void ether_init()
{
//	fd = fopen("recv.bin","wb+");
	ip.addr = 0x7001A8C0;
	mask.addr = 0x00FFFFFF;
	gw.addr = 0x0101A8C0;
	uint8_t mac[6] = {0xb0,0x6e,0xbf,0x37,0x50,0xb0};
//	uint8_t mac[6] = {0x20,0x0d,0xb0,0x2c,0xf6,0x54};

	t_netif_set_mac(&netif0,mac);
	t_netif_set_addr(&netif0,&ip,&mask,&gw);

	linux_set_recv_callback(ether0_recv_data);
	t_netif_add(&netif0,etherif0_init,ether0_output);

}

#ifdef USER_MAIN
int main()
#else
int main2()
#endif
{
	t_pbuf_init();
	t_memp_init();
	t_tcpip_init();
	t_arp_init();
	ether_init();
	t_udp_init();

#ifdef T_TEST_ARP
	t_arp_test(&netif0);
#endif

	uint32_t times = 0;
	while(1){
		char c = getchar();
		if (c == 32){
		}else if (c == 0x31){ // 1
			printf("create socket\n");
			struct t_udp_pcb* udp = t_udp_new();
			if (udp != NULL){
			t_udp_bind(udp, &ip,3307);
			}
		}
		printf("input c:%d\n",c);
	}
	return 0;
}
