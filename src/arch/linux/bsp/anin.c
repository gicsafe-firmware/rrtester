/**
 *  \file       anin.c
 *  \brief      Implementation of Analog Inputs adquisition and filtering.
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
#include <string.h>
#include "rkh.h"
#include "anin.h"
#include "mTimeCfg.h"
#include "emaFilter.h"
#include "anSampler.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ANINS_EMA_ALPHA     2
#define RAND_SEED           (1234U)

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ADCSampleUnit anIns[NUM_ANIN_SIGNALS];
static ADCSampleUnit anIns_simu[NUM_ANIN_SIGNALS] = {300, 400};
static rui32_t l_rnd;  /* random seed */

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static rui32_t 
lrand( void )
{  
    /* 
     * A very cheap pseudo-random-number generator.
     * "Super-Duper" Linear Congruential Generator (LCG)
     * LCG(2^32, 3*7*11*13*23, 0, seed) [MS]
     */
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}

ADCSampleUnit
anIn_adcRead(int channel)
{   
    (void)channel;

    return anIns_simu[channel] + (lrand() % 10);
}

/* ---------------------------- Global functions --------------------------- */
void
anIn_init(void)
{
    memset(anIns, 0, sizeof(anIns));
    l_rnd = RAND_SEED;
}

void
anIn_captureAndFilter(void)
{
    unsigned char i;
    int16_t value;

    for(i=0; i < NUM_ANIN_SIGNALS; ++i)
    {
        value = anIn_adcRead(i);
        anIns[i] = emaFilter_LowPass(value, anIns[i], ANINS_EMA_ALPHA);
    }
}

ADCSampleUnit
anIn_get(AnInSignalId channel)
{
    if(channel > NUM_ANIN_SIGNALS)
        return 0;

    return anIn_adcRead(channel);
    //return anIns[channel];
}

void
anIn_update(void)
{
   anSampler_put();
}

/* ------------------------------ End of file ------------------------------ */
