/**
 *  \file       eth.h
 *  \brief
 */

/* -------------------------- Development history -------------------------- */
/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __ETH_H__
#define __ETH_H__

/* ----------------------------- Include files ----------------------------- */
#include "events.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void eth_init(void);
void eth_deinit(void);
void eth_socketOpen(char *ip, char *port);
void eth_socketConnected(void);
void eth_socketDisconnected(void);
void eth_socketWrite(rui8_t *p, ruint size);
ruint eth_socketRead(rui8_t *p, ruint size);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
