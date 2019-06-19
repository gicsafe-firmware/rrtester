/**
 *  \file       adconv.c
 *  \brief      Implementation of ADC abstraction.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  CaMa  Carlos Mancón     manconci@gmail.com
 *  DaBa  Dario Baluña      db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "adconv.h"
#include "anSampler.h"
#include "anin.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef struct ADConvFeature ADConvFeature;
struct ADConvFeature
{
    int fullScale;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ADConvFeature feature[] = 
{
    ADCONV_FULL_SCALE_ANI0, 
    ADCONV_FULL_SCALE_ANI1
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static SampleValue 
convertToSampleValue(AnInSignalId channel, SampleValue sample)
{
    double value;
    int whole, fraction, result;

    value = ((double)(sample * feature[channel].fullScale) / 
             (1 << ADCONV_RESOLUTION));
    whole = (int)value;
    fraction = (int)(value * ADCONV_DIG_FRACTION);
    fraction = fraction - (whole * ADCONV_DIG_FRACTION);

    result = fraction;
    result |= whole << 8;
    return (SampleValue)result;
}

/* ---------------------------- Global functions --------------------------- */
SampleValue
ADConv_getSample(AnInSignalId channel)
{
    SampleValue sample;

    sample = anIn_get(channel);
    return convertToSampleValue(channel, sample);
}

/* ------------------------------ End of file ------------------------------ */
