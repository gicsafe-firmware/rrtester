/**
 *  \file       eth.c
 *  \brief      Win32 Ethernet socket abstraction.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include "rkhfwk_dynevt.h"
#include "bsp.h"
#include "signals.h"
#include "topics.h"
#include "ConMgrEth.h"
#include "conmgr.h"

//#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

RKH_THIS_MODULE

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
enum
{
    Unplugged_st, Plugged_st   
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static rui8_t eth;
static SOCKET s;
static unsigned adapter[9] = { Unplugged_st, Unplugged_st, Unplugged_st, 
                               Unplugged_st, Unplugged_st, Unplugged_st,
                               Unplugged_st, Unplugged_st, Unplugged_st };

static RKH_STATIC_EVENT(e_linkConnected, evEthLinkConnect);
static RKH_STATIC_EVENT(e_linkDisconnect, evEthLinkDisconnect);
static RKH_STATIC_EVENT(e_ipStatus, evIPStatus);
static RKH_STATIC_EVENT(e_connected, evConnected);
static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
print_adapterInfo(PIP_ADAPTER_INFO p)
{
    UINT i;
    errno_t error;
    char buffer[32];
/* variables used to print DHCP time info */
    struct tm newtime;


    printf("\tComboIndex: \t%d\n", p->ComboIndex);
    printf("\tAdapter Name: \t%s\n", p->AdapterName);
    printf("\tAdapter Desc: \t%s\n", p->Description);
    printf("\tAdapter Addr: \t");
    for (i = 0; i < p->AddressLength; i++) {
        if (i == (p->AddressLength - 1))
            printf("%.2X\n", (int) p->Address[i]);
        else
            printf("%.2X-", (int) p->Address[i]);
    }
    printf("\tIndex: \t%d\n", p->Index);
    printf("\tType: \t");
    switch (p->Type) {
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

    if (p->DhcpEnabled) {
        printf("\tDHCP Enabled: Yes\n");
        printf("\t  DHCP Server: \t%s\n",
               p->DhcpServer.IpAddress.String);

        printf("\t  Lease Obtained: ");
        /* Display local time */
        error = _localtime32_s(&newtime, (__time32_t*) &p->LeaseObtained);
        if (error)
            printf("Invalid Argument to _localtime32_s\n");
        else {
            // Convert to an ASCII representation 
            error = asctime_s(buffer, 32, &newtime);
            if (error)
                printf("Invalid Argument to asctime_s\n");
            else
                /* asctime_s returns the string terminated by \n\0 */
                printf("%s", buffer);
        }

        printf("\t  Lease Expires:  ");
        error = _localtime32_s(&newtime, (__time32_t*) &p->LeaseExpires);
        if (error)
            printf("Invalid Argument to _localtime32_s\n");
        else {
            // Convert to an ASCII representation 
            error = asctime_s(buffer, 32, &newtime);
            if (error)
                printf("Invalid Argument to asctime_s\n");
            else
                /* asctime_s returns the string terminated by \n\0 */
                printf("%s", buffer);
        }
    } else
        printf("\tDHCP Enabled: No\n");

    if (p->HaveWins) {
        printf("\tHave Wins: Yes\n");
        printf("\t  Primary Wins Server:    %s\n",
               p->PrimaryWinsServer.IpAddress.String);
        printf("\t  Secondary Wins Server:  %s\n",
               p->SecondaryWinsServer.IpAddress.String);
    } else
        printf("\tHave Wins: No\n");
    printf("\n");
}

void
check_adapterStatus(PIP_ADAPTER_INFO p, unsigned int i)
{

    if( (p->IpAddressList.IpAddress.String != NULL) && 
        (p->GatewayList.IpAddress.String != NULL) 
      )
    {
        if(adapter[i] == Unplugged_st )
        {
            adapter[i] = Plugged_st;
            RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_linkConnected), &eth);
            RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_ipStatus), &eth);
        }

//        retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len)int retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len)
    }
    else
    {
        if(adapter[i] == Plugged_st )
        {
            adapter[i] = Plugged_st;
            RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_linkDisconnect),
						      &eth);
        }

    }
}

static
DWORD WINAPI
ethThread(LPVOID par)
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    LONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        exit(EXIT_FAILURE);
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            exit(EXIT_FAILURE);
        }
    }

    for(;;)
    {
        if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
            pAdapter = pAdapterInfo;
            while (pAdapter) {
                check_adapterStatus(pAdapter, i++);
                pAdapter = pAdapter->Next;
            }
        } else {
            printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
        }
        
	    Sleep(1000);
    }

    if (pAdapterInfo)
        FREE(pAdapterInfo);
}

/* ---------------------------- Global functions --------------------------- */
void
eth_init(void)
{
    DWORD ethId;
    HANDLE ethTh;


    ethTh = CreateThread(NULL, 1024, &ethThread, 0, 0, &ethId);
    RKH_ASSERT(ethTh != (HANDLE)0);
    SetThreadPriority(ethTh, THREAD_PRIORITY_NORMAL);
}

void
eth_socketOpen(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKADDR_IN target; /* Socket address information */
    unsigned short port;
    char *p;
    int err;

    /* --- INITIALIZATION ----------------------------------- */
    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    s = INVALID_SOCKET;

    if (err != 0)
    {
        printf("WSAStartup error %ld", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    /* ------------------------------------------------------ */

    /* ---- build address structure to bind to socket.-------- */
    target.sin_family = AF_INET;
    port = strtol(AZURE_PORT, &p, 10);
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(AZURE_SERVER);
    /* ------------------------------------------------------ */

    /* ---- create SOCKET-------------------------------------- */
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("socket error %ld", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    /* ------------------------------------------------------ */

    /* ---- try CONNECT ----------------------------------------- */
    if (connect(s, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
    {
        printf("connect error %ld", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T, &e_connected), &eth);
}

void
eth_socketConnected(void)
{
    ConnectionTopic_publish(&e_NetConnected, conMgrEth);
}

void
eth_socketDisconnected(void)
{
    ConnectionTopic_publish(&e_NetDisconnected, conMgrEth);
}

void
eth_socketWrite(void)
{

}

void
eth_socketRead(void)
{

}

/* ------------------------------ End of file ------------------------------ */
