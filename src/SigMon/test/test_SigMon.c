/*
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 *            State-machine framework for reactive embedded systems
 *
 *                      Copyright (C) 2010 Leandro Francucci.
 *          All rights reserved. Protected by international copyright laws.
 *
 *
 *  RKH is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH site: http://vortexmakes.com/que-es/
 *  RKH GitHub: https://github.com/vortexmakes/RKH
 *  RKH Sourceforge: https://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail: lf@vortexmakes.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       test_smTransition.c
 *  \brief      Unit test for state machine module.
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  DaBa  Dario Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <unitrazer.h>
#include <tzlink.h>
#include <tzparse.h>
#include "rkhsm.h"
#include "common.h"
#include "signals.h"
#include "SigMon.h"
#include "Mock_SigMonAct.h"
#include "rkhport.h"
#include "rkhfwk_bittbl.h"
#include "rkhtrc.h"
#include "rkhtrc_filter.h"
#include "rkhtrc_record.h"
#include "rkhtrc_stream.h"
#include "Mock_rkhassert.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
static RKH_STATIC_EVENT(evEnableObj, evEnable);
static RKH_STATIC_EVENT(evDisableObj, evDisable);
static RKH_STATIC_EVENT(evFailureObj, evFailure);
static RKH_STATIC_EVENT(evSyncObj, evSync);
static RKH_STATIC_EVENT(evIn0Obj, evIn0);
static RKH_STATIC_EVENT(evIn1Obj, evIn1);
static RKH_STATIC_EVENT(evIn2Obj, evIn2);
static RKH_STATIC_EVENT(evIn4Obj, evIn4);
static RKH_STATIC_EVENT(evIn5Obj, evIn5);
static RKH_STATIC_EVENT(evIn6Obj, evIn6);
extern const RKH_EVT_T evCreation;

/* ---------------------------- Local data types --------------------------- */
typedef void (*ActExp)(void);
typedef struct TestInput TestInput;
struct TestInput
{
    RKH_SIG_T sig;
    const RKH_SBSC_T *nextState;
    ActExp actExp;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static const RKH_ST_T *targetStates[16], *exitStates[16], *entryStates[16];
static RKH_ST_T *expectedState;
static RKH_RCODE_T result;
static int first = 1;
static SMFilterCfg smFilterCfg;
static RKH_EVT_T evtObj;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
setUpUnitrazer(void)
{
    if (first)
    {
        tzlink_open(0, NULL);
    }

    first = 0;
    sm_init();
}

static void
loadStateMachineSymbols(void)
{
    RKH_TR_FWK_SIG(evEnable);
    RKH_TR_FWK_SIG(evDisable);
    RKH_TR_FWK_SIG(evFailure);
    RKH_TR_FWK_SIG(evSync);
    RKH_TR_FWK_SIG(evIn0);
    RKH_TR_FWK_SIG(evIn1);
    RKH_TR_FWK_SIG(evIn2);
    RKH_TR_FWK_SIG(evIn4);
    RKH_TR_FWK_SIG(evIn5);
    RKH_TR_FWK_SIG(evIn6);

    RKH_TR_FWK_AO(sigMon);
    RKH_TR_FWK_STATE(sigMon, &SMInactive);
    RKH_TR_FWK_STATE(sigMon, &SMActive);
    RKH_TR_FWK_STATE(sigMon, &Seq1);
    RKH_TR_FWK_STATE(sigMon, &Seq2);
    RKH_TR_FWK_STATE(sigMon, &Seq3);
    RKH_TR_FWK_STATE(sigMon, &Seq4);
    RKH_TR_FWK_STATE(sigMon, &Seq5);
    RKH_TR_FWK_STATE(sigMon, &Seq6);
    RKH_TR_FWK_STATE(sigMon, &Seq7);
    RKH_TR_FWK_STATE(sigMon, &Seq8);
    RKH_TR_FWK_STATE(sigMon, &Seq9);
    RKH_TR_FWK_STATE(sigMon, &Seq10);
    RKH_TR_FWK_STATE(sigMon, &Seq11);
    RKH_TR_FWK_STATE(sigMon, &OutOfSeq);
    RKH_TR_FWK_STATE(sigMon, &WaitSyncSeq);
}

static void
setRKHTraceFilters(void)
{
    SMFilterCfg *pSmFilterCfg;

    pSmFilterCfg = SMFilterCfg_init();
    smFilterCfg = *pSmFilterCfg;
}

static void
filterTraceOn(void)
{
    sm_init_ignore();
    sm_clrh_ignore();
    sm_trn_ignore();
    sm_state_ignore();
    sm_enstate_ignore();
    sm_exstate_ignore();
    sm_nenex_ignore();
    sm_ntrnact_ignore();
    sm_tsState_ignore();
    sm_evtProc_ignore();
    sm_evtNotFound_ignore();
    sm_grdFalse_ignore();
    sm_cndNotFound_ignore();
    sm_unknState_ignore();
    sm_exHLevel_ignore();
    sm_exTSeg_ignore();
    sm_exeAct_ignore();
    sm_dch_ignore();
}

static void
actExp0(void)
{
    SigMon_enSeq1_Expect((SigMon *)sigMon);
}

static void
actExp1(void)
{
    SigMon_Seq1ToSeq1Loc6_Expect((SigMon *)sigMon, &evtObj);
}

static void
actExp2(void)
{
    SigMon_Seq1ToSeq2Ext7_Expect((SigMon *)sigMon, &evtObj);
}

static void
actExp3(void)
{
    SigMon_Seq2ToSeq2Loc5_Expect((SigMon *)sigMon, &evtObj);
}

static void
actExp4(void)
{
    SigMon_Seq2ToSeq3Ext8_Expect((SigMon *)sigMon, &evtObj);
}

/* ---------------------------- Global functions --------------------------- */
void
setUp(void)
{
    setUpUnitrazer();
    Mock_SigMonAct_Init();
    loadStateMachineSymbols();
    setRKHTraceFilters();
}

void
tearDown(void)
{
    sm_verify(); /* Makes sure there are no unused expectations, if */
                 /* there are, this function causes the test to fail. */
    sm_cleanup();
    Mock_SigMonAct_Verify();
    Mock_SigMonAct_Destroy();
}

void
test_TrnFirstStateAfterInit(void)
{
    UtrzProcessOut *p;

    SigMon_ToSMInactiveExt0_Expect((SigMon *)sigMon, (RKH_EVT_T *)&evCreation);
    expectedInitSm(sigMon, RKH_STATE_CAST(&SMInactive));
    rkh_sm_init((RKH_SM_T *)sigMon);

    p = unitrazer_getLastOut();
    TEST_ASSERT_EQUAL(UT_PROC_SUCCESS, p->status);
}

void
test_Enable(void)
{
    UtrzProcessOut *p;
    int nExSt, nEnSt;
    RKH_EVT_T event;

    nEnSt = 2;
    nExSt = 1;
    event.e = evEnable;

    SigMon_ToSMInactiveExt0_Expect((SigMon *)sigMon, (RKH_EVT_T *)&evCreation);
    SigMon_enSMActive_Expect((SigMon *)sigMon);
    expectedInitSm((RKH_SMA_T *)sigMon, RKH_STATE_CAST(&SMInactive));
	sm_dch_expect(event.e, RKH_STATE_CAST(&SMInactive));
	sm_trn_expect(RKH_STATE_CAST(&SMInactive), RKH_STATE_CAST(&SMActive));
    sm_tsState_expect(RKH_STATE_CAST(&SMActive));
    sm_exstate_expect(RKH_STATE_CAST(&SMInactive));
    sm_enstate_expect(RKH_STATE_CAST(&SMActive));
    sm_tsState_expect(RKH_STATE_CAST(&WaitSyncSeq));
    sm_enstate_expect(RKH_STATE_CAST(&WaitSyncSeq));
    sm_nenex_expect(nEnSt, nExSt);
    sm_state_expect(RKH_STATE_CAST(&WaitSyncSeq));
	sm_evtProc_expect();

    rkh_sm_init((RKH_SM_T *)sigMon);
    rkh_sm_dispatch((RKH_SM_T *)sigMon, &evEnableObj);

    p = unitrazer_getLastOut();
    TEST_ASSERT_EQUAL(UT_PROC_SUCCESS, p->status);
}

void
test_Synchro(void)
{
    UtrzProcessOut *p;
    RKH_EVT_T *pEvt;

    pEvt = &evSyncObj;
    stateList_create(targetStates, 1, &SMActive);
    stateList_create(entryStates, 0);
    stateList_create(exitStates, 0);

    SigMon_ToSMInactiveExt0_Expect((SigMon *)sigMon, (RKH_EVT_T *)&evCreation);
    SigMon_SMActiveToSMActiveLoc2_Expect(RKH_CAST(SigMon, sigMon), pEvt);
    setProfile(sigMon, 
               RKH_STATE_CAST(&SMInactive),
               RKH_STATE_CAST(&WaitSyncSeq),
               RKH_STATE_CAST(&SMActive),
               targetStates, 
               entryStates, 
               exitStates, 
               RKH_STATE_CAST(&WaitSyncSeq), 
               1, 
               TRN_INTERNAL, 
               INIT_STATE_MACHINE,
               pEvt, 
               RKH_STATE_CAST(&WaitSyncSeq));

    rkh_sm_dispatch((RKH_SM_T *)sigMon, pEvt);

    p = unitrazer_getLastOut();
    TEST_ASSERT_EQUAL(UT_PROC_SUCCESS, p->status);
}

void
test_Failure(void)
{
    UtrzProcessOut *p;
    RKH_EVT_T *pEvt;

    pEvt = &evFailureObj;
    stateList_create(targetStates, 1, &SigMon_Final);
    stateList_create(entryStates, 1, SigMon_Final);
    stateList_create(exitStates, 2, &WaitSyncSeq, &SMActive);

    SigMon_ToSMInactiveExt0_Expect((SigMon *)sigMon, (RKH_EVT_T *)&evCreation);
    SigMon_exSMActive_Expect(RKH_CAST(SigMon, sigMon));
    SigMon_SMActiveToSigMon_FinalExt3_Expect(RKH_CAST(SigMon, sigMon), pEvt);
    setProfile(sigMon, 
               RKH_STATE_CAST(&SMInactive),
               RKH_STATE_CAST(&WaitSyncSeq),
               RKH_STATE_CAST(&SMActive),
               targetStates, 
               entryStates, 
               exitStates, 
               RKH_STATE_CAST(&SigMon_Final), 
               1, 
               TRN_NOT_INTERNAL, 
               INIT_STATE_MACHINE,
               pEvt, 
               RKH_STATE_CAST(&WaitSyncSeq));

    rkh_sm_dispatch((RKH_SM_T *)sigMon, pEvt);

    p = unitrazer_getLastOut();
    TEST_ASSERT_EQUAL(UT_PROC_SUCCESS, p->status);
}

void
test_InputSequence(void)
{
    UtrzProcessOut *p;
    int i;
    TestInput *pInSeq;
    static TestInput inSeq[] =
    {
        {evIn0, &WaitSyncSeq,   NULL},
        {evIn4, &WaitSyncSeq,   NULL},
        {evIn0, &WaitSyncSeq,   NULL},
        {evIn5, &WaitSyncSeq,   NULL},
        {evIn1, &Seq1,          actExp0},
        {evIn1, &Seq1,          actExp1},
        {evIn1, &Seq1,          actExp1},
        {evIn1, &Seq1,          actExp1},
        {evIn1, &Seq1,          actExp1},
        {evIn0, &Seq2,          actExp2},
        {evIn0, &Seq2,          actExp3},
        {evIn0, &Seq2,          actExp3},
        {evIn0, &Seq2,          actExp3},
        {evIn0, &Seq2,          actExp3},
        {evIn6, &Seq3,          actExp4},
        {evIn2, &Seq4,          NULL},
        {evIn4, &Seq5,          NULL},
        {evIn0, &Seq6,          NULL},
        {evIn4, &Seq7,          NULL},
        {evIn0, &Seq8,          NULL},
        {evIn4, &Seq9,          NULL},
        {evIn0, &Seq10,         NULL},
        {evIn5, &Seq11,         NULL},
        {evIn1, &Seq1,          actExp0}
    };

    filterTraceOn();
    setState(sigMon, RKH_STATE_CAST(&WaitSyncSeq));

    for (pInSeq = inSeq; pInSeq < inSeq + sizeof(inSeq)/sizeof(TestInput); 
        ++pInSeq)
    {
        evtObj.e = pInSeq->sig;
        if (pInSeq->actExp != (ActExp)0)
        {
            (*pInSeq->actExp)();
        }
        rkh_sm_dispatch((RKH_SM_T *)sigMon, &evtObj);
        TEST_ASSERT_EQUAL(pInSeq->nextState, getState(sigMon));
    }

    p = unitrazer_getLastOut();
    TEST_ASSERT_EQUAL(UT_PROC_SUCCESS, p->status);
}

/* ------------------------------ End of file ------------------------------ */
