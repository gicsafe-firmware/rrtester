/**
 *  \file       bsp.h
 *  \brief      BSP for 80x86 OS win32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.23  DaBa  v0.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __BSP_H__
#define __BSP_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhtrc.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
//#define USE_GSM	/* Comment to use Ethernet */
#ifndef USE_GSM
#define USE_ETH
#endif
/*
 * Serial Ports channels identifiers
 */
enum
{
    GSM_PORT,

    NUM_CHANNELS
};

/*
 * User trace events id's
 */
enum
{
    MODCMD_USR_TRACE = RKH_TE_USER,
	ETH_USR_TRACE
};

/*
 * Status Led's
 */
typedef enum
{
    DisconnectedSt,
    UnregisteredSt = DisconnectedSt,

    ConnectedSt,
    RegisteredSt = ConnectedSt,
} Status_t;

/*
 * Network types
 */
typedef enum
{
    GSMNetwork,
    ETHNetwork
} NetType_t;

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void bsp_init(int argc, char *argv[]);

void bsp_keyParser(int c);
void bsp_timeTick(void);

void bsp_eth_open(void);

void bsp_serial_open(int ch);
void bsp_serial_close(int ch);
void bsp_serial_puts(int ch, char *p);
void bsp_serial_putnchar(int ch, unsigned char *p, ruint ndata);

void bsp_linkStatus(NetType_t t, Status_t status);
void bsp_socketStatus(NetType_t t, Status_t status);
void bsp_modStatusToggle();

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */
