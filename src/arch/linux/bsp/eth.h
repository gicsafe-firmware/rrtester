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
 *  \file   eth.h
 *  \brief  Specifies this module.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  CaMa  Carlos Mancón  manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __ETH_H__
#define __ETH_H__

/* ----------------------------- Include files ----------------------------- */
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <fcntl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <limits.h>
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define ETH_THREAD_STACK_SIZE PTHREAD_STACK_MIN
/* ------------------------------- Data types ------------------------------ */
typedef int SOCKET;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
