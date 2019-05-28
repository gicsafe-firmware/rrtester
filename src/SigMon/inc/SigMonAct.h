/**
 *  \file       SigMon.h
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
#ifndef __SIGMONACT_H__
#define __SIGMONACT_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "SigMon.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define SIGMON_CLOCK_PER    500 /* in [ms] */
#define SIGMON_DIGIN_TICKS  10
#define SIGMON_SYNC_TIME    RKH_TIME_MS(SIGMON_CLOCK_PER/SIGMON_DIGIN_TICKS)

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* ........................ Declares effect actions ........................ */
void SigMon_ToSMInactiveExt0(SigMon *const me, RKH_EVT_T *pe);
void SigMon_SMActiveToSigMon_FinalExt3(SigMon *const me, RKH_EVT_T *pe);
void SigMon_Seq2ToSeq3Ext8(SigMon *const me, RKH_EVT_T *pe);
void SigMon_SMActiveToSMActiveLoc2(SigMon *const me, RKH_EVT_T *pe);
void SigMon_Seq0ToSeq0Loc4(SigMon *const me, RKH_EVT_T *pe);
void SigMon_Seq2ToSeq2Loc5(SigMon *const me, RKH_EVT_T *pe);
void SigMon_Seq1ToSeq1Loc6(SigMon *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
void SigMon_enSMActive(SigMon *const me);
void SigMon_enSeq0(SigMon *const me);

/* ......................... Declares exit actions ......................... */
void SigMon_exSMActive(SigMon *const me);

/* ............................ Declares guards ............................ */

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
