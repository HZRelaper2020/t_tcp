sources := port/for_linux.c 

sources += api/t_tcpip.c 

sources += include/arch/t_sys.c

sources += core/t_ip.c
sources += core/t_netif.c
sources += core/t_pbuf.c
sources += core/t_memp.c
sources += core/t_arp.c

sources += user/s_main.c

INC := -I./include
INC += -I.
INC += -I./user
INC += -I./include/ipv4

FLAGS := -g
#FLAGS += -DT_TEST_MALLOC

main:
	gcc ${sources} ${FLAGS} -lpcap -lpthread ${INC} 
	sudo ./a.out
