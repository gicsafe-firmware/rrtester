/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SIGMON_H__
#define __SIGMON_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"
#include "rkhtmr.h"

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
typedef struct SigMonTmEvt SigMonTmEvt;
struct SigMonTmEvt 
{
    RKH_EVT_T evt;
    RKH_TMR_T tmr;
};

/* ............................. Active object ............................. */
typedef struct SigMon SigMon;
struct SigMon
{
    RKH_SMA_T sma;      /* base structure */
    rInt digIn;         /* maintains the last digital input status */
                        /* digIn[0] : clock x 6 */
                        /* digIn[1] : clock x 3 */
                        /* digIn[2] : clock x 1 */
    SigMonTmEvt evSyncObj;
    RKH_EVT_T evInObj;
    rInt nAnSmp;        /* number of analog samples to average */
    rui16_t currVal;
    rui16_t voltVal;
    rInt nDigIn;        /* counter for storing digital inputs */
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
