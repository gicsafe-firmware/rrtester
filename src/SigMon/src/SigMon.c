/**
 *  \file       SigMon.c
 *  \brief      Active object implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"
#include "signals.h"
#include "SigMon.h"
#include "SigMonAct.h"

/* ----------------------------- Local macros ------------------------------ */
/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(SMInactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_BASIC_STATE(WaitSyncSeq, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq0, SigMon_enSeq0, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq2, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq3, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq4, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq5, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq11, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(OutOfSeq, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq1, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq10, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq8, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq7, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq6, NULL, NULL, &SMActive, NULL);
RKH_CREATE_BASIC_STATE(Seq9, NULL, NULL, &SMActive, NULL);

RKH_CREATE_COMP_REGION_STATE(SMActive, SigMon_enSMActive, SigMon_exSMActive, RKH_ROOT, &WaitSyncSeq, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

RKH_CREATE_TRANS_TABLE(SMInactive)
	RKH_TRREG(evEnable, NULL, NULL, &SMActive),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(SMActive)
	RKH_TRREG(evDisable, NULL, NULL, &SMInactive),
	RKH_TRREG(evFailure, NULL, SigMon_SMActiveToSigMon_FinalExt3, &SigMon_Final),
	RKH_TRINT(evSync, NULL, SigMon_SMActiveToSMActiveLoc2),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(WaitSyncSeq)
	RKH_TRREG(evIn1, NULL, NULL, &Seq0),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq0)
	RKH_TRREG(evIn4, NULL, NULL, &Seq1),
	RKH_TRREG(evIn0, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
	RKH_TRINT(evIn1, NULL, SigMon_Seq0ToSeq0Loc4),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq2)
	RKH_TRREG(evIn6, NULL, SigMon_Seq2ToSeq3Ext8, &Seq3),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn4, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRINT(evIn0, NULL, SigMon_Seq2ToSeq2Loc5),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq3)
	RKH_TRREG(evIn2, NULL, NULL, &Seq4),
	RKH_TRREG(evIn0, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn4, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq4)
	RKH_TRREG(evIn4, NULL, NULL, &Seq5),
	RKH_TRREG(evIn0, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq5)
	RKH_TRREG(evIn0, NULL, NULL, &Seq6),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn4, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq11)
	RKH_TRREG(evIn1, NULL, NULL, &Seq0),
	RKH_TRREG(evIn0, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn4, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(OutOfSeq)
	RKH_TRREG(evIn1, NULL, NULL, &Seq0),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq1)
	RKH_TRREG(evIn0, NULL, NULL, &Seq2),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
	RKH_TRINT(evIn4, NULL, SigMon_Seq1ToSeq1Loc6),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq10)
	RKH_TRREG(evIn5, NULL, NULL, &Seq11),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn4, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq8)
	RKH_TRREG(evIn4, NULL, NULL, &Seq9),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq7)
	RKH_TRREG(evIn0, NULL, NULL, &Seq8),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq6)
	RKH_TRREG(evIn4, NULL, NULL, &Seq7),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(Seq9)
	RKH_TRREG(evIn0, NULL, NULL, &Seq10),
	RKH_TRREG(evIn1, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn2, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn5, NULL, NULL, &OutOfSeq),
	RKH_TRREG(evIn6, NULL, NULL, &OutOfSeq),
RKH_END_TRANS_TABLE


RKH_CREATE_FINAL_STATE(SigMon_Final, RKH_NULL);

/* ............................. Active object ............................. */
RKH_SMA_CREATE(SigMon, sigMon, 0, HCAL, &SMInactive, SigMon_ToSMInactiveExt0, NULL);
RKH_SMA_DEF_PTR(sigMon);

/* ------------------------------- Constants ------------------------------- */

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
