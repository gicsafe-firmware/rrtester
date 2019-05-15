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
#include "signal.h"
#include "SigMon.h"
#include "SigMonAct.h"
#include "bsp.h"
#include "SigMonActRequired.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ............................ Effect actions ............................. */
void 
SigMon_SMActiveToSigMon_FinalExt3(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

void 
SigMon_Seq2ToSeq3Ext8(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

void 
SigMon_SMActiveToSMActiveLoc2(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

void 
SigMon_Seq0ToSeq0Loc4(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

void 
SigMon_Seq2ToSeq2Loc5(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

void 
SigMon_Seq1ToSeq1Loc6(SigMon *const me, RKH_EVT_T *pe)
{
	me->digIn = 0;
}

/* ............................. Entry actions ............................. */
void 
SigMon_enSMActive(SigMon *const me)
{
	me->digIn = 0;
}

void 
SigMon_enSeq0(SigMon *const me)
{
	me->digIn = 0;
}

/* ............................. Exit actions .............................. */
void 
SigMon_exSMActive(SigMon *const me)
{
	me->digIn = 0;
}

/* ................................ Guards ................................. */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
