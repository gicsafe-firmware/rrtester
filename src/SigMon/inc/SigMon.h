/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SIGMON_H__
#define __SIGMON_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(sigMon);

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE SMInactive, WaitSyncSeq, Seq0, Seq2, Seq3, Seq4, Seq5, Seq11, OutOfSeq, Seq1, Seq10, Seq8, Seq7, Seq6, Seq9;
RKH_DCLR_COMP_STATE SMActive;
RKH_DCLR_FINAL_STATE SigMon_Final;

/* ------------------------------- Data types ------------------------------ */
/* ............................. Active object ............................. */
typedef struct SigMon SigMon;
struct SigMon
{
    RKH_SMA_T sma;      /* base structure */
    rInt digIn;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
