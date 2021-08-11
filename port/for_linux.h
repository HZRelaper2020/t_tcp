#ifndef SOURCE_FOR_LINUX__H
#define SOURCE_FOR_LINUX__H

int linux_ether_init();

int linux_send_data(uint8_t* data,int len);

int linux_set_recv_callback(void (*callback)(const uint8_t* data,int size));


#endif
