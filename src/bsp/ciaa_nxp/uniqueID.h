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
 *  \file   uniqueID.h
 *  \brief  Specification of BSP's unique device identifier.
 *
 *  Long description and purpose of this file.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  CaMa  Carlos Mancón  manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/*
 *  (1) This is the Chip_IAP_ReadUID() implementation from LPCOpen 3.02.
 * The current version (v2.16) has a known bug.
 *
 *  (2) If LPCOpen is updated, this module can be eliminated and proceed to use
 * the updated library's function.
 *
 */
/* --------------------------------- Module -------------------------------- */
#ifndef __UNIQUEID_H__
#define __UNIQUEID_H__

/* ----------------------------- Include files ----------------------------- */
#include <stdint.h>

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */

/* -------------------------- Function prototypes -------------------------- */
uint32_t readUID(uint32_t uid[]);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
