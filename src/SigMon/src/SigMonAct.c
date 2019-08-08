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
#include "rkhtrc.h"
#include "rkhtrc_define.h"
#include "rkhtrc_filter.h"
#include "rkhtrc_record.h"
#include "rkhassert.h"
#include "signals.h"
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
    me->currVal = Relay_getCurrent();
    me->voltVal = Relay_getVoltage();
}

/* ............................ Effect actions ............................. */
void 
SigMon_ToSMInactiveExt0(SigMon *const me, RKH_EVT_T *pe)
{
    RKH_TR_FWK_AO(me);
    RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
    RKH_TR_FWK_STATE(me, &SMInactive);
    RKH_TR_FWK_STATE(me, &SMActive);
    RKH_TR_FWK_STATE(me, &WaitSyncSeq);
    RKH_TR_FWK_STATE(me, &Seq2);
    RKH_TR_FWK_STATE(me, &Seq3);
    RKH_TR_FWK_STATE(me, &Seq4);
    RKH_TR_FWK_STATE(me, &Seq5);
    RKH_TR_FWK_STATE(me, &Seq11);
    RKH_TR_FWK_STATE(me, &OutOfSeq);
    RKH_TR_FWK_STATE(me, &Seq1);
    RKH_TR_FWK_STATE(me, &Seq10);
    RKH_TR_FWK_STATE(me, &Seq8);
    RKH_TR_FWK_STATE(me, &Seq7);
    RKH_TR_FWK_STATE(me, &Seq6);
    RKH_TR_FWK_STATE(me, &Seq9);
    RKH_TR_FWK_SIG(evSync);
    RKH_TR_FWK_SIG(evEnable);
    RKH_TR_FWK_SIG(evDisable);
    RKH_TR_FWK_SIG(evFailure);
    RKH_TR_FWK_SIG(evIn0);
    RKH_TR_FWK_SIG(evIn1);
    RKH_TR_FWK_SIG(evIn2);
    RKH_TR_FWK_SIG(evIn4);
    RKH_TR_FWK_SIG(evIn5);
    RKH_TR_FWK_SIG(evIn6);
#if 0
    RKH_TR_FWK_OBJ_NAME(SigMon_ToSMInactiveExt0, "ToSMInactiveExt0");
    RKH_TR_FWK_OBJ_NAME(SigMon_SMActiveToSigMon_FinalExt3, "SMActiveToSigMon_FinalExt3");
    RKH_TR_FWK_OBJ_NAME(SigMon_Seq2ToSeq3Ext8, "Seq2ToSeq3Ext8");
    RKH_TR_FWK_OBJ_NAME(SigMon_SMActiveToSMActiveLoc2, "SMActiveToSMActiveLoc2");
    RKH_TR_FWK_OBJ_NAME(SigMon_Seq0ToSeq0Loc4, "Seq0ToSeq0Loc4");
    RKH_TR_FWK_OBJ_NAME(SigMon_Seq2ToSeq2Loc5, "Seq2ToSeq2Loc5");
    RKH_TR_FWK_OBJ_NAME(SigMon_Seq1ToSeq1Loc6, "Seq1ToSeq1Loc6");
    RKH_TR_FWK_OBJ_NAME(SigMon_enSMActive, "enSMActive");
    RKH_TR_FWK_OBJ_NAME(SigMon_enSeq0, "enSeq0");
    RKH_TR_FWK_OBJ_NAME(SigMon_exSMActive, "exSMActive");
#endif
    StoreTest_init();
    me->digIn.clk = 0;
    me->digIn.clkX3 = 0;
    me->digIn.clkX6 = 0;
    me->digIn.failure = 0;
}

void 
SigMon_SMActiveToSigMon_FinalExt3(SigMon *const me, RKH_EVT_T *pe)
{
    StoreTest_saveDigInStatus(me->digIn);
}

void 
SigMon_Seq1ToSeq2Ext7(SigMon *const me, RKH_EVT_T *pe)
{
    calcAnSmp(me);
}

void 
SigMon_Seq2ToSeq3Ext8(SigMon *const me, RKH_EVT_T *pe)
{
    StoreTest_saveRelayStatus(me->currVal, me->voltVal);
}

void 
SigMon_SMActiveToSMActiveLoc2(SigMon *const me, RKH_EVT_T *pe)
{
    int clockVal;
    DigIn digIn;

	digIn = DigIn_get();
    if (digIn.failure == 0)
    {
        clockVal = ((digIn.clk << 2) | 
                    (digIn.clkX3 << 1) | 
                    (digIn.clkX6));
        RKH_ENSURE(clockVal < (1 << 3));
        me->evInObj.e = mapDigIn[clockVal];
    }
    else
    {
        me->evInObj.e = evFailure;
    }
    RKH_SMA_POST_LIFO(RKH_UPCAST(RKH_SMA_T, me), &me->evInObj, me);

    if((digIn.clk == 1) && (digIn.clk != me->digIn.clk))
    {
        StoreTest_saveDigInStatus(digIn);
    }
    
    me->digIn = digIn;
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
SigMon_enSeq1(SigMon *const me)
{
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
