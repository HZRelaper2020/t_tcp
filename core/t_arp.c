#include "t_common.h"

struct t_arp_entry{
};

static struct t_arp_entry t_arp_table[T_ARP_TABLE_SIZE];

void t_arp_init()
{
	for (int i =0;i< T_ARP_TABLE_SIZE;i++){
	}
}

static void t_update_arp_entry(struct t_netif* inp)
{
}

t_err_t t_arp_input(struct t_pbuf *p,struct t_netif* inp)
{
	if (p->tot_len >= sizeof(struct t_etharp_hdr)){ 
		struct  t_etharp_hdr* hdr = (struct t_etharp_hdr*)p->payload;

		int for_us = !t_ip_addr_netcmp(&hdr->target_ip,&inp->ip_addr,&inp->netmask);
		switch(hdr->opcode){
			case 0x0100: // request
				break;
			case 0x0200: // reply
				break;
			default:
				ERROR(("t_arp_input not supported opcde"));
				break;
		}

		printf("recv arp for us:%d\n",for_us);
	}

        return 0;
}
