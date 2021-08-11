#include <stdio.h>
#include "t_common.h"


#include "port/for_linux.h"

static struct t_netif netif0;
struct t_ip_addr ip,mask,gw;

static t_err_t etherif0_init(struct t_netif* netif)
{
	linux_ether_init();
	return 0;
}

static t_err_t  ether0_output(struct t_netif* netif,struct t_pbuf *p)
{
//	linux_send_data()
	return 0;
}

static void ether0_recv_data(const uint8_t* data,int len)
{
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

	ip.addr = 0x6701A8C0;
	mask.addr = 0x00FFFFFF;
	gw.addr = 0x0101A8C0;
	uint8_t mac[6] = {0xb0,0x6e,0xbf,0x37,0x50,0xbd};

	t_netif_set_mac(&netif0,mac);
	t_netif_set_addr(&netif0,&ip,&mask,&gw);

	linux_set_recv_callback(ether0_recv_data);
	t_netif_add(&netif0,etherif0_init,ether0_output);

}

int main()
{
	t_pbuf_init();
	t_memp_init();
	t_tcpip_init();
	t_arp_init();
	ether_init();

	while(1){
		char c = getchar();
		printf("input c:%d\n",c);
	}
	return 0;
}
