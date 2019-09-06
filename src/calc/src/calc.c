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
 *  \file   calc.c
 *  \brief  Implementation of a generic calculus module.
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

/* ----------------------------- Include files ----------------------------- */
#include "stdbool.h"
#include "calc.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static bool haveTable = false;
static uint32_t table[256];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
uint32_t
rc_crc32(uint32_t crc, char *buf, size_t len)
{
    /* From rosettacode.org */
    uint32_t rem;
    uint8_t octet;
    int i, j;
    const char *p, *q;

    if (!haveTable)
    {
        /* Calculate CRC table. */
        for (i = 0; i < 256; i++)
        {
            rem = i; /* remainder from polynomial division */
            for (j = 0; j < 8; j++)
            {
                if (rem & 1)
                {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                }
                else
                {
                    rem >>= 1;
                }
            }
            table[i] = rem;
        }
        haveTable = true;
    }

    crc = ~crc;
    q = buf + len;
    for (p = buf; p < q; p++)
    {
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}

/* ------------------------------ End of file ------------------------------ */
