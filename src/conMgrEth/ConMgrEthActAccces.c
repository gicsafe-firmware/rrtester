/**
 *  \file       ConMgrEthAccess.c
 *  \brief
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include "bsp.h"
#include "eth.h"
#include "signals.h"
#include "events.h"
#include "topics.h"
#include "config.h"
#include "ConMgrEth.h"
#include "ConMgrEthActAccess.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_ROM_STATIC_EVENT(e_Sent,     evSent);
static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);
static RKH_ROM_STATIC_EVENT(e_RecvFail, evRecvFail);
static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);

static ReceivedEvt e_Received;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
conMgrEthActAccess_init(ConMgrEth *const me)
{
    me->ip = CONNECTION_DOMAIN;
    me->port = CONNECTION_PORT;

    ConnectionTopic_subscribe(conMgrEth);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Received), evReceived);
	
    RKH_TR_FWK_SIG(evRecvFail);
    RKH_TR_FWK_SIG(evSendFail);
    RKH_TR_FWK_SIG(evReceived);
    RKH_TR_FWK_SIG(evSent);
}

void
conMgrEthActAccess_socketOpen(char *ip, char *port)
{
    bsp_linkStatus(ETHNetwork, DisconnectedSt);
    eth_socketOpen(ip, port);
    bsp_linkStatus(ETHNetwork, ConnectedSt);
}

void
recvFail(void)
{
    ConnectionTopic_publish(&e_RecvFail, conMgrEth);
}

void
sendFail(void)
{
    ConnectionTopic_publish(&e_SendFail, conMgrEth);
}

void
sendOk(void)
{
    ConnectionTopic_publish(&e_Sent, conMgrEth);
}

void
recvOk(void)
{
    ConnectionTopic_publish(&e_Received, conMgrEth);
}

void
conMgrEthActAccess_socketConnected(ConMgrEth *const me)
{
    ConnectionTopic_publish(&e_NetConnected, me);
    bsp_socketStatus(ETHNetwork, ConnectedSt);
}

void
conMgrEthActAccess_socketDisconnected(ConMgrEth *const me)
{
    ConnectionTopic_publish(&e_NetDisconnected, me);
    bsp_socketStatus(ETHNetwork, DisconnectedSt);
}

void
conMgrEthActAccess_socketWrite(SendEvt *p)
{
    eth_socketWrite(p->buf, p->size);
}

void
conMgrEthActAccess_socketRead(ConMgrEth *const me)
{
    (void)me;

    e_Received.size = eth_socketRead(e_Received.buf, sizeof(e_Received.buf));
}

/* ------------------------------ End of file ------------------------------ */
