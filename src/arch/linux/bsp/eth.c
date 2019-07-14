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
#include <string.h>
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
//#define ETH_OUTPUT_LOG
static const int SLEEP_LAPSE_ETH_THREAD = 4;
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
				if ( (intf->if_index == currentIndex) &&
						((AF_INET == family)) //||(AF_INET6 == family)))
						&& (strcmp("lo",pCurrentAdapter->ifa_name)) )
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

int
sendall(int s, char *buf, ruint *len)
{
	/*From Beej's Guide*/
	int total = 0; // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;
	while(total < *len) {
		n = send(s, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n==-1?-1:0; // return -1 on failure, 0 on success
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
	int status;
	struct addrinfo conf;
	struct addrinfo *target;
	memset(&conf, 0, sizeof conf);

	conf.ai_family = AF_UNSPEC; 	// IPv6 friendly
	conf.ai_socktype = SOCK_STREAM;
	conf.ai_flags = AI_PASSIVE; 	// fill in my IP for me
	conf.ai_protocol = IPPROTO_TCP;
	if ((status = getaddrinfo(ip, port, &conf, &target)) != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

    s = socket (target->ai_family, target->ai_socktype, target->ai_protocol);
    if (s == -1)
    {
        printf("socket error %d\n", errno);
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);
        return;
    }

    if (connect(s, target->ai_addr, target->ai_addrlen) == -1)
    {
        printf("connect error %d\n", errno);
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Error), &eth);
        return;
    }

    RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_connected), &eth);
}

void
eth_socketClose(void)
{
	close(s);
	RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_disconnected), &eth);
}

void
eth_socketWrite(rui8_t *p, ruint size)
{
    int ret;

    /* enable blocking socket */
    /* Does it really need to block?*/
    ret = sendall(s, (char *)p, &size);
    if (ret < 0)
    {
        printf("send failed with error: %d\n", errno);
        close(s);
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, 
                                            &e_disconnected), &eth);
    }
    else
    {
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_Ok), &eth);
#ifdef ETH_OUTPUT_LOG
        FILE *fETHLog = NULL;
        if ((fETHLog = fopen("eth.log","a+")) == NULL)
        	printf("Can't open eth.log file for bsp's eth module\n");
        fwrite((char *)p, sizeof(char), (size_t)size, fETHLog);
        fflush(fETHLog);
        fclose(fETHLog);
#endif
    }
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
