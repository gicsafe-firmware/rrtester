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
 *  \file       common.c
 *  \ingroup    test_sm
 *  \brief      Common functions for unit test of state machine module.
 *
 *  \addtogroup test
 *  @{
 *  \addtogroup test_sm State Machine
 *  @{
 *  \brief      Common functions for unit test of state machine module.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2016.12.15  LeFr  v2.4.05  ---
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "unitrazer.h"
#include "rkh.h"
#include "common.h"
#include "SigMon.h"
#include <stdarg.h>

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
static SMFilterCfg smFilterCfg;

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static int
executeExpectOnList(const RKH_ST_T **stateList, int kindOfExpect)
{
    const RKH_ST_T **state;
    int n;

    for (n = 0, state = stateList; *state; ++state, ++n)
    {
        switch (kindOfExpect)
        {
            case EXPECT_TS_STATE:
                sm_tsState_expect(RKH_STATE_CAST(*state));
                break;
            case EXPECT_EXSTATE:
                sm_exstate_expect(RKH_STATE_CAST(*state));
                break;
            case EXPECT_ENSTATE:
                sm_enstate_expect(RKH_STATE_CAST(*state));
                break;
            default:
                return 0;
        }
    }
    return n;
}

/* ---------------------------- Global functions --------------------------- */
const RKH_ST_T *
getHistory(const RKH_SHIST_T *history)
{
    return *(history->target);
}

void
setHistory(const RKH_SHIST_T *history, const RKH_ST_T *state)
{
    *(history->target) = state;
}

void
setState(RKH_SMA_T *const me, const RKH_ST_T *state)
{
    ((RKH_SM_T *)me)->state = state;
}

void
setProfile(RKH_SMA_T *const me, 
           const RKH_ST_T *dftSt,
           const RKH_ST_T *currentState,
           const RKH_ST_T *sourceState, 
           const RKH_ST_T **targetStates,
           const RKH_ST_T **entryStates, 
           const RKH_ST_T **exitStates,
           const RKH_ST_T *mainTargetState, 
           int nExecEffectActions,
           int kindOfTrn, 
           int initStateMachine, 
           const RKH_EVT_T *event,
           const RKH_ST_T *dispatchCurrentState)
{
    int nExitStates, nEntryStates;

    if (initStateMachine)
    {
        expectedInitSm(me, RKH_STATE_CAST(dftSt));
    }
    if (smFilterCfg.dch == true) 
    {
        sm_dch_expect(event->e, RKH_STATE_CAST(dispatchCurrentState));
    }
    sm_trn_expect(RKH_STATE_CAST(sourceState), RKH_STATE_CAST(*targetStates));

    if (kindOfTrn == TRN_NOT_INTERNAL)
    {
        executeExpectOnList(targetStates, EXPECT_TS_STATE);
        nExitStates = executeExpectOnList(exitStates, EXPECT_EXSTATE);
    }

    if (smFilterCfg.nTrnAct == true) 
    {
        sm_ntrnact_expect(nExecEffectActions, 1);
    }

    if (kindOfTrn == TRN_NOT_INTERNAL)
    {
        nEntryStates = executeExpectOnList(entryStates, EXPECT_ENSTATE);
        if (smFilterCfg.nEnEx == true) 
        {
            sm_nenex_expect(nEntryStates, nExitStates);
        }
        sm_state_expect(RKH_STATE_CAST(mainTargetState));
    }
    sm_evtProc_expect();

    if (initStateMachine)
    {
        rkh_sm_init((RKH_SM_T *)me);
    }
    if (currentState)
    {
        setState(me, RKH_STATE_CAST(currentState));
    }
}

void
trnStepExpect(RKH_SM_T *const me, const RKH_ST_T *currentState,
              const RKH_ST_T *sourceState, TargetEntrySt *tgEnSt,
              const RKH_ST_T **exitStates,
              const RKH_EVT_T *event)
{
    TargetEntrySt *st;
    int nEnSt, i;

    /* Init state machine */
    if (CB(RKH_SMA_ACCESS_CONST(me, istate))->type == RKH_BASIC)
    {
        expectedInitSm((RKH_SMA_T *)me, RKH_STATE_CAST(currentState));
    }

    if (sourceState != (const RKH_ST_T *)0)
    {
        /* Start transition */
        if (smFilterCfg.dch == true) 
        {
            sm_dch_expect(event->e, RKH_STATE_CAST(currentState));
        }
        sm_trn_expect(RKH_STATE_CAST(sourceState),
                      RKH_STATE_CAST(tgEnSt[0].tgSt));

        /* First step */
        sm_tsState_expect(RKH_STATE_CAST(tgEnSt[0].tgSt));
        sm_exstate_expect(RKH_STATE_CAST(exitStates[0]));
        sm_enstate_expect(RKH_STATE_CAST(tgEnSt[0].enSt));
        nEnSt = 1;

        /* Micro step */
        for (i = 0, st = &tgEnSt[1]; st->tgSt || st->enSt; ++st, ++i)
        {
            if (st->tgSt != 0)
            {
                sm_tsState_expect(RKH_STATE_CAST(st->tgSt));
            }
            if (st->enSt != 0)
            {
                sm_enstate_expect(RKH_STATE_CAST(st->enSt));
                ++nEnSt;
            }
        }

        /* End transition */
        if (smFilterCfg.nEnEx == true) 
        {
            sm_nenex_expect(nEnSt, 1);
        }
        sm_state_expect(RKH_STATE_CAST(tgEnSt[i].enSt));
        sm_evtProc_expect();
    }
}

const RKH_ST_T *
getState(RKH_SMA_T *const me)
{
    return ((RKH_SM_T *)me)->state;
}

void
stateList_create(const RKH_ST_T **list, int nElems, ...)
{
    int i;
    va_list args;
    RKH_ST_T *state;

    va_start(args, nElems);
    for (i = 0; i < nElems; i++)
    {
        state = va_arg(args, RKH_ST_T *);
        *list++ = state;
    }
    va_end(args);
    *list = (RKH_ST_T *)0;
}

void
expectedInitSm(RKH_SMA_T *const me, const RKH_ST_T *dftSt)
{
    if (CB(RKH_SMA_ACCESS_CONST(me, istate))->type == RKH_BASIC)
    {
        if (smFilterCfg.init == true) 
        {
            sm_init_expect(RKH_STATE_CAST(dftSt));
        }
        sm_trn_expect(RKH_STATE_CAST(dftSt), RKH_STATE_CAST(dftSt));
        sm_tsState_expect(RKH_STATE_CAST(dftSt));
        sm_enstate_expect(RKH_STATE_CAST(dftSt));
        if (smFilterCfg.nEnEx == true) 
        {
            sm_nenex_expect(1, 0);
        }
        sm_state_expect(RKH_STATE_CAST(dftSt));
        sm_evtProc_expect();
    }
}

void 
SMFilterCfg_set(SMFilterCfg *me)
{
    if (me != (SMFilterCfg *)0)
    {
        smFilterCfg = *me;
    }
}

SMFilterCfg *
SMFilterCfg_init(void)
{
    RKH_FILTER_OFF_ALL_SIGNALS();
    RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_SM);
    RKH_FILTER_OFF_SMA(sigMon);
    RKH_FILTER_OFF_EVENT(RKH_TE_FWK_ASSERT);

    smFilterCfg.init = true;
    smFilterCfg.nTrnAct = false;
    smFilterCfg.dch = true;
    smFilterCfg.nEnEx= true;

    return &smFilterCfg;
}

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/* ------------------------------ End of file ------------------------------ */
