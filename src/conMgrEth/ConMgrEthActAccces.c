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
#include "MqttProt.h"
#include "ConMgrEthActAccess.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_ROM_STATIC_EVENT(e_Sent,     evSent);
static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);
static RKH_ROM_STATIC_EVENT(e_RecvFail, evRecvFail);
ReceivedEvt e_Received;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
init(void)
{
    ConnectionTopic_subscribe(conMgrEth);
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

/* ------------------------------ End of file ------------------------------ */
