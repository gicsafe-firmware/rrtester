/**
 *  \file       ConMgrEthActAccess.h
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
#ifndef __CONMGRETHACTACCESS_H__
#define __CONMGRETHACTACCESS_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
	extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void conMgrEthActAccess_init(ConMgrEth *const me);
void conMgrEthActAccess_socketOpen(char *ip, char *port);
void conMgrEthActAccess_socketConnected(ConMgrEth *const me);
void conMgrEthActAccess_socketDisconnected(ConMgrEth *const me);
void conMgrEthActAccess_socketWrite(SendEvt *p);
void conMgrEthActAccess_socketRead(ConMgrEth *const me);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
