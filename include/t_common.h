#ifndef __T_COMMON_H
#define __T_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint8_t u8_t;
//#define ERROR(a) do{time_t now;time(&now);printf("%10ld  ",now);printf a;printf("\n");}while(0)
//#define PRINT(a) do{time_t now;time(&now);printf("%10ld  ",now);printf a;}while(0)
#define ERROR(a) do{printf a;printf("\n");}while(0)
#define PRINT(a) do{printf a;}while(0)

#include "user/t_lwipopts.h"


static void inline print_hex(const uint8_t* data,int size)
{
	for (int i=0;i<size;i++){
		PRINT(("%02X ",*(data+i)));
		if ( i%16 == 15)
			PRINT(("\n"));
	}
	PRINT(("\n\n"));
}

#pragma pack(1)
#include "t_pbuf.h"
#include "t_memp.h"

#include "t_err.h"

#include "ipv4/t_ip_addr.h"

#include "t_netif.h"
#include "t_tcpip.h"
#include "arch/t_sys.h"

#include "ipv4/t_ip.h"
#include "t_arp.h"
#include "t_icmp.h"

#include "lwip/t_inet.h"

#pragma pack()

#endif
