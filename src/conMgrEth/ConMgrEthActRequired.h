/**
 *  \file       ConMgrEthActRequired.h
 *  \brief
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONMGRETHACTREQUIRED_H__
#define __CONMGRETHACTREQUIRED_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhtype.h"
#include "events.h"
#include "ConMgrEthActAccess.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
	extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
#define init()                  conMgrEthActAccess_init(me)
#define socketOpen(ip,port)     conMgrEthActAccess_socketOpen(ip,port)
#define socketWrite()           conMgrEthActAccess_socketWrite(RKH_UPCAST(SendEvt, pe))
#define socketRead()            conMgrEthActAccess_socketRead(me)
#define socketConnected()       conMgrEthActAccess_socketConnected(me)
#define socketDisconnected()    conMgrEthActAccess_socketDisconnected(me)

void recvOk(void);
void sendOk(void);
void recvFail(void);
void sendFail(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
