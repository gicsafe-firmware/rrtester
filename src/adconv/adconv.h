/**
 *  \file       adconv.h
 *  \brief      Specification of ADC abstraction.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __ADCONV_H__
#define __ADCONV_H__

/* ----------------------------- Include files ----------------------------- */
#include "anSampler.h"
#include "anin.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/** According to schematic "Probador de rele final 291218 v2   sch.pdf" the
 * volt relay measure is performed over the point between R30 and R37
 * (resistive division). R30 is connected to pin 2 of J12 (max value 12 Volts)
 * and R37 is connected to GND.
 * Due to R30 is 6k8, R37 is 1.2k and  the maximum value is 12 Volts,
 * the Volt Relay measure point will have a range between 0 and 1.8 Volts.
 * This means that when 1.8 V is measured, the real voltage is 12 Volts.
 * In other hands, the ADC converter (ADC128D818) is a 12 bits converter and
 * has an internal Vref of 2.56Volts. So the full scale of the ADC converter
 * is 2.56V.
 * From the last two points, if in the measure point is read a 1.8 Volts means
 * the real voltage is 12 Volts, when in the same point is read 2.56V
 * (adc full scale) means the real voltage is 17,07 Volts.
 * For that reason, the ADC full scale of the voltage reading is set to
 * 17 Volts.
 * Related to current measure, the value is read between Q3 and R36.
 * R36 is a 3.9 resistance and is connected to GND.
 * The ADC converter has a Vref of 2.56 V.
 * When a voltage value of 2.56 is read, that means that 656,4 mA are going
 * through the R36 resistance (2,56 V / 3,9 Ohms).
 * For that reason, the ADC full scale of the current reading is set to
 * 656 mA.
*/
#define ADCONV_RESOLUTION       12  /* In bits */
#define ADCONV_FULL_SCALE_ANI0  656 /* In milliampers */
#define ADCONV_FULL_SCALE_ANI1  17  /* In volts */
#define ADCONV_DIG_FRACTION     100 /* Number of digits in fractional part */

/* ------------------------------- Data types ------------------------------ */

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
SampleValue ADConv_getSample(AnInSignalId channel);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
