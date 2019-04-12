/**
 *  \file       ConMgrEth.h
 *  \brief      Active object's action specifications.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONMGRETHACT_H__
#define __CONMGRETHACT_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "ConMgrEth.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* ........................ Declares effect actions ........................ */
void ConMgrEth_ToinactiveExt0(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_idleToreceivingExt7(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_idleTosendingExt8(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_receivingToidleExt10(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_sendingToidleExt11(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_inactiveToinactiveLoc0(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_inactiveToinactiveLoc1(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_activeToactiveLoc2(ConMgrEth *const me, RKH_EVT_T *pe);
void ConMgrEth_activeToactiveLoc3(ConMgrEth *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
void ConMgrEth_enwaitServer(ConMgrEth *const me);

/* ......................... Declares exit actions ......................... */

/* ............................ Declares guards ............................ */

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
