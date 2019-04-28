/**
 *  \file       eth.c
 *  \brief      Lnx Ethernet socket abstraction.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "eth.h"
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include "rkhfwk_dynevt.h"
#include "bsp.h"
#include "signals.h"
#include "topics.h"
#include "config.h"
#include "ConMgrEth.h"

#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
/* ------------------------------- Constants ------------------------------- */
enum
{
    Unplugged_st, Plugged_st
};
//#define ETH_THREAD_DEBUG
static const int SLEEP_LAPSE_ETH_THREAD = 10;
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ruint running = 1;
static rui8_t eth;
static SOCKET s;

static unsigned adapter[9] = {Unplugged_st, Unplugged_st, Unplugged_st,
                              Unplugged_st, Unplugged_st, Unplugged_st,
                              Unplugged_st, Unplugged_st, Unplugged_st};

static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
static RKH_ROM_STATIC_EVENT(e_Error, evError);
static RKH_ROM_STATIC_EVENT(e_linkConnected, evEthLinkConnect);
static RKH_ROM_STATIC_EVENT(e_linkDisconnect, evEthLinkDisconnect);
static RKH_ROM_STATIC_EVENT(e_ipStatus, evIPStatus);
static RKH_ROM_STATIC_EVENT(e_connected, evConnected);
static RKH_ROM_STATIC_EVENT(e_disconnected, evDisconnected);

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
#ifdef OLD
static void
print_adapterInfo(PIP_ADAPTER_INFO p)
{
    UINT i;
    errno_t error;
    char buffer[32];
    struct tm newtime;

    printf("\tComboIndex: \t%d\n", p->ComboIndex);
    printf("\tAdapter Name: \t%s\n", p->AdapterName);
    printf("\tAdapter Desc: \t%s\n", p->Description);
    printf("\tAdapter Addr: \t");
    for (i = 0; i < p->AddressLength; i++)
    {
        if (i == (p->AddressLength - 1))
        {
            printf("%.2X\n", (int) p->Address[i]);
        }
        else
        {
            printf("%.2X-", (int) p->Address[i]);
        }
    }
    printf("\tIndex: \t%d\n", p->Index);
    printf("\tType: \t");
    switch (p->Type)
    {
        case MIB_IF_TYPE_OTHER:
            printf("Other\n");
            break;
        case MIB_IF_TYPE_ETHERNET:
            printf("Ethernet\n");
            break;
        case MIB_IF_TYPE_TOKENRING:
            printf("Token Ring\n");
            break;
        case MIB_IF_TYPE_FDDI:
            printf("FDDI\n");
            break;
        case MIB_IF_TYPE_PPP:
            printf("PPP\n");
            break;
        case MIB_IF_TYPE_LOOPBACK:
            printf("Lookback\n");
            break;
        case MIB_IF_TYPE_SLIP:
            printf("Slip\n");
            break;
        default:
            printf("Unknown type %ld\n", p->Type);
            break;
    }

    printf("\tIP Address: \t%s\n",
           p->IpAddressList.IpAddress.String);
    printf("\tIP Mask: \t%s\n", p->IpAddressList.IpMask.String);

    printf("\tGateway: \t%s\n", p->GatewayList.IpAddress.String);
    printf("\t***\n");

    if (p->DhcpEnabled)
    {
        printf("\tDHCP Enabled: Yes\n");
        printf("\t  DHCP Server: \t%s\n",
               p->DhcpServer.IpAddress.String);

        printf("\t  Lease Obtained: ");
        /* Display local time */
        error = _localtime32_s(&newtime, (__time32_t*) &p->LeaseObtained);
        if (error)
        {
            printf("Invalid Argument to _localtime32_s\n");
        }
        else
        {
            /* Convert to an ASCII representation */
            error = asctime_s(buffer, 32, &newtime);
            if (error)
            {
                printf("Invalid Argument to asctime_s\n");
            }
            else
            {
                /* asctime_s returns the string terminated by \n\0 */
                printf("%s", buffer);
            }
        }

        printf("\t  Lease Expires:  ");
        error = _localtime32_s(&newtime, (__time32_t*) &p->LeaseExpires);
        if (error)
        {
            printf("Invalid Argument to _localtime32_s\n");
        }
        else
        {
            /* Convert to an ASCII representation */
            error = asctime_s(buffer, 32, &newtime);
            if (error)
            {
                printf("Invalid Argument to asctime_s\n");
            }
            else
            {
                /* asctime_s returns the string terminated by \n\0 */
                printf("%s", buffer);
            }
        }
    }
    else
    {
        printf("\tDHCP Enabled: No\n");
    }

    if (p->HaveWins)
    {
        printf("\tHave Wins: Yes\n");
        printf("\t  Primary Wins Server:    %s\n",
               p->PrimaryWinsServer.IpAddress.String);
        printf("\t  Secondary Wins Server:  %s\n",
               p->SecondaryWinsServer.IpAddress.String);
    }
    else
    {
        printf("\tHave Wins: No\n");
    }
    printf("\n");
}
#endif
void
plugAdapter(struct ifaddrs * p, int i)
{
	if (adapter[i] == Unplugged_st)
	{
		adapter[i] = Plugged_st;
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_linkConnected), &eth);
		RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
				&e_ipStatus), &eth);
		printf("Using Ethernet Adapter: %d:%s\n", i, p->ifa_name);
#ifdef ETH_THREAD_DEBUG
		char host[NI_MAXHOST];
		s = getnameinfo(p->ifa_addr,
				(p->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) :
						sizeof(struct sockaddr_in6),
						host, NI_MAXHOST,
						NULL, 0, NI_NUMERICHOST);
		printf("\t\taddres: <%s>\n", host);
#endif
//TODO            print_adapterInfo(p);
	}
}
void
unplugAdapter(int i)
{
	if (adapter[i] == Plugged_st)
	{
		adapter[i] = Unplugged_st;
		RKH_SMA_POST_FIFO(conMgrEth,
				RKH_UPCAST(RKH_EVT_T, &e_linkDisconnect),
				&eth);
		printf("Unplugged Adapter: %d\n", i);
	}
}
void
check_AdapterStatus(struct if_nameindex *if_ni, struct ifaddrs *pAdapterInfo)
{
	int family, n, currentIndex, plugged;
	struct ifaddrs *pCurrentAdapter;
	struct if_nameindex *intf;

	for (intf = if_ni; intf->if_index != 0 || intf->if_name != NULL; intf++)
	{

		plugged = Unplugged_st;
		for (pCurrentAdapter = pAdapterInfo, n = 0;
				pCurrentAdapter != NULL;
				pCurrentAdapter = pCurrentAdapter->ifa_next, n++)
		{
			if (pCurrentAdapter->ifa_addr != NULL)
			{
				family = pCurrentAdapter->ifa_addr->sa_family;
				currentIndex = if_nametoindex(pCurrentAdapter->ifa_name);
				if ((intf->if_index == currentIndex) && ((AF_INET == family)))
						//||(AF_INET6 == family)))
				{
#ifdef ETH_THREAD_DEBUG
					printf("%-8s %s (%d)\n",
							pCurrentAdapter->ifa_name,
							(family == AF_INET) ? "AF_INET" :
									(family == AF_INET6) ? "AF_INET6" : "???",
											family);
#endif
					plugged = Plugged_st;
					plugAdapter(pCurrentAdapter, currentIndex);
				}
			}
		}
		if (Unplugged_st == plugged)
		{
			unplugAdapter(intf->if_index);
		}
	}
}
static
void *
ethThread(void* par)
{
#ifdef ETH_THREAD_DEBUG
	printf("\nehtThread started\n");
#endif
	struct if_nameindex *if_ni;
	struct ifaddrs *pAdapterInfo;
	int dwRetVal;

	config_clientId(MQTT_CLIENT_ID);
	config_topic(MQTT_CLIENT_ID);

	while (running)
	{
		sleep(SLEEP_LAPSE_ETH_THREAD);
		if_ni = if_nameindex();
		if (if_ni != NULL) {
#ifdef ETH_THREAD_DEBUG
			struct if_nameindex *intf;
			printf("\nAvailable interfaces:\n");
			for (intf = if_ni; intf->if_index != 0 || intf->if_name != NULL; intf++)
			{
				printf("\t%s\n", intf->if_name);
			}
			printf("\n")
#endif
			if ((dwRetVal = getifaddrs(&pAdapterInfo)) == 0)
			{
				check_AdapterStatus(if_ni, pAdapterInfo);
				freeifaddrs(pAdapterInfo);
			}
			else
			{
				printf("getifaddrs failed with error: %d\n", dwRetVal);
			}
			if_freenameindex(if_ni);
		}
		else
		{
			perror("if_nameindex");
		}
	}

	return NULL;
}
/* ---------------------------- Global functions --------------------------- */
void
eth_init(void)
{
	int ret;
	pthread_t ethId;
	pthread_attr_t thAtt;
	pthread_attr_init(&thAtt);
	ret = pthread_attr_setstacksize(&thAtt, ETH_THREAD_STACK_SIZE);
	if (EINVAL == ret)
	{
		printf("\nStack size is less than PTHREAD_STACK_MIN (16384) \
or not a multiple of the system page size. Used default size.\n");
	}
	ret = pthread_create(&ethId, NULL, &ethThread, NULL);
	switch (ret) {
		case EAGAIN:
			printf("Insufficient resources to create another thread.");
			break;
		case EINVAL:
			printf("Invalid settings in attributes.");
			break;
		case EPERM:
			printf("No permission to set the scheduling policy \
and parameters specified in attributes.");
			break;
		default:
			if (0 != ret)
			{
				printf("Couldn't create a new thread for unknown reasons");
			}
		break;
	}
	RKH_ASSERT(ethId != (pthread_t)0);

    running = 1;

}

void
eth_deinit(void)
{
    running = 0;
}

void
eth_socketOpen(char *ip, char *port)
{
	printf("Called eth_socketOpen()");
#ifdef OLD
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKADDR_IN target;
    unsigned short lport;
    char *p;
    int err;

    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    s = INVALID_SOCKET;

    if (err != 0)
    {
        printf("WSAStartup error %ld\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    target.sin_family = AF_INET;
    lport = (unsigned short)strtol(port, &p, 10);
    target.sin_port = htons(lport);
    target.sin_addr.s_addr = inet_addr(ip);

    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("socket error %ld\n", WSAGetLastError());
        WSACleanup();
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);
        return;
    }

    if (connect(s, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
    {
        printf("connect error %ld\n", WSAGetLastError());
        WSACleanup();
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);
        return;
    }

    RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_connected), &eth);
#endif
}

void
eth_socketWrite(rui8_t *p, ruint size)
{
	printf("Called eth_socketWrite()");
#ifdef OLD
    u_long mode;
    int ret;

    mode = 0;  /* 0 to enable blocking socket */
    ioctlsocket(s, FIONBIO, &mode);

    ret = send(s, (char *)p, size, 0);
    if (ret == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, 
                                            &e_disconnected), &eth);
    }
    else
    {
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Ok), &eth);
    }
#endif
}

ruint
eth_socketRead(rui8_t *p, ruint size)
{
    int ret;

    /* enable non-blocking socket */
    fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);

    ret = recv(s, (char *)p, size, 0);
    if (ret == 0)
    {
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
                                                &e_disconnected), &eth);
        return 0;
    }
    else if (ret < 0 )
    {
        ret = errno;
        switch (ret)
        {
            case EWOULDBLOCK:
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
