/**
 *  \file       events.h
 *  \brief      Event data types definitions.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña  db@vortexmakes.com
 *  CaMa  Carlos Mancón manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __EVENTS_H__
#define __EVENTS_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkhevt.h"
#include "epoch.h"
#include "rkhitl.h"
#include "rkhtmr.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/**
 *  ImeiEvt process definitions
 */
#define IMEI_LENGTH         15
#define IMEI_BUF_SIZE       IMEI_LENGTH + 1

/**
 * Specifies sizeof send / receive buffers.
 */
#define SEND_BUFF_SIZE      2048
#define RECV_BUFF_SIZE      1024

/**
 *  Oper process definitions
 */
#define OPER_LENGTH         10
#define OPER_BUF_SIZE       OPER_LENGTH + 1

/**
 * Specifies max modCmd length
 */
#define MODMGR_MAX_SIZEOF_CMDSTR    70

/* ------------------------------- Data types ------------------------------ */
typedef struct ModCmd ModCmd;
struct ModCmd
{
    RKH_EVT_T evt;
    const char *fmt;            /** contains a format string of AT command */
    RKH_SMA_T * const *aoDest;   /** pointer to SMA that receive the modem */
                                /*  response */
    RKH_TNT_T waitResponseTime; /** timeout waiting for modem response */
    RKH_TNT_T interCmdTime;     /** inter-command delay */
};

typedef struct ModMgrEvt ModMgrEvt;
struct ModMgrEvt
{
    ModCmd args;
    char cmd[MODMGR_MAX_SIZEOF_CMDSTR];
    unsigned char *data;
    ruint nData;
};

typedef struct ModMgrResp ModMgrResp;
struct ModMgrResp
{
    RKH_EVT_T evt;
    RKH_SIG_T fwdEvt;
};

typedef struct SendEvt SendEvt;
struct SendEvt
{
    RKH_EVT_T evt;
    unsigned char buf[SEND_BUFF_SIZE];
    ruint size;
};

typedef struct ReceivedEvt ReceivedEvt;
struct ReceivedEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};

typedef struct LocalTimeEvt LocalTimeEvt;
struct LocalTimeEvt
{
    ModMgrResp e;
    Time time;
};

typedef struct ImeiEvt ImeiEvt;
struct ImeiEvt
{
    ModMgrResp e;
    char buf[IMEI_BUF_SIZE];
};

typedef struct OperEvt OperEvt;
struct OperEvt
{
    ModMgrResp e;
    char buf[OPER_BUF_SIZE];
};

typedef struct SigLevelEvt SigLevelEvt;
struct SigLevelEvt
{
    ModMgrResp e;
    int value;
};

typedef struct IoChgEvt IoChgEvt;
struct IoChgEvt
{
    RKH_EVT_T evt;
    ruint din;
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
