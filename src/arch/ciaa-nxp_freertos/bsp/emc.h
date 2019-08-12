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
 *  \file   emc.h
 *  \brief  Brief description and purpose of this file.
 *
 *  Long description and purpose of this file.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RiGr  Rick Grimes  rick.grimes@twd.com
 */

/* --------------------------------- Notes --------------------------------- */
/*
 *  (1) These notes could be referenced by number.
 *      (1.1) Sub-note.
 *  (2) Another note.
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __EMC_H__
#define __EMC_H__

/* ----------------------------- Include files ----------------------------- */
/* (1) #include <string.h>                                                   */
/* (2) #include <other system headers>                                       */
/* (3) #include "user header files"                                          */

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* (1) Function macro defines                                                */

/* -------------------------------- Constants ------------------------------ */
/* (1) Macro defines                                                         */
/* (2) Enumerations                                                          */
/* (3) Extern declaration of const objects defined in other files            */
/*
 * SDRAM Base Address
 */
#define CIAA_EMC_LPC43XX_SDRAM_BASE     0x28000000
/*
 * SDRAM Size (8 MBytes)
 */
#define CIAA_EMC_LPC43XX_SDRAM_SIZE     8 * (1024 * 1024)

/* ------------------------------- Data types ------------------------------ */
/* (1) typedefs                                                              */

/* -------------------------- External variables --------------------------- */
/* (1) Extern declarations of variables defined in other files               */

/* -------------------------- Function prototypes -------------------------- */
void emc_pinInit(void);
void emc_dramInit(void);
bool emc_testRAM(uint32_t baseAddress, uint32_t size);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
