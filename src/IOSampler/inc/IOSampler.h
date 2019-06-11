/**
 *  \file       IOSampler.h
 *  \brief      Specification of I/O sampler
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.14  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __IOSAMPLER_H__
#define __IOSAMPLER_H__

/* ----------------------------- Include files ----------------------------- */
#include "epoch.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define MAX_NUM_IO_SAMPLES          256
#define IO_SAMPLING_RATE_PER_SEC    2   /* # of samples per second */

/* ------------------------------- Data types ------------------------------ */
typedef unsigned short IOSample;
typedef struct IOChg IOChg;
struct IOChg
{
    Epoch timeStamp;
    IOSample signalValue[MAX_NUM_IO_SAMPLES];
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
int IOSampler_init(void);
int IOSampler_put(IOSample signalValue);
int IOSampler_get(IOChg *set, int nSamples);
int IOSampler_getNumSamples(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
