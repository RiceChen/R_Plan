#ifndef _IEEE802_11_DEMO_H_
#define _IEEE802_11_DEMO_H_


#define WLAN_DEFAULT_IP         "192.168.0.1"
#define WLAN_DEFAULT_GW         "192.168.0.1"
#define WLAN_DEFAULT_MASK       "255.255.255.0"

void demo_scan_app_init(void);
void demo_scan_adv_app_init(uint8_t *oob_ssid);
void demo_softap_app_init(char *ap_ssid, char *ap_key);
void demo_sta_app_init(char *oob_ssid,char *connect_key);
void demo_sta_adv_app_init(char *oob_ssid,char *connect_key);
void demo_wlan_app_init(VIF_ADDCFG_PTR cfg);
void demo_state_app_init(void);
void demo_ip_app_init(void);

#endif

