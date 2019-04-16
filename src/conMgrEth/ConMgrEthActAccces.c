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
#include "signals.h"
#include "events.h"
#include "topics.h"
#include "ConMgrEth.h"
#include "ConMgrEthActAccess.h"
#include "conmgr.h"
#include "MqttProt.h"
#include "eth.h"

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

ReceivedEvt e_Received;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
_init(ConMgrEth *const me)
{
	//me->ip = "127.0.0.1";
	me->ip = AZURE_SERVER;
	me->port = AZURE_PORT;
		
    ConnectionTopic_subscribe(conMgrEth);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Received), evReceived);
}

void 
_socketOpen(char *ip, char *port)
{
    eth_socketOpen(ip, port);
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
_socketConnected(ConMgrEth *const me)
{
    ConnectionTopic_publish(&e_NetConnected, me);
}

void
_socketDisconnected(ConMgrEth *const me)
{
    ConnectionTopic_publish(&e_NetDisconnected, me);
}

void
_socketWrite(SendEvt *p)
{
    eth_socketWrite(p->buf, p->size);
}

void
_socketRead(ConMgrEth *const me)
{
    e_Received.size = eth_socketRead(e_Received.buf, sizeof(e_Received.buf));
}

/* ------------------------------ End of file ------------------------------ */
