/**
 *  \file       mTimeTble.c
 * 	\bried      mTime timers Table.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "mTime.h"
#include "mTimeCfg.h"

#include "din.h"
#include "anin.h"
#include "epoch.h"
#include "modpwr.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static void(* const actions_100[])( void ) =
{
#ifdef MODPWR_CTRL_ENABLE
	modPwr_ctrl, 
#endif
    dIn_scan, epoch_updateByStep,
    NULL
};

static void(* const actions_1000[])( void ) =
{
	anIn_captureAndFilter, 
    NULL
};

static void(* const actions_10000[])( void ) =
{
	anIn_update, 
    NULL
};

const timerChain_t timerChain[] =
{
	{ 100/MTIME_TIME_TICK, 	actions_100	},
	{ 1000/MTIME_TIME_TICK, actions_1000 },
	{ 10000/MTIME_TIME_TICK, actions_10000 }
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */





