/**
 *  \file       SigMon.c
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
#include "rkhfwk_cast.h"
#include "rkhassert.h"
#include "signal.h"
#include "SigMon.h"
#include "SigMonAct.h"
#include "bsp.h"
#include "SigMonActRequired.h"
#include "DigIn.h"
#include "Relay.h"
#include "StoreTest.h"

RKH_MODULE_NAME(SigMonAct)

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static const RKH_SIG_T mapDigIn[7] = 
{
    evIn0, evIn1, evIn2, 0, evIn4, evIn5, evIn6
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void 
calcAnSmp(SigMon *const me)
{
	++me->nAnSmp;
    me->currVal += Relay_getCurrent();
    me->currVal /= 2;
    me->voltVal += Relay_getVoltage();
    me->voltVal /= 2;
}

/* ............................ Effect actions ............................. */
void 
SigMon_SMActiveToSigMon_FinalExt3(SigMon *const me, RKH_EVT_T *pe)
{
    StoreTest_setFailure();
}

void 
SigMon_Seq2ToSeq3Ext8(SigMon *const me, RKH_EVT_T *pe)
{
    StoreTest_setRelayParam(me->currVal, me->voltVal);
}

void 
SigMon_SMActiveToSMActiveLoc2(SigMon *const me, RKH_EVT_T *pe)
{
	DigIn status = DigIn_get();
	me->digIn = ((status.clk << 2) | (status.clkX3 << 1) | status.clkX6);
    RKH_ENSURE(me->digIn < (1 << 3));
    me->evInObj.e = mapDigIn[me->digIn];
    RKH_SMA_POST_LIFO(RKH_UPCAST(RKH_SMA_T, me), &me->evInObj, me);
}

void 
SigMon_Seq0ToSeq0Loc4(SigMon *const me, RKH_EVT_T *pe)
{
    calcAnSmp(me);
}

void 
SigMon_Seq2ToSeq2Loc5(SigMon *const me, RKH_EVT_T *pe)
{
    calcAnSmp(me);
}

void 
SigMon_Seq1ToSeq1Loc6(SigMon *const me, RKH_EVT_T *pe)
{
    calcAnSmp(me);
}

/* ............................. Entry actions ............................. */
void 
SigMon_enSMActive(SigMon *const me)
{
    RKH_SET_STATIC_EVENT(&me->evInObj, 0);
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &me->evSyncObj), evSync);
    RKH_TMR_INIT(&me->evSyncObj.tmr, RKH_UPCAST(RKH_EVT_T, &me->evSyncObj), 
                 NULL);
    RKH_TMR_PERIODIC(&me->evSyncObj.tmr, RKH_UPCAST(RKH_SMA_T, me), 
                     SIGMON_SYNC_TIME, SIGMON_SYNC_TIME);
}

void 
SigMon_enSeq0(SigMon *const me)
{
	me->nAnSmp = 1;
    me->currVal = Relay_getCurrent();
    me->voltVal = Relay_getVoltage();
}

/* ............................. Exit actions .............................. */
void 
SigMon_exSMActive(SigMon *const me)
{
    rkh_tmr_stop(&me->evSyncObj.tmr);
}

/* ................................ Guards ................................. */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
