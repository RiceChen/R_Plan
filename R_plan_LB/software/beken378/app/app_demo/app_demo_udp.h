#ifndef __APP_DEMO_UDP_H__
#define __APP_DEMO_UDP_H__

UINT32 app_demo_udp_init(void);
void app_demo_udp_deinit(void);
int app_demo_udp_send_packet (UINT8 *data, UINT32 len);
void app_demo_disconnect_cmd_udp(void);

#endif
// eof

