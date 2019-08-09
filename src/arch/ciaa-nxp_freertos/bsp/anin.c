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
 *  AlPe  Alejandro Permingeat   apermingeat@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi.h"
#include "anin.h"
#include "mTimeCfg.h"
#include "emaFilter.h"
#include "anSampler.h"
#include "sapi_adc128d818.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ANINS_EMA_ALPHA     2

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ADCSampleUnit anIns[NUM_ANIN_SIGNALS];
static int currChannel;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static uint16_t
anIn_adcRead(int channel)
{
	/** According to schematic "Probador de rele final 291218 vs2 sch.pdf"
	 *  VOLT_RELE is mapped to AN13 (AN5 of U12)
	 *  CORR_RELE is mapped to AN14 (AN6 of U12)
	 *  */
	uint16_t analogValue = 0;
	uint8_t adc129d818_i2cAdd;
	uint8_t adc129d818_channel;

	if (NUM_ANIN_SIGNALS > channel)
	{
		switch (channel)
		{
			case	AnIn0: /**current*/
				adc129d818_i2cAdd = ADC128D818_ADDRESS_LOW_LOW;
				adc129d818_channel = 6;
				break;
			case	AnIn1: /**current*/
				adc129d818_i2cAdd = ADC128D818_ADDRESS_LOW_LOW;
				adc129d818_channel = 5;
				break;
		}
		analogValue = ADC128D818_readChannel(adc129d818_i2cAdd, adc129d818_channel);
	}

    return analogValue;
}

static bool_t
anIn_init_ADC128D818(void)
{
	bool_t result;

	result = ADC128D818_init(ADC128D818_ADDRESS_LOW_LOW,
                ADC128D818_OPERATION_MODE_1,
				ADC128D818_RATE_CONTINUOUS,
                ADC128D818_VREF_INT,
                0,
				ADC128D818_ENABLE_ALL);
	if (result)
	{
 	 result = ADC128D818_init(ADC128D818_ADDRESS_HIGH_HIGH,
                ADC128D818_OPERATION_MODE_1,
				ADC128D818_RATE_CONTINUOUS,
                ADC128D818_VREF_INT,
                0,
				ADC128D818_ENABLE_ALL);
	 }
	return(result);

 }

/* ---------------------------- Global functions --------------------------- */
void
anIn_init(void)
{
	anIn_init_ADC128D818();
    currChannel = AnIn0;
}

void
anIn_captureAndFilter(void)
{
    uint16_t value;

    value = anIn_adcRead(currChannel);
    anIns[currChannel] = emaFilter_LowPass(value,
                                           anIns[currChannel],
                                           ANINS_EMA_ALPHA);

    if (++currChannel >= NUM_ANIN_SIGNALS)
    {
        currChannel = AnIn0;
    }
}

ADCSampleUnit
anIn_get(AnInSignalId channel)
{
    if (channel > NUM_ANIN_SIGNALS)
    {
        return 0;
    }

    return anIns[channel];
}


/* ------------------------------ End of file ------------------------------ */
