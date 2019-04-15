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
void init(void);
void eth_socketOpen(void);
void eth_socketWrite(void);
void eth_socketRead(void);
void eth_socketConnected(void);
void eth_socketDisconnected(void);
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
