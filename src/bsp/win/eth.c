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
#include "config.h"
#include "ConMgrEth.h"

#include <winsock.h>
#include <stdint.h>
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

void
check_adapterStatus(PIP_ADAPTER_INFO p, unsigned int i)
{
    if ((p->IpAddressList.IpAddress.String != NULL) &&
        (strcmp(p->IpAddressList.IpAddress.String, "0.0.0.0") != 0) &&
        (p->GatewayList.IpAddress.String != NULL) &&
        (strcmp(p->GatewayList.IpAddress.String, "0.0.0.0") != 0)
        )
    {
        if (adapter[i] == Unplugged_st)
        {
            adapter[i] = Plugged_st;
            RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
                                                    &e_linkConnected), &eth);
            RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
                                                    &e_ipStatus), &eth);
            printf("Using Ethernet Adapter: %d\n", i);
            print_adapterInfo(p);
        }
    }
    else
    {
        if (adapter[i] == Plugged_st)
        {
            adapter[i] = Unplugged_st;
            RKH_SMA_POST_FIFO(conMgrEth,
                              RKH_UPCAST(RKH_EVT_T, &e_linkDisconnect),
                              &eth);
        }
    }
}

static
DWORD WINAPI
ethThread(LPVOID par)
{
    (void)par;

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    LONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    DWORD dwRetVal = 0;
    unsigned int i = 0;

    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        exit(EXIT_FAILURE);
    }

    if (GetAdaptersInfo(pAdapterInfo,
                        (PULONG)&ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            exit(EXIT_FAILURE);
        }
    }

    while (running)
    {
        Sleep(1000);
        if ((dwRetVal =
                 GetAdaptersInfo(pAdapterInfo,
                                 (PULONG)&ulOutBufLen)) == NO_ERROR)
        {
            pAdapter = pAdapterInfo;
            i = 0;

            while (pAdapter)
            {
                check_adapterStatus(pAdapter, i++);
                pAdapter = pAdapter->Next;
            }
        }
        else
        {
            printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
        }
    }

    if (pAdapterInfo)
    {
        FREE(pAdapterInfo);
    }

    return 0;
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
}

void
eth_socketWrite(rui8_t *p, ruint size)
{
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
}

ruint
eth_socketRead(rui8_t *p, ruint size)
{
    int ret;
    u_long mode;

    mode = 1;  /* 1 to enable non-blocking socket */
    ioctlsocket(s, FIONBIO, &mode);

    ret = recv(s, (char *)p, size, 0);
    if (ret == 0)
    {
        RKH_SMA_POST_FIFO(conMgrEth, RKH_UPCAST(RKH_EVT_T,
                                                &e_disconnected), &eth);
        return 0;
    }
    else if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        switch (ret)
        {
            case WSAEWOULDBLOCK:
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

ruint
eth_getMACaddress(char macBuff[])
{
    int i;
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    LONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    DWORD dwRetVal = 0;
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        exit(EXIT_FAILURE);
    }
    if (GetAdaptersInfo(pAdapterInfo,
                        (PULONG)&ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            exit(EXIT_FAILURE);
        }
    }
    dwRetVal = GetAdaptersInfo(pAdapterInfo, (PULONG)&ulOutBufLen);
    if (dwRetVal == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        while (pAdapter)
        {
            if ((pAdapter->IpAddressList.IpAddress.String != NULL) &&
                (strcmp(pAdapter->IpAddressList.IpAddress.String,
                        "0.0.0.0") != 0) &&
                (pAdapter->GatewayList.IpAddress.String != NULL) &&
                (strcmp(pAdapter->GatewayList.IpAddress.String, "0.0.0.0") != 0)
                )
            {
                for (i = 0; i < 6; i++) /* If its IPv6, trucate it */
                {
                    macBuff[i] = pAdapter->Address[i];
                }
                break;
            }
            pAdapter = pAdapter->Next;
        }
    }
    else
    {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }
    if (pAdapterInfo)
    {
        FREE(pAdapterInfo);
    }
}

/* ------------------------------ End of file ------------------------------ */
