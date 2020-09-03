#ifndef __LSSDP_H
#define __LSSDP_H

#include <stdbool.h>  // bool, true, false
#include <stdint.h>   // uint32_t

// LSSDP Log Level
enum LSSDP_LOG {
    LSSDP_LOG_DEBUG = 1 << 0,
    LSSDP_LOG_INFO  = 1 << 1,
    LSSDP_LOG_WARN  = 1 << 2,
    LSSDP_LOG_ERROR = 1 << 3
};

/* Struct : lssdp_nbr */
#define LSSDP_FIELD_LEN         64
#define LSSDP_LOCATION_LEN      128
typedef struct lssdp_nbr {
    char            usn         [LSSDP_FIELD_LEN];          // Unique Service Name (Device Name or MAC)
    char            location    [LSSDP_LOCATION_LEN];       // URL or IP(:Port)

    /* Additional SSDP Header Fields */
    char            sm_id       [LSSDP_FIELD_LEN];
    char            device_type [LSSDP_FIELD_LEN];
    long long       update_time;
    struct lssdp_nbr * next;
} lssdp_nbr;


/* Struct : lssdp_ctx */
#define LSSDP_INTERFACE_NAME_LEN    16                      // IFNAMSIZ
#define LSSDP_INTERFACE_LIST_SIZE   1
#define LSSDP_IP_LEN                16
typedef struct lssdp_ctx {
    int             sock;                                   // SSDP socket
    unsigned short  port;                                   // SSDP port (0x0000 ~ 0xFFFF)
    lssdp_nbr *     neighbor_list;                          // SSDP neighbor list
    long            neighbor_timeout;                       // milliseconds
    bool            debug;                                  // show debug log

    /* Network Interface */
    size_t          interface_num;                          // interface number
    struct lssdp_interface {
        char        name        [LSSDP_INTERFACE_NAME_LEN]; // name[16]
        char        ip          [LSSDP_IP_LEN];             // ip[16] = "xxx.xxx.xxx.xxx"
        uint32_t    addr;                                   // address in network byte order
        uint32_t    netmask;                                // mask in network byte order
    } interface[LSSDP_INTERFACE_LIST_SIZE];                 // interface[16]

    /* SSDP Header Fields */
    struct {
        /* SSDP Standard Header Fields */
        char        search_target       [LSSDP_FIELD_LEN];  // Search Target
        char        unique_service_name [LSSDP_FIELD_LEN];  // Unique Service Name: MAC or User Name

        /* Additional SSDP Header Fields */
        char        sm_id               [LSSDP_FIELD_LEN];
        char        device_type         [LSSDP_FIELD_LEN];

        struct {                                            // Location (optional):
            char    prefix              [LSSDP_FIELD_LEN];  // Protocal: "https://" or "http://"
            char    domain              [LSSDP_FIELD_LEN];  // if domain is empty, using Interface IP as default
            char    suffix              [LSSDP_FIELD_LEN];  // URI or Port: "/index.html" or ":80"
        } location;
    } header;

    /* Callback Function */
    int (* network_interface_changed_callback) (struct lssdp_ctx * lssdp);
    int (* neighbor_list_changed_callback)     (struct lssdp_ctx * lssdp);
    int (* packet_received_callback)           (struct lssdp_ctx * lssdp, const char * packet, size_t packet_len);

} lssdp_ctx;


/*
 * 01. lssdp_network_interface_update
 *
 * update network interface.
 *
 * Note:
 *  - lssdp.interface, lssdp.interface_num will be updated.
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_network_interface_update(lssdp_ctx * lssdp);

/*
 * 02. lssdp_socket_create
 *
 * create SSDP socket.
 *
 * Note:
 *  - SSDP port must be setup ready before call this function. (lssdp.port > 0)
 *  - if SSDP socket is already exist (lssdp.sock > 0),
 *    the socket will be closed, and create a new one.
 *  - SSDP neighbor list will be force clean up.
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_socket_create(lssdp_ctx * lssdp);

/*
 * 03. lssdp_socket_close
 *
 * close SSDP socket.
 *
 * Note:
 *  - if SSDP socket <= 0, will be ignore, and lssdp.sock will be set to -1.
 *  - SSDP neighbor list will be force clean up.
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_socket_close(lssdp_ctx * lssdp);

/*
 * 04. lssdp_socket_read
 *
 * read SSDP socket.
 *
 * 1. if read success, packet_received_callback will be invoked.
 * 2. if received SSDP packet is match to Search Target (lssdp.header.search_target),
 *     - M-SEARCH: send RESPONSE back
 *     - NOTIFY/RESPONSE: add/update to SSDP neighbor list
 *
 * Note:
 *  - SSDP socket and port must be setup ready before call this function. (sock, port > 0)
 *  - if SSDP neighbor list has been changed, neighbor_list_changed_callback will be invoked.
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_socket_read(lssdp_ctx * lssdp);

/*
 * 05. lssdp_send_msearch
 *
 * send SSDP M-SEARCH packet to multicast address (239.255.255.250)
 *
 * Note:
 *  - SSDP port must be setup ready before call this function. (lssdp.port > 0)
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_send_msearch(lssdp_ctx * lssdp);

/*
 * 06. lssdp_send_notify
 *
 * send SSDP NOTIFY packet to multicast address (239.255.255.250)
 *
 * Note:
 *  - SSDP port must be setup ready before call this function. (lssdp.port > 0)
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_send_notify(lssdp_ctx * lssdp);

/*
 * 07. lssdp_neighbor_check_timeout
 *
 * check neighbor in list is timeout or not. (lssdp.neighbor_timeout)
 * the timeout neighbor will be remove from the list.
 *
 * Note:
 *  - if neighbor be removed, neighbor_list_changed_callback will be invoked.
 *
 * @param lssdp
 * @return = 0      success
 *         < 0      failed
 */
int lssdp_neighbor_check_timeout(lssdp_ctx * lssdp);

/*
 * 08. lssdp_set_log_callback
 *
 * setup SSDP log callback. All SSDP library log will be forward to here.
 *
 * @param callback
 */
void lssdp_set_log_callback(void (* callback)(const char * file, const char * tag, int level, int line, const char * func, const char * message));

#endif
