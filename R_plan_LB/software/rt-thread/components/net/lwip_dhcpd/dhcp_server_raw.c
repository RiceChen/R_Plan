/*
 * File      : dhcp_server_raw.c
 *             A simple DHCP server implementation
 *
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2011-2018, Shanghai Real-Thread Technology Co., Ltd
 * http://www.rt-thread.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-04-01     Ren.Haibo    the first version
 * 2018-06-12     aozima       ignore DHCP_OPTION_SERVER_ID.
 */

#include <stdio.h>
#include <stdint.h>

#include <rtthread.h>

#include <lwip/opt.h>
#include <lwip/sockets.h>
#include <lwip/inet_chksum.h>
#include <netif/etharp.h>
#include <netif/ethernetif.h>
#include <lwip/ip.h>
#include <lwip/init.h>

#if (LWIP_VERSION) < 0x02000000U
    #error "not support old LWIP"
#endif

#if !LWIP_IPV4
    #error "must enable IPV4"
#endif

#if (LWIP_VERSION) >= 0x02000000U
    #include <lwip/prot/dhcp.h>
#endif

/* DHCP server option */

/* allocated client ip range */
#ifndef DHCPD_CLIENT_IP_MIN
    #define DHCPD_CLIENT_IP_MIN     100
#endif
#ifndef DHCPD_CLIENT_IP_MAX
    #define DHCPD_CLIENT_IP_MAX     254
#endif

/* the DHCP server address */
#ifndef DHCPD_SERVER_IP
    #define DHCPD_SERVER_IP "192.168.169.1"
#endif

#define DHCP_DEBUG_PRINTF

#ifdef  DHCP_DEBUG_PRINTF
    #define DEBUG_PRINTF        rt_kprintf("[DHCP] "); rt_kprintf
#else
    #define DEBUG_PRINTF(...)
#endif /* DHCP_DEBUG_PRINTF */

/* we need some routines in the DHCP of lwIP */
#undef  LWIP_DHCP
#define LWIP_DHCP   1
#include <lwip/dhcp.h>

/** Mac address length  */
#define DHCP_MAX_HLEN               6
/** dhcp default live time */
#define DHCP_DEFAULT_LIVE_TIME      0x80510100

/** Minimum length for request before packet is parsed */
#define DHCP_MIN_REQUEST_LEN        44

#define LWIP_NETIF_LOCK(...)
#define LWIP_NETIF_UNLOCK(...)

/**
* The dhcp client node struct.
*/
struct dhcp_client_node
{
    struct dhcp_client_node *next;
    u8_t chaddr[DHCP_MAX_HLEN];
    ip4_addr_t ipaddr;
    u32_t lease_end;
};

/**
* The dhcp server struct.
*/
struct dhcp_server
{
    struct dhcp_server *next;
    struct netif *netif;
    struct udp_pcb *pcb;
    struct dhcp_client_node *node_list;
    ip4_addr_t start;
    ip4_addr_t end;
    ip4_addr_t current;
};

static u8_t *dhcp_server_option_find(u8_t *buf, u16_t len, u8_t option);

/**
* The dhcp server struct list.
*/
static struct dhcp_server *lw_dhcp_server;

/**
* Find a dhcp client node by mac address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_mac(struct dhcp_server *dhcpserver, const u8_t *chaddr, u8_t hlen)
{
    struct dhcp_client_node *node;

    for (node = dhcpserver->node_list; node != NULL; node = node->next)
    {
        if (memcmp(node->chaddr, chaddr, hlen) == 0)
        {
            return node;
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param ip The client ip
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_ip(struct dhcp_server *dhcpserver, const ip4_addr_t *ip)
{
    struct dhcp_client_node *node;

    for (node = dhcpserver->node_list; node != NULL; node = node->next)
    {
        /* don't use ip4_addr_cmp, the ip maybe not align. */
        if (0 == memcmp(&node->ipaddr, ip, sizeof(*ip)))
        {
            return node;
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find(struct dhcp_server *dhcpserver, struct dhcp_msg *msg,
                 u8_t *opt_buf, u16_t len)
{
    u8_t *opt;
    struct dhcp_client_node *node;

    node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
    if (node != NULL)
    {
        return node;
    }

    opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
    if (opt != NULL)
    {
        node = dhcp_client_find_by_ip(dhcpserver, (ip4_addr_t *)(&opt[2]));
        if (node != NULL)
        {
            return node;
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_alloc(struct dhcp_server *dhcpserver, struct dhcp_msg *msg,
                  u8_t *opt_buf, u16_t len)
{
    u8_t *opt;
    u32_t ipaddr;
    struct dhcp_client_node *node;

    node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
    if (node != NULL)
    {
        return node;
    }

    opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
    if (opt != NULL)
    {
        node = dhcp_client_find_by_ip(dhcpserver, (ip4_addr_t *)(&opt[2]));
        if (node != NULL)
        {
            return node;
        }
    }

dhcp_alloc_again:
    node = dhcp_client_find_by_ip(dhcpserver, &dhcpserver->current);
    if (node != NULL)
    {
        ipaddr = (ntohl(dhcpserver->current.addr) + 1);
        if (ipaddr > ntohl(dhcpserver->end.addr))
        {
            ipaddr = ntohl(dhcpserver->start.addr);
        }
        dhcpserver->current.addr = htonl(ipaddr);
        goto dhcp_alloc_again;
    }
    node = (struct dhcp_client_node *)mem_malloc(sizeof(struct dhcp_client_node));
    if (node == NULL)
    {
        return NULL;
    }
    SMEMCPY(node->chaddr, msg->chaddr, msg->hlen);
    node->ipaddr = dhcpserver->current;

    node->next = dhcpserver->node_list;
    dhcpserver->node_list = node;

    return node;
}

/**
* find option from buffer.
*
* @param buf The buffer to find option
* @param len The buffer length
* @param option Which option to find
* @return dhcp option buffer
*/
static u8_t *
dhcp_server_option_find(u8_t *buf, u16_t len, u8_t option)
{
    u8_t *end = buf + len;
    while ((buf < end) && (*buf != DHCP_OPTION_END))
    {
        if (*buf == option)
        {
            return buf;
        }
        buf += (buf[1] + 2);
    }
    return NULL;
}

/**
* If an incoming DHCP message is in response to us, then trigger the state machine
*/
static void
dhcp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *recv_addr, u16_t port)
{
    struct dhcp_server *dhcp_server = (struct dhcp_server *)arg;
    struct dhcp_msg *msg;
    struct pbuf *q;
    u8_t *opt_buf;
    u8_t *opt;
    struct dhcp_client_node *node;
    u8_t msg_type;
    u16_t length;
    ip_addr_t addr = *recv_addr;
    u32_t tmp;

    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[%s:%d] %c%c recv %d\n", __FUNCTION__, __LINE__, dhcp_server->netif->name[0], dhcp_server->netif->name[1], p->tot_len));
    /* prevent warnings about unused arguments */
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(port);

    if (p->len < DHCP_MIN_REQUEST_LEN)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCP request message or pbuf too short\n"));
        pbuf_free(p);
        return;
    }

    q = pbuf_alloc(PBUF_TRANSPORT, 1500, PBUF_RAM);
    if (q == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg failed!\n"));
        pbuf_free(p);
        return;
    }
    if (q->tot_len < p->tot_len)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg too small %d:%d\n", q->tot_len, p->tot_len));
        pbuf_free(p);
        return;
    }

    pbuf_copy(q, p);
    pbuf_free(p);

    msg = (struct dhcp_msg *)q->payload;
    if (msg->op != DHCP_BOOTREQUEST)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("not a DHCP request message, but type %"U16_F"\n", (u16_t)msg->op));
        goto free_pbuf_and_return;
    }

    if (msg->cookie != PP_HTONL(DHCP_MAGIC_COOKIE))
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("bad DHCP_MAGIC_COOKIE!\n"));
        goto free_pbuf_and_return;
    }

    if (msg->hlen > DHCP_MAX_HLEN)
    {
        goto free_pbuf_and_return;
    }

    opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
    length = q->tot_len - DHCP_OPTIONS_OFS;
    opt = dhcp_server_option_find(opt_buf, length, DHCP_OPTION_MESSAGE_TYPE);
    if (opt)
    {
        msg_type = *(opt + 2);
        if (msg_type == DHCP_DISCOVER)
        {
            node = dhcp_client_alloc(dhcp_server, msg, opt_buf, length);
            if (node == NULL)
            {
                goto free_pbuf_and_return;
            }
            node->lease_end = DHCP_DEFAULT_LIVE_TIME;
            /* create dhcp offer and send */
            msg->op = DHCP_BOOTREPLY;
            msg->hops = 0;
            msg->secs = 0;
            SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
            msg->sname[0] = '\0';
            msg->file[0] = '\0';
            msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
            SMEMCPY(&msg->yiaddr, &node->ipaddr, 4);

            opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
            /* add msg type */
            *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
            *opt_buf++ = 1;
            *opt_buf++ = DHCP_OFFER;

            /* add server id */
            *opt_buf++ = DHCP_OPTION_SERVER_ID;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
            opt_buf += 4;

            /* add_lease_time */
            *opt_buf++ = DHCP_OPTION_LEASE_TIME;
            *opt_buf++ = 4;
            tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
            SMEMCPY(opt_buf, &tmp, 4);
            opt_buf += 4;

            /* add config */
            *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
            opt_buf += 4;

            *opt_buf++ = DHCP_OPTION_DNS_SERVER;
            *opt_buf++ = 4;
#ifdef DHCP_DNS_SERVER_IP
            {
                ip_addr_t dns_addr;
                ipaddr_aton(DHCP_DNS_SERVER_IP, &dns_addr);
                SMEMCPY(opt_buf, &ip_2_ip4(&dns_addr)->addr, 4);
            }
#else
            /* default use gatewary dns server */
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
#endif /* DHCP_DNS_SERVER_IP */
            opt_buf += 4;

            *opt_buf++ = DHCP_OPTION_ROUTER;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
            opt_buf += 4;

            /* add option end */
            *opt_buf++ = DHCP_OPTION_END;

            length = (u32_t)opt_buf - (u32_t)msg;
            if (length < q->tot_len)
            {
                pbuf_realloc(q, length);
            }

            ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
            udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
        }
        else
        {
            if (1)
            {
                if (msg_type == DHCP_REQUEST)
                {
                    node = dhcp_client_find(dhcp_server, msg, opt_buf, length);
                    if (node != NULL)
                    {
                        /* Send ack */
                        node->lease_end = DHCP_DEFAULT_LIVE_TIME;
                        /* create dhcp offer and send */
                        msg->op = DHCP_BOOTREPLY;
                        msg->hops = 0;
                        msg->secs = 0;
                        SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
                        msg->sname[0] = '\0';
                        msg->file[0] = '\0';
                        msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
                        SMEMCPY(&msg->yiaddr, &node->ipaddr, 4);
                        opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;

                        /* add msg type */
                        *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
                        *opt_buf++ = 1;
                        *opt_buf++ = DHCP_ACK;

                        /* add server id */
                        *opt_buf++ = DHCP_OPTION_SERVER_ID;
                        *opt_buf++ = 4;
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
                        opt_buf += 4;

                        /* add_lease_time */
                        *opt_buf++ = DHCP_OPTION_LEASE_TIME;
                        *opt_buf++ = 4;
                        tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
                        SMEMCPY(opt_buf, &tmp, 4);
                        opt_buf += 4;

                        /* add config */
                        *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
                        *opt_buf++ = 4;
                        SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
                        opt_buf += 4;

                        *opt_buf++ = DHCP_OPTION_DNS_SERVER;
                        *opt_buf++ = 4;
#ifdef DHCP_DNS_SERVER_IP
                        {
                            ip_addr_t dns_addr;
                            ipaddr_aton(DHCP_DNS_SERVER_IP, &dns_addr);
                            SMEMCPY(opt_buf, &ip_2_ip4(&dns_addr)->addr, 4);
                        }
#else
                        /* default use gatewary dns server */
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
#endif /* DHCP_DNS_SERVER_IP */
                        opt_buf += 4;

                        *opt_buf++ = DHCP_OPTION_ROUTER;
                        *opt_buf++ = 4;
                        SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
                        opt_buf += 4;

                        /* add option end */
                        *opt_buf++ = DHCP_OPTION_END;

                        length = (u32_t)opt_buf - (u32_t)msg;
                        if (length < q->tot_len)
                        {
                            pbuf_realloc(q, length);
                        }

                        ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
                        udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
                    }
                    else
                    {
                        /* Send no ack */
                        /* create dhcp offer and send */
                        msg->op = DHCP_BOOTREPLY;
                        msg->hops = 0;
                        msg->secs = 0;
                        SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
                        msg->sname[0] = '\0';
                        msg->file[0] = '\0';
                        msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
                        memset(&msg->yiaddr, 0, 4);
                        opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;

                        /* add msg type */
                        *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
                        *opt_buf++ = 1;
                        *opt_buf++ = DHCP_NAK;

                        /* add server id */
                        *opt_buf++ = DHCP_OPTION_SERVER_ID;
                        *opt_buf++ = 4;
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
                        opt_buf += 4;

                        /* add option end */
                        *opt_buf++ = DHCP_OPTION_END;
                        length = (u32_t)opt_buf - (u32_t)msg;
                        if (length < q->tot_len)
                        {
                            pbuf_realloc(q, length);
                        }

                        ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
                        udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
                    }
                }
                else if (msg_type == DHCP_RELEASE)
                {
                    struct dhcp_client_node *node_prev = NULL;

                    for (node = dhcp_server->node_list; node != NULL; node = node->next)
                    {
                        if (memcmp(node->chaddr, msg->chaddr, msg->hlen) == 0)
                        {
                            if (node == dhcp_server->node_list)
                            {
                                dhcp_server->node_list = node->next;
                            }
                            else
                            {
                                node_prev->next = node->next;
                            }
                            break;
                        }
                        node_prev = node;
                        node = node->next;
                    }

                    if (node != NULL)
                    {
                        mem_free(node);
                    }
                }
                else if (msg_type ==  DHCP_DECLINE)
                {
                    ;
                }
                else if (msg_type == DHCP_INFORM)
                {
                    ;
                }
            }
        }
    }

free_pbuf_and_return:
    pbuf_free(q);
}

/**
* start dhcp server for a netif
*
* @param netif The netif which use dhcp server
* @param start The Start IP address
* @param end The netif which use dhcp server
* @return lwIP error code
* - ERR_OK - No error
* - ERR_MEM - Out of memory
*/
static err_t
dhcp_server_start(struct netif *netif, ip4_addr_t *start, ip4_addr_t *end)
{
    struct dhcp_server *dhcp_server;

    /* If this netif alreday use the dhcp server. */
    for (dhcp_server = lw_dhcp_server; dhcp_server != NULL; dhcp_server = dhcp_server->next)
    {
        if (dhcp_server->netif == netif)
        {
            dhcp_server->start = *start;
            dhcp_server->end = *end;
            dhcp_server->current = *start;
            return ERR_OK;
        }
    }

    dhcp_server = NULL;
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting new DHCP server\n"));
    dhcp_server = (struct dhcp_server *)mem_malloc(sizeof(struct dhcp_server));
    if (dhcp_server == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): could not allocate dhcp\n"));
        return ERR_MEM;
    }

    /* clear data structure */
    memset(dhcp_server, 0, sizeof(struct dhcp_server));

    /* store this dhcp server to list */
    dhcp_server->next = lw_dhcp_server;
    lw_dhcp_server = dhcp_server;
    dhcp_server->netif = netif;
    dhcp_server->node_list = NULL;
    dhcp_server->start = *start;
    dhcp_server->end = *end;
    dhcp_server->current = *start;

    /* allocate UDP PCB */
    dhcp_server->pcb = udp_new();
    if (dhcp_server->pcb == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG  | LWIP_DBG_TRACE, ("dhcp_server_start(): could not obtain pcb\n"));
        return ERR_MEM;
    }

    ip_set_option(dhcp_server->pcb, SOF_BROADCAST);
    /* set up local and remote port for the pcb */
    udp_bind(dhcp_server->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
    //udp_connect(dhcp_server->pcb, IP_ADDR_ANY, DHCP_CLIENT_PORT);
    /* set up the recv callback and argument */
    udp_recv(dhcp_server->pcb, dhcp_server_recv, dhcp_server);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting DHCP server\n"));

    return ERR_OK;
}


void dhcpd_start(const char *netif_name)
{
    struct netif *netif = netif_list;
    err_t res;

    DEBUG_PRINTF("%s: %s\r\n", __FUNCTION__, netif_name);

    LWIP_NETIF_LOCK();
    if (strlen(netif_name) > sizeof(netif->name))
    {
        DEBUG_PRINTF("network interface name too long!\r\n");
        goto _exit;
    }

    while (netif != RT_NULL)
    {
        if (strncmp(netif_name, netif->name, sizeof(netif->name)) == 0)
            break;

        netif = netif->next;
        if (netif == RT_NULL)
        {
            DEBUG_PRINTF("network interface: %s not found!\r\n", netif_name);
            break;
        }
    }

    if (netif == RT_NULL)
    {
        goto _exit;
    }

    ip4_addr_t ip_start, ip_end;
    ip_start.addr = ntohl(ntohl(netif->ip_addr.addr & netif->netmask.addr) 
                | (DHCPD_CLIENT_IP_MIN) & ntohl(~netif->netmask.addr));
    
    ip_end.addr = ntohl(ntohl(netif->ip_addr.addr & netif->netmask.addr) 
                | (DHCPD_CLIENT_IP_MAX) & ntohl(~netif->netmask.addr));

    DEBUG_PRINTF("ip_start: [%s]\r\n", inet_ntoa(ip_start));
    DEBUG_PRINTF("ip_end: [%s]\r\n", inet_ntoa(ip_end));

    res = dhcp_server_start(netif, &ip_start, &ip_end);
    if (res != 0)
    {
        DEBUG_PRINTF("dhcp_server_start res: %s.\r\n", res);
    }


_exit:
    LWIP_NETIF_UNLOCK();
    return;
}


