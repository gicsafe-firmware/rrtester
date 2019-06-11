/**
 *  \file       ioSampler.c
 *  \brief      Implementation of IO change detector.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  CaMa  Carlos Mancón     manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <string.h>
#include "IOSampler.h"
#include "CirBuffer.h"
#include "epoch.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef struct IOSampler IOSampler;
struct IOSampler
{
    CirBuffer buffer;
    IOChg samples;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static IOSampler ioSampler;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
int
IOSampler_init(void)
{
    int result;

    /* enter_critical_section() */
    result = cirBuffer_init(&ioSampler.buffer,
                            (unsigned char *)ioSampler.samples.signalValue,
                            sizeof(IOSample), 
                            MAX_NUM_IO_SAMPLES);
    /* exit_critical_section() */
    return result;
}

int
IOSampler_put(IOSample signalValue)
{
    int result;
    IOSample sample;

    ioSampler.samples.timeStamp = epoch_get();
    sample = signalValue;

    /* enter_critical_section() */
    result = cirBuffer_put(&ioSampler.buffer, (unsigned char *)&sample);
    /* exit_critical_section() */
    return result;
}

int
IOSampler_get(IOChg *set, int nSamples)
{
    int nGetSamples;

    /* enter_critical_section() */
    set->timeStamp = ioSampler.samples.timeStamp;
    set->timeStamp -= (cirBuffer_getNumElem(&ioSampler.buffer) / 
                       IO_SAMPLING_RATE_PER_SEC);
    nGetSamples = cirBuffer_getBlock(&ioSampler.buffer,
                                     (unsigned char *)set->signalValue, 
                                     nSamples);
    /* exit_critical_section() */
    return nGetSamples;
}

int 
IOSampler_getNumSamples(void)
{
    int nSamples;

    /* enter_critical_section() */
    nSamples = cirBuffer_getNumElem(&ioSampler.buffer);
    /* exit_critical_section() */
    return nSamples;
}

/* ------------------------------ End of file ------------------------------ */
