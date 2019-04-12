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
#include "signals.h"
#include "events.h"
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

}

void
recvFail(void)
{
    RKH_SMA_POST_FIFO(mqttProt, &e_RecvFail, conMgrEth);
}

void
sendFail(void)
{
    RKH_SMA_POST_FIFO(mqttProt, &e_SendFail, conMgrEth);
}

void
sendOk(void)
{
    RKH_SMA_POST_FIFO(mqttProt, &e_Sent, conMgrEth);
}

void
recvOk(void)
{
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Received), evReceived);
}

/* ------------------------------ End of file ------------------------------ */
