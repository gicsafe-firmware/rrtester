/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONMGRETH_H__
#define __CONMGRETH_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(conMgrEth);

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE inactive, unplugged, idle, receiving, sending, waitServer, waitIp;
RKH_DCLR_COMP_STATE active, plugged, connecting, connected;

/* ------------------------------- Data types ------------------------------ */
/* ............................. Active object ............................. */
typedef struct ConMgrEth ConMgrEth;
struct ConMgrEth
{
    RKH_SMA_T sma;      /* base structure */
    char * ip;
    char * port;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
