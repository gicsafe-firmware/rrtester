/**
 *  \file       eth.c
 *  \brief      FREERTOS LWIP Ethernet.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.03.25  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "bsp.h"
#include "ConMgrEth.h"
#include "signals.h"

#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/timers.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"

#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#include "arch/lpc18xx_43xx_emac.h"
#include "arch/lpc_arch.h"
#include "arch/sys_arch.h"
#include "lpc_phy.h" /* For the PHY monitor support */

#include "tcpecho.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static struct netif lpc_netif;

static rui8_t eth;
static int s;

static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
static RKH_ROM_STATIC_EVENT(e_Error, evError);
static RKH_ROM_STATIC_EVENT(e_linkConnected, evEthLinkConnect);
static RKH_ROM_STATIC_EVENT(e_linkDisconnect, evEthLinkDisconnect);
static RKH_ROM_STATIC_EVENT(e_ipStatus, evIPStatus);
static RKH_ROM_STATIC_EVENT(e_connected, evConnected);
static RKH_ROM_STATIC_EVENT(e_disconnected, evDisconnected);
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
void
msDelay(uint32_t ms)
{
    vTaskDelay((configTICK_RATE_HZ * ms) / 1000);
}

/* Callback for TCPIP thread to indicate TCPIP init is done */
static void
tcpip_init_done_signal(void *arg)
{
    /* Tell main thread TCP/IP init is done */
    *(s32_t *) arg = 1;
}

static void
link_status_changed(struct netif *netif)
{
	if (netif->flags & NETIF_FLAG_LINK_UP)
	{
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
        		&e_linkConnected), &eth);
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
        		&e_ipStatus), &eth);

        RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
             RKH_TUSR_STR("Ethernet Link Connected");
        RKH_TRC_USR_END();
	}
	else
	{
		lwip_close(s);
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_disconnected), &eth);
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_linkDisconnect), &eth);

		RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
			RKH_TUSR_STR("Ethernet Link Disconnected");
		RKH_TRC_USR_END();
	}
}

/* LWIP kickoff and PHY link monitor thread */
static void
vSetupEthTask(void *pvParameters)
{
    ip_addr_t ipaddr, netmask, gw;
    volatile s32_t tcpipdone = 0;
    uint32_t physts;
    static int prt_ip = 0;

    /* Wait until the TCP/IP thread is finished before
     * continuing or wierd things may happen */

    RKH_TR_FWK_TUSR(ETH_USR_TRACE);

    LWIP_DEBUGF(LWIP_DBG_ON, ("Waiting for TCPIP thread to initialize...\n"));
    tcpip_init(tcpip_init_done_signal, (void *) &tcpipdone);
    while (!tcpipdone)
    {
        msDelay(1);
    }

    LWIP_DEBUGF(LWIP_DBG_ON, ("Starting LWIP TCP echo server...\n"));

    /* Static IP assignment */
#if LWIP_DHCP
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
#else
    IP4_ADDR(&gw, 10, 1, 10, 1);
    IP4_ADDR(&ipaddr, 10, 1, 10, 234);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
#endif

    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, false);							/* GPIO3[0] PHY_NRESET output high */
    msDelay(10);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 3, 0, true);							/* GPIO3[0] PHY_NRESET output high */

    /* Add netif interface for lpc17xx_8x */
    if (!netif_add(&lpc_netif, &ipaddr, &netmask, &gw, NULL, lpc_enetif_init,
                   tcpip_input))
    {
        LWIP_ASSERT("Net interface failed to initialize\r\n", 0);
    }
    netif_set_default(&lpc_netif);
    netif_set_up(&lpc_netif);

    netif_set_link_callback(&lpc_netif, (netif_status_callback_fn) link_status_changed);

    /* Enable MAC interrupts only after LWIP is ready */
    NVIC_SetPriority(ETHERNET_IRQn, config_ETHERNET_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(ETHERNET_IRQn);

#if LWIP_DHCP
    dhcp_start(&lpc_netif);
#endif

    /* Initialize and start applications */
    
//    tcpecho_init();
//    tcptrace_init();
    //RKH_SMA_POST_FIFO(ethMgr, RKH_UPCAST(RKH_EVT_T, &e_Received), conMgr);

    /* This loop monitors the PHY link and will handle cable events
     * via the PHY driver. */
    while (1)
    {
        /* Call the PHY status update state machine once in a while
         * to keep the link status up-to-date */
        vTaskDelay(configTICK_RATE_HZ / 4);
        physts = lpcPHYStsPoll();

        /* Only check for connection state when the PHY status has changed */
        if (physts & PHY_LINK_CHANGED)
        {
            if (physts & PHY_LINK_CONNECTED)
            {
                prt_ip = 0;

                /* Set interface speed and duplex */
                if (physts & PHY_LINK_SPEED100)
                {
                    Chip_ENET_SetSpeed(LPC_ETHERNET, 1);
                    NETIF_INIT_SNMP(&lpc_netif,
                                    snmp_ifType_ethernet_csmacd,
                                    100000000);
                }
                else
                {
                    Chip_ENET_SetSpeed(LPC_ETHERNET, 0);
                    NETIF_INIT_SNMP(&lpc_netif,
                                    snmp_ifType_ethernet_csmacd,
                                    10000000);
                }
                if (physts & PHY_LINK_FULLDUPLX)
                {
                    Chip_ENET_SetDuplex(LPC_ETHERNET, true);
                }
                else
                {
                    Chip_ENET_SetDuplex(LPC_ETHERNET, false);
                }

                tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up,
                                          (void *) &lpc_netif, 1);
            }
            else
            {
                tcpip_callback_with_block(
                    (tcpip_callback_fn) netif_set_link_down,
                    (void *) &lpc_netif,
                    1);
            }

            DEBUGOUT("Link connect status: %d\r\n",
                     ((physts & PHY_LINK_CONNECTED) != 0));

            /* Delay for link detection (250mS) */
            vTaskDelay(configTICK_RATE_HZ / 4);
        }

        /* Print IP address info */
        if (!prt_ip)
        {
            if (lpc_netif.ip_addr.addr)
            {
                static char ip[16];
                static char mask[16];
                static char gw[16];

                DEBUGOUT("IP_ADDR    : %s\r\n",
                         ipaddr_ntoa_r((const ip_addr_t *) &lpc_netif.ip_addr,
                                       ip,
                                       16));


                DEBUGOUT("NET_MASK   : %s\r\n",
                         ipaddr_ntoa_r((const ip_addr_t *) &lpc_netif.netmask,
                                       mask,
                                       16));

                DEBUGOUT("GATEWAY_IP : %s\r\n",
                         ipaddr_ntoa_r((const ip_addr_t *) &lpc_netif.gw,
                                       gw, 16));

                RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
                	RKH_TUSR_STR(ip);
                	RKH_TUSR_STR(mask);
                    RKH_TUSR_STR(gw);
                RKH_TRC_USR_END();

                prt_ip = 1;
            }
        }
    }
}

/* ---------------------------- Global functions --------------------------- */
void
eth_init(void)
{
    xTaskCreate(vSetupEthTask, "SetupEth",
                configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
                (xTaskHandle *) NULL);
}

void
eth_deinit(void)
{
}

void
eth_socketOpen(char *ip, char *port)
{
	RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
		RKH_TUSR_STR("CALLED socketOpen\r\n");
	RKH_TRC_USR_END();


	struct sockaddr_in addr;
	/* set up address to connect to */
	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = PP_HTONS(1883);
	addr.sin_addr.s_addr = inet_addr("18.195.250.188"); //HiveMQ

	s = lwip_socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);

		RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
			RKH_TUSR_STR("lwip_socket < 0\r\n");
		RKH_TRC_USR_END();
	}
	int ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == 0)
	{
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_connected), &eth);

		RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
			RKH_TUSR_STR("Socket Connected\r\n");
		RKH_TRC_USR_END();

		int opt = 100;
		lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));
		lwip_setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &opt, sizeof(int));
	}
	else
	{
		lwip_close(s);
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);

		RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
			RKH_TUSR_STR("lwip_connect < 0\r\n");
		RKH_TRC_USR_END();
	}
}

void
eth_socketWrite(rui8_t *p, ruint size)
{
	RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
		RKH_TUSR_STR("CALLED socketWrite\r\n");
	RKH_TRC_USR_END();

	int ret = lwip_send(s, p, size, 0);
	if (ret < 0)
	{
		lwip_close(s);
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_disconnected), &eth);
	}
	else
	{
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Ok), &eth);
	}
}

ruint
eth_socketRead(rui8_t *p, ruint size)
{
	RKH_TRC_USR_BEGIN(ETH_USR_TRACE)
		RKH_TUSR_STR("CALLED socketRead\r\n");
	RKH_TRC_USR_END();

	int ret;

	ret = lwip_read(s, (char *)p, size);
/*	if (-1 == ret)
	{
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_disconnected), &eth);
		return 0;
	}
	else*/
	if (ret < 0 )
	{
		int error;
		u32_t optlen = sizeof(error);
		lwip_getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &optlen);

		switch (error)
		{
		case EWOULDBLOCK:	// Same as EAGAIN check for portability
			RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Ok), &eth);
			break;

		default:
			RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
					&e_Error), &eth);
			break;
		}
		return 0;
	}

	RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Ok), &eth);

	return ret;
}

/* ------------------------------ End of file ------------------------------ */
