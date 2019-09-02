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
 *  \file   uniqueID.c
 *  \brief  Implementation of BSP's unique device identifier.
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
 *  (1) See header file notes.
 */

/* ----------------------------- Include files ----------------------------- */
#include "uniqueID.h"
#include "chip.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
uint32_t
readUID(uint32_t uid[])
{
    /* Chip_IAP_ReadUID() implementation from LPCOpen 3.02
     * current version (v2.16) has a known bug
     * */
    uint32_t command[5], result[5];
    uint32_t i;

    command[0] = IAP_READ_UID_CMD;
    iap_entry(command, result);

    for (i=0; i < 4; i++)
        uid[i] = result[i + 1];

    return result[0];
}
/* ------------------------------ End of file ------------------------------ */
