/**
 *  \file       ConMgrEth.c
 *  \brief      Active object's action implementations.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "conMgrEth_signal.h"
#include "ConMgrEth.h"
#include "ConMgrEthAct.h"
#include "bsp.h"
#include "ConMgrEthActRequired.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ............................ Effect actions ............................. */
void 
ConMgrEth_ToinactiveExt0(ConMgrEth *const me, RKH_EVT_T *pe)
{
	RKH_TR_FWK_AO(me);
	RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
	RKH_TR_FWK_STATE(me, &inactive);
	RKH_TR_FWK_STATE(me, &active);
	RKH_TR_FWK_STATE(me, &unplugged);
	RKH_TR_FWK_STATE(me, &plugged);
	RKH_TR_FWK_STATE(me, &connecting);
	RKH_TR_FWK_STATE(me, &connected);
	RKH_TR_FWK_STATE(me, &idle);
	RKH_TR_FWK_STATE(me, &receiving);
	RKH_TR_FWK_STATE(me, &sending);
	RKH_TR_FWK_STATE(me, &waitServer);
	RKH_TR_FWK_STATE(me, &waitIp);
	RKH_TR_FWK_SIG(evOpen);
	RKH_TR_FWK_SIG(evClose);
	RKH_TR_FWK_SIG(evOk);
	RKH_TR_FWK_SIG(evError);
	RKH_TR_FWK_SIG(evEthLinkConnect);
	RKH_TR_FWK_SIG(evEthLinkDisconnect);
	RKH_TR_FWK_SIG(evIPStatus);
	RKH_TR_FWK_SIG(evConnected);
	RKH_TR_FWK_SIG(evDisconnected);
	RKH_TR_FWK_SIG(evRecv);
	RKH_TR_FWK_SIG(evSend);
	#if 1
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_ToinactiveExt0, "ToinactiveExt0");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_idleToreceivingExt7, "idleToreceivingExt7");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_idleTosendingExt8, "idleTosendingExt8");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_receivingToidleExt10, "receivingToidleExt10");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_receivingToidleExt11, "receivingToidleExt11");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_sendingToidleExt12, "sendingToidleExt12");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_inactiveToinactiveLoc0, "inactiveToinactiveLoc0");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_inactiveToinactiveLoc1, "inactiveToinactiveLoc1");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_activeToactiveLoc2, "activeToactiveLoc2");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_activeToactiveLoc3, "activeToactiveLoc3");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_enconnected, "enconnected");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_enwaitServer, "enwaitServer");
		RKH_TR_FWK_OBJ_NAME(ConMgrEth_exconnected, "exconnected");
	#endif
	
	init();
}

void 
ConMgrEth_idleToreceivingExt7(ConMgrEth *const me, RKH_EVT_T *pe)
{
	socketRead();
}

void 
ConMgrEth_idleTosendingExt8(ConMgrEth *const me, RKH_EVT_T *pe)
{
	socketWrite();
}

void 
ConMgrEth_receivingToidleExt10(ConMgrEth *const me, RKH_EVT_T *pe)
{
	recvOk();
}

void 
ConMgrEth_receivingToidleExt11(ConMgrEth *const me, RKH_EVT_T *pe)
{
	recvFail();
}

void 
ConMgrEth_sendingToidleExt12(ConMgrEth *const me, RKH_EVT_T *pe)
{
	sendOk();
}

void 
ConMgrEth_inactiveToinactiveLoc0(ConMgrEth *const me, RKH_EVT_T *pe)
{
	recvFail();
}

void 
ConMgrEth_inactiveToinactiveLoc1(ConMgrEth *const me, RKH_EVT_T *pe)
{
	sendFail();
}

void 
ConMgrEth_activeToactiveLoc2(ConMgrEth *const me, RKH_EVT_T *pe)
{
	recvFail();
}

void 
ConMgrEth_activeToactiveLoc3(ConMgrEth *const me, RKH_EVT_T *pe)
{
	sendFail();
}

/* ............................. Entry actions ............................. */
void 
ConMgrEth_enconnected(ConMgrEth *const me)
{
	socketConnected();
}

void 
ConMgrEth_enwaitServer(ConMgrEth *const me)
{
	socketOpen(me->ip, me->port);
}

/* ............................. Exit actions .............................. */
void 
ConMgrEth_exconnected(ConMgrEth *const me)
{
	socketDisconnected();
}

/* ................................ Guards ................................. */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
