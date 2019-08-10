/**
 *  \file       ain.h
 *  \brief      Specification of Analog Inputs adquisition and filtering.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 *  AlPe  Alejandro Permingeat apermingeat@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __AIN_H__
#define __AIN_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef enum AnInSignalId AnInSignalId;
enum AnInSignalId
{
    AnIn0,  /* current */
    AnIn1,  /* voltage */
    NUM_ANIN_SIGNALS
};

typedef unsigned short ADCSampleUnit;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void anIn_init(void);
void anIn_captureAndFilter(void);
ADCSampleUnit anIn_get(AnInSignalId channel);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
