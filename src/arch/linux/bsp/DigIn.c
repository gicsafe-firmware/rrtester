/*
 *  --------------------------------------------------------------------------
 *
 *                               GICSAFe-Firmware
 *                               ----------------
 *
 *                      Copyright (C) 2019 CONICET-GICSAFe
 *          All rights reserved. Protected by international copyright laws.
 *
 *  Contact information:
 *  site: https://github.com/gicsafe-firmware
 *  e-mail: <someone>@<somewhere>
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file   DigIn.c
 *  \brief  Digital Input module implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  CaMa  Carlos Mancón     manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/*
 * SIGMON_CLOCK_PER		Holds the fastest signal period
 *
 * SIGMON_DIGIN_TICKS	Holds the amount of samples taken from the signals.
 *
 * The correct sequence has the following values: 4>0>6>2>4>0>4>0>4>0>5>1
 */

/* ----------------------------- Include files ----------------------------- */
#include "DigIn.h"
#include "SigMonAct.h"

/* ----------------------------- Local macros ------------------------------ */
#define EXTRACT_BIT_FROM_SEQ(x) (sequence[currState] >> (x)) & 1
/* ------------------------------- Constants ------------------------------- */
#define DIGIN_STATES_QTY    12
#define CLK_BIT_POS         2
#define CLK3_BIT_POS        1
#define CLK6_BIT_POS        0
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static rui16_t callCount = 0;
static rui8_t currState = 0;
rui8_t sequence[DIGIN_STATES_QTY] = {4, 0, 6, 2, 4, 0, 4, 0, 4, 0, 5, 1};
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
DigIn
DigIn_get(void)
{
    struct DigIn ret;

    ret.clk = EXTRACT_BIT_FROM_SEQ(CLK_BIT_POS);
    ret.clkX3 = EXTRACT_BIT_FROM_SEQ(CLK3_BIT_POS);
    ret.clkX6 = EXTRACT_BIT_FROM_SEQ(CLK6_BIT_POS);
    ret.failure = 0;

    /* Increment call counter and determine in which half of the square wave
     * are we. Each new half is a new state of the sequence */
    currState = (++callCount) / (SIGMON_DIGIN_TICKS / 2);

    /* If all the states have passed, let's go back to the beginning */
    currState = currState % DIGIN_STATES_QTY;

    /* To avoid overflows, let's keep the variable <= number of total samples.
     * (the periodic sample ticks are distributed into 2 sequence states) */
    callCount = callCount % (SIGMON_DIGIN_TICKS * DIGIN_STATES_QTY / 2);

    return ret;
}
/* ------------------------------ End of file ------------------------------ */
