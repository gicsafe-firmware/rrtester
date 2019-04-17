/**
 *  \file       ConMgrGsm.c
 *  \brief      Implementation of GSM connection and protocol manager.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña      db@vortexmakes.com
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include <string.h>
#include "ConMgrGsm.h"
#include "modpwr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "mqttProt.h"
#include "config.h"
#include "signals.h"
#include "events.h"
#include "topics.h"
#include "rtime.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
#define SIZEOF_QDEFER   1

/* ......................... Declares active object ........................ */
typedef struct ConMgrGsm ConMgrGsm;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE ConMgrGsm_inactive, ConMgrGsm_sync,
                ConMgrGsm_init, ConMgrGsm_pin, ConMgrGsm_setPin, ConMgrGsm_enableNetTime,
                ConMgrGsm_getImei, ConMgrGsm_cipShutdown, ConMgrGsm_setManualGet,
                ConMgrGsm_waitReg, ConMgrGsm_unregistered, ConMgrGsm_failure,
                ConMgrGsm_waitNetClockSync, ConMgrGsm_localTime, ConMgrGsm_getOper,
                ConMgrGsm_setAPN, ConMgrGsm_enableGPRS,
                ConMgrGsm_checkIP, ConMgrGsm_waitRetryConfig, ConMgrGsm_waitingServer,
                ConMgrGsm_idle, ConMgrGsm_getStatus, ConMgrGsm_waitPrompt, ConMgrGsm_waitOk,
                ConMgrGsm_receiving, ConMgrGsm_restarting, ConMgrGsm_wReopen,
                ConMgrGsm_waitRetryConnect, ConMgrGsm_disconnecting;

RKH_DCLR_COMP_STATE ConMgrGsm_active, ConMgrGsm_initialize, ConMgrGsm_registered,
                    ConMgrGsm_configure, ConMgrGsm_connecting, ConMgrGsm_connected,
                    ConMgrGsm_sending;
RKH_DCLR_FINAL_STATE ConMgrGsm_activeFinal, ConMgrGsm_initializeFinal, 
                     ConMgrGsm_configureFinal, ConMgrGsm_sendingFinal;
RKH_DCLR_COND_STATE ConMgrGsm_checkSyncTry, ConMgrGsm_checkConfigTry,
                    ConMgrGsm_checkConnectTry;

/* ........................ Declares initial action ........................ */
static void init(ConMgrGsm *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void open(ConMgrGsm *const me, RKH_EVT_T *pe);
static void close(ConMgrGsm *const me, RKH_EVT_T *pe);
static void defer(ConMgrGsm *const me, RKH_EVT_T *pe);
static void setSigLevel(ConMgrGsm *const me, RKH_EVT_T *pe);
static void initializeInit(ConMgrGsm *const me, RKH_EVT_T *pe);
static void storeImei(ConMgrGsm *const me, RKH_EVT_T *pe);
static void storeOper(ConMgrGsm *const me, RKH_EVT_T *pe);
static void checkRegStatus(ConMgrGsm *const me, RKH_EVT_T *pe);
static void startRegStatus(ConMgrGsm *const me, RKH_EVT_T *pe);
static void localTimeGet(ConMgrGsm *const me, RKH_EVT_T *pe);
static void rtimeSync(ConMgrGsm *const me, RKH_EVT_T *pe);
static void configureInit(ConMgrGsm *const me, RKH_EVT_T *pe);
static void configTry(ConMgrGsm *const me, RKH_EVT_T *pe);
static void requestIp(ConMgrGsm *const me, RKH_EVT_T *pe);
static void connectInit(ConMgrGsm *const me, RKH_EVT_T *pe);
static void connectTry(ConMgrGsm *const me, RKH_EVT_T *pe);
static void socketOpen(ConMgrGsm *const me, RKH_EVT_T *pe);
static void socketClose(ConMgrGsm *const me, RKH_EVT_T *pe);
static void readData(ConMgrGsm *const me, RKH_EVT_T *pe);
static void sendRequest(ConMgrGsm *const me, RKH_EVT_T *pe);
static void flushData(ConMgrGsm *const me, RKH_EVT_T *pe);
static void sendOk(ConMgrGsm *const me, RKH_EVT_T *pe);
static void recvOk(ConMgrGsm *const me, RKH_EVT_T *pe);
static void sendFail(ConMgrGsm *const me, RKH_EVT_T *pe);
static void recvFail(ConMgrGsm *const me, RKH_EVT_T *pe);
static void tryGetStatus(ConMgrGsm *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void sendSync(ConMgrGsm *const me);
static void sendInit(ConMgrGsm *const me);
static void checkPin(ConMgrGsm *const me);
static void setPin(ConMgrGsm *const me);
static void netTimeEnable(ConMgrGsm *const me);
static void getImei(ConMgrGsm *const me);
static void cipShutdown(ConMgrGsm *const me);
static void unregEntry(ConMgrGsm *const me);
static void regEntry(ConMgrGsm *const me);
static void failureEntry(ConMgrGsm *const me);
static void setupManualGet(ConMgrGsm *const me);
static void waitNetClockSyncEntry(ConMgrGsm *const me);
static void waitRetryConfigEntry(ConMgrGsm *const me);
static void getOper(ConMgrGsm *const me);
static void setupAPN(ConMgrGsm *const me);
static void startGPRS(ConMgrGsm *const me);
static void wReopenEntry(ConMgrGsm *const me);
static void waitRetryConnEntry(ConMgrGsm *const me);
static void getConnStatus(ConMgrGsm *const me);
static void isConnected(ConMgrGsm *const me);
static void connectingEntry(ConMgrGsm *const me);
static void socketConnected(ConMgrGsm *const me);
static void idleEntry(ConMgrGsm *const me);

/* ......................... Declares exit actions ......................... */
static void unregExit(ConMgrGsm *const me);
static void regExit(ConMgrGsm *const me);
static void waitNetClockSyncExit(ConMgrGsm *const me);
static void wReopenExit(ConMgrGsm *const me);
static void waitRetryConnExit(ConMgrGsm *const me);
static void failureExit(ConMgrGsm *const me);
static void connectingExit(ConMgrGsm *const me);
static void socketDisconnected(ConMgrGsm *const me);
static void idleExit(ConMgrGsm *const me);
static void getStatusExit(ConMgrGsm *const me);

/* ............................ Declares guards ............................ */
rbool_t checkSyncTry(ConMgrGsm *const me, RKH_EVT_T *pe);
rbool_t checkConfigTry(ConMgrGsm *const me, RKH_EVT_T *pe);
rbool_t checkConnectTry(ConMgrGsm *const me, RKH_EVT_T *pe);
rbool_t checkConnectedFailCounter(ConMgrGsm *const me, RKH_EVT_T *pe);


/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(ConMgrGsm_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_inactive)
    RKH_TRINT(evSend, NULL, sendFail),
    RKH_TRINT(evRecv, NULL, recvFail),
    RKH_TRREG(evOpen, NULL, open, &ConMgrGsm_active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_active, NULL, NULL, RKH_ROOT, 
                             &ConMgrGsm_initialize, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_active)
    RKH_TRINT(evSigLevel, NULL, setSigLevel),
    RKH_TRINT(evSend, NULL, sendFail),
    RKH_TRINT(evRecv, NULL, recvFail),
    RKH_TRCOMPLETION(NULL, NULL, &ConMgrGsm_inactive),
    RKH_TRREG(evClose, NULL, close, &ConMgrGsm_inactive),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_initialize, NULL, NULL, &ConMgrGsm_active, 
                             &ConMgrGsm_sync, initializeInit,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_initialize)
    RKH_TRCOMPLETION(NULL, NULL, &ConMgrGsm_unregistered),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgrGsm_failure),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_sync, sendSync, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_sync)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_init),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgrGsm_checkSyncTry),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(ConMgrGsm_checkSyncTry);
RKH_CREATE_BRANCH_TABLE(ConMgrGsm_checkSyncTry)
    RKH_BRANCH(checkSyncTry,   NULL,   &ConMgrGsm_sync),
    RKH_BRANCH(ELSE,           NULL,   &ConMgrGsm_failure),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_init, sendInit, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_init)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_pin),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_pin, checkPin, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_pin)
    RKH_TRREG(evSimPin,     NULL, NULL, &ConMgrGsm_setPin),
    RKH_TRREG(evSimError,   NULL, NULL, &ConMgrGsm_failure),
    RKH_TRREG(evSimReady,   NULL, NULL, &ConMgrGsm_enableNetTime),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_setPin, setPin, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_setPin)
    RKH_TRREG(evOk,         NULL, NULL,   &ConMgrGsm_pin),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_enableNetTime, netTimeEnable, NULL, 
                                                    &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_enableNetTime)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_getImei),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_getImei, getImei, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_getImei)
    RKH_TRREG(evImei,       NULL, storeImei, &ConMgrGsm_cipShutdown),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_cipShutdown, cipShutdown, NULL, &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_cipShutdown)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_setManualGet),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_setManualGet, setupManualGet, NULL, 
                                                    &ConMgrGsm_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_setManualGet)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_initializeFinal),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_registered, regEntry, regExit, &ConMgrGsm_active, 
                             &ConMgrGsm_waitNetClockSync, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_registered)
    RKH_TRREG(evNoReg, NULL, NULL,   &ConMgrGsm_unregistered),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_unregistered, unregEntry, unregExit,
                            &ConMgrGsm_active, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_unregistered)
    RKH_TRINT(evTimeout,    NULL,    checkRegStatus),
    RKH_TRINT(evNoReg,      NULL,    startRegStatus),
    RKH_TRREG(evRegTimeout,  NULL,    NULL, &ConMgrGsm_failure),
    RKH_TRREG(evReg, NULL, NULL,   &ConMgrGsm_registered),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_failure, failureEntry, failureExit, &ConMgrGsm_active, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_failure)
    RKH_TRREG(evTimeout, NULL,  NULL, &ConMgrGsm_active),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitNetClockSync, 
                            waitNetClockSyncEntry, waitNetClockSyncExit,
                            &ConMgrGsm_registered, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitNetClockSync)
    RKH_TRREG(evTimeout,       NULL, NULL, &ConMgrGsm_getOper),
    RKH_TRREG(evNetClockSync,  NULL, localTimeGet, &ConMgrGsm_localTime),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_localTime, NULL, NULL, &ConMgrGsm_registered, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_localTime)
    RKH_TRREG(evLocalTime,     NULL, rtimeSync,  &ConMgrGsm_getOper),
    RKH_TRREG(evNoResponse,    NULL, NULL,       &ConMgrGsm_getOper),
RKH_END_TRANS_TABLE

RKH_CREATE_HISTORY_STORAGE(ConMgrGsm_configure);
RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_configure, NULL, NULL, &ConMgrGsm_registered, 
                             &ConMgrGsm_getOper, configureInit,
                             RKH_SHISTORY, NULL, NULL, NULL,
                             RKH_GET_HISTORY_STORAGE(ConMgrGsm_configure));
RKH_CREATE_TRANS_TABLE(ConMgrGsm_configure)
    RKH_TRCOMPLETION(NULL, connectInit, &ConMgrGsm_connecting),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgrGsm_checkConfigTry),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_getOper, getOper, NULL, &ConMgrGsm_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_getOper)
    RKH_TRREG(evOper,          NULL, storeOper,     &ConMgrGsm_setAPN),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_setAPN, setupAPN, NULL, 
                                                    &ConMgrGsm_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_setAPN)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_enableGPRS),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_enableGPRS, startGPRS, NULL, 
                                                    &ConMgrGsm_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_enableGPRS)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgrGsm_checkIP),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_checkIP, getConnStatus, NULL, 
                                                    &ConMgrGsm_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_checkIP)
    RKH_TRREG(evIPGprsAct,  requestIp,    NULL, &ConMgrGsm_checkIP),
    RKH_TRREG(evIP,         NULL,         NULL, &ConMgrGsm_checkIP),
    RKH_TRREG(evIPInitial,  NULL,         NULL, &ConMgrGsm_checkIP),
    RKH_TRREG(evIPStart,    NULL,         NULL, &ConMgrGsm_checkIP),
    RKH_TRREG(evIPStatus,   NULL,         NULL, &ConMgrGsm_configureFinal),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(ConMgrGsm_checkConfigTry);
RKH_CREATE_BRANCH_TABLE(ConMgrGsm_checkConfigTry)
    RKH_BRANCH(checkConfigTry,   NULL, &ConMgrGsm_waitRetryConfig),
    RKH_BRANCH(ELSE,           NULL,   &ConMgrGsm_failure),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitRetryConfig, waitRetryConfigEntry, NULL,
                                                    &ConMgrGsm_registered, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitRetryConfig)
    RKH_TRREG(evTimeout,  NULL,    configTry, &ConMgrGsm_configureHist),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_connecting, NULL, NULL, 
                             &ConMgrGsm_registered, &ConMgrGsm_waitingServer,
                             socketOpen,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_connecting)
    RKH_TRREG(evClose,  NULL,  socketClose, &ConMgrGsm_disconnecting),
    RKH_TRREG(evNoResponse, NULL,  NULL, &ConMgrGsm_checkConnectTry),
    RKH_TRREG(evError,      NULL,  NULL, &ConMgrGsm_checkConnectTry),
    RKH_TRREG(evClosed,     NULL,  NULL, &ConMgrGsm_checkConnectTry),
    RKH_TRREG(evIPStatus,   NULL,  NULL, &ConMgrGsm_checkConnectTry),
    RKH_TRREG(evIPInitial,  NULL,  NULL, &ConMgrGsm_configure),
    RKH_TRREG(evIPStart,    NULL,  NULL, &ConMgrGsm_configure),
    RKH_TRREG(evIPGprsAct,  NULL,  NULL, &ConMgrGsm_configure),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitingServer, connectingEntry, connectingExit,
                                                    &ConMgrGsm_connecting, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitingServer)
    RKH_TRREG(evTimeout,    NULL,  getConnStatus, &ConMgrGsm_waitingServer),
    RKH_TRREG(evConnected,  NULL,  NULL, &ConMgrGsm_connected),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_connected, 
                             socketConnected, socketDisconnected, 
                             &ConMgrGsm_connecting, &ConMgrGsm_idle, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_connected)
    RKH_TRREG(evClosed,       NULL,  NULL,        &ConMgrGsm_connecting),
    RKH_TRREG(evRestart,      NULL,  socketClose, &ConMgrGsm_restarting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_idle, idleEntry, idleExit,
                                                &ConMgrGsm_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_idle)
    RKH_TRREG(evTimeout, NULL,  getConnStatus,  &ConMgrGsm_getStatus),
    RKH_TRREG(evSend,    NULL,  sendRequest,    &ConMgrGsm_sending),
    RKH_TRREG(evRecv,    NULL,  readData,       &ConMgrGsm_receiving),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_getStatus, NULL, getStatusExit,
                                                &ConMgrGsm_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_getStatus)
    RKH_TRINT(evSend,    NULL,  defer),
    RKH_TRINT(evRecv,    NULL,  defer),
    RKH_TRREG(evNoResponse, checkConnectedFailCounter, tryGetStatus, &ConMgrGsm_idle),
    RKH_TRREG(evConnected, NULL, isConnected,   &ConMgrGsm_idle),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgrGsm_sending, NULL, NULL, 
                             &ConMgrGsm_connected, &ConMgrGsm_waitPrompt, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_sending)
    RKH_TRCOMPLETION(NULL, NULL, &ConMgrGsm_idle),
	RKH_TRREG(evError, NULL, sendFail, &ConMgrGsm_idle),
    RKH_TRREG(evNoResponse, NULL, sendFail, &ConMgrGsm_idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitPrompt, NULL, NULL, &ConMgrGsm_sending, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitPrompt)
    RKH_TRREG(evOk, NULL,  flushData, &ConMgrGsm_waitOk),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitOk, NULL, NULL, &ConMgrGsm_sending, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitOk)
    RKH_TRREG(evOk, NULL,  sendOk, &ConMgrGsm_sendingFinal),    
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_receiving, NULL, NULL, &ConMgrGsm_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_receiving)
    RKH_TRREG(evOk, NULL,  recvOk, &ConMgrGsm_idle),
	RKH_TRREG(evError, NULL, recvFail, &ConMgrGsm_idle),
	RKH_TRREG(evNoResponse, NULL, recvFail, &ConMgrGsm_idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_restarting, NULL, NULL, &ConMgrGsm_connecting, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_restarting)
    RKH_TRREG(evDisconnected, NULL,  NULL, &ConMgrGsm_wReopen),
	RKH_TRREG(evNoResponse, NULL, NULL, &ConMgrGsm_wReopen),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_wReopen, wReopenEntry, wReopenExit, 
                                                        &ConMgrGsm_connecting, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_wReopen)
    RKH_TRREG(evTimeout, NULL,  NULL, &ConMgrGsm_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(ConMgrGsm_checkConnectTry);
RKH_CREATE_BRANCH_TABLE(ConMgrGsm_checkConnectTry)
    RKH_BRANCH(checkConnectTry,  NULL, &ConMgrGsm_waitRetryConnect),
    RKH_BRANCH(ELSE,             NULL, &ConMgrGsm_failure),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_waitRetryConnect,
                                        waitRetryConnEntry, waitRetryConnExit,
                                        &ConMgrGsm_connecting, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_waitRetryConnect)
    RKH_TRREG(evTimeout, NULL,  connectTry, &ConMgrGsm_connecting),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgrGsm_disconnecting, NULL, NULL,
                                                    &ConMgrGsm_registered, NULL);
RKH_CREATE_TRANS_TABLE(ConMgrGsm_disconnecting)
    RKH_TRREG(evDisconnected,  NULL,    NULL, &ConMgrGsm_activeFinal),
    RKH_TRREG(evNoResponse,    NULL,    NULL, &ConMgrGsm_activeFinal),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */
struct ConMgrGsm
{
    RKH_SMA_T ao;       /* base structure */
    RKH_TMR_T timer;    
    RKH_TMR_T timerReg;
    rui8_t retryCount; 
    SendEvt *psend;
    int sigLevel;
    char Imei[IMEI_BUF_SIZE];
    char Oper[OPER_BUF_SIZE];
};

typedef struct Apn
{
    char *addr;
    char *usr;
    char *psw;
}Apn;

typedef struct Operator
{
    char *netCode;
    Apn apn;
}Operator;

RKH_SMA_CREATE(ConMgrGsm, conMgrGsm, 1, HCAL, &ConMgrGsm_inactive, init, NULL);
RKH_SMA_DEF_PTR(conMgrGsm);

/* ------------------------------- Constants ------------------------------- */
#define IMEI_SNR_OFFSET  8

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/*
 *  Declare and allocate the 'e_tout' event.
 *  The 'e_tout' event with TIMEOUT signal never changes, so it can be
 *  statically allocated just once by means of RKH_ROM_STATIC_EVENT() macro.
 */
static RKH_STATIC_EVENT(e_tout, evToutDelay);
static RKH_STATIC_EVENT(e_regTout, evRegTimeout);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);
static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);
static RKH_ROM_STATIC_EVENT(e_Sent,     evSent);
static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);
static RKH_ROM_STATIC_EVENT(e_RecvFail, evRecvFail);
static ReceivedEvt e_Received;

static RKH_QUEUE_T qDefer;
static RKH_EVT_T *qDefer_sto[SIZEOF_QDEFER];

static Operator operTable[] =
{
    { MOVISTAR_OPERATOR, 
        { MOVISTAR_APN_ADDR, MOVISTAR_APN_USER, MOVISTAR_APN_PASS } 
    },
    { CLARO_OPERATOR, 
        { CLARO_APN_ADDR, CLARO_APN_USER, CLARO_APN_PASS } 
    },
    { PERSONAL_OPERATOR, 
        { PERSONAL_APN_ADDR, PERSONAL_APN_USER, PERSONAL_APN_PASS } 
    },
    { NULL }
};

static Apn *defaultAPN = &(operTable[0].apn);

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static Apn *
getAPNbyOper(char *oper)
{
    Operator *pOper;
    
    for( pOper = &operTable[0]; pOper != NULL; ++pOper)
    {
        if(strcmp(oper, pOper->netCode) == 0)
            return &(pOper->apn);
    }
    return defaultAPN;
}

/* ............................ Initial action ............................. */
static void
init(ConMgrGsm *const me, RKH_EVT_T *pe)
{
	(void)pe;

    ConnectionTopic_subscribe(me);

    RKH_TR_FWK_AO(me);

    RKH_TR_FWK_TIMER(&me->timer);
    RKH_TR_FWK_TIMER(&me->timerReg);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_inactive);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_active);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_initialize);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_sync);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_checkSyncTry);
	RKH_TR_FWK_STATE(me, &ConMgrGsm_init);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_pin);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_setPin);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_enableNetTime);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_getImei);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_cipShutdown);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_initializeFinal);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_registered);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_unregistered);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_failure);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitNetClockSync);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_localTime);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_getOper);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_configure);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_configureHist);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_setManualGet);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_setAPN);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_enableGPRS);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_checkIP);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_checkConfigTry);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitRetryConfig);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_configureFinal);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_connecting);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitingServer);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_connected);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_idle);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_getStatus);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_sending);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitPrompt);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitOk);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_sendingFinal);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_receiving);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_restarting);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_wReopen);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_waitRetryConnect);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_checkConnectTry);
    RKH_TR_FWK_STATE(me, &ConMgrGsm_disconnecting);
    RKH_TR_FWK_TIMER(&me->timer);
    RKH_TR_FWK_TIMER(&me->timerReg);
    RKH_TR_FWK_SIG(evOpen);
    RKH_TR_FWK_SIG(evClose);
    RKH_TR_FWK_SIG(evCmd);
	RKH_TR_FWK_SIG(evOk);
	RKH_TR_FWK_SIG(evURC);
    RKH_TR_FWK_SIG(evSimPin);
    RKH_TR_FWK_SIG(evSimError);
    RKH_TR_FWK_SIG(evSimReady);
    RKH_TR_FWK_SIG(evImei);
    RKH_TR_FWK_SIG(evReg);
    RKH_TR_FWK_SIG(evNoReg);
    RKH_TR_FWK_SIG(evOper);
    RKH_TR_FWK_SIG(evIPInitial);
    RKH_TR_FWK_SIG(evIPStart);
    RKH_TR_FWK_SIG(evIPStatus);
    RKH_TR_FWK_SIG(evIPGprsAct);
    RKH_TR_FWK_SIG(evConnecting);
    RKH_TR_FWK_SIG(evClosed);
    RKH_TR_FWK_SIG(evConnected);
    RKH_TR_FWK_SIG(evSend);
    RKH_TR_FWK_SIG(evRecv);
    RKH_TR_FWK_SIG(evSent);
    RKH_TR_FWK_SIG(evReceived);
    RKH_TR_FWK_SIG(evSendFail);
    RKH_TR_FWK_SIG(evRecvFail);
    RKH_TR_FWK_SIG(evNetConnected);
    RKH_TR_FWK_SIG(evNetDisconnected);
    RKH_TR_FWK_SIG(evDisconnected);
    RKH_TR_FWK_SIG(evTerminate);
    RKH_TR_FWK_SIG(evNetClockSync);
    RKH_TR_FWK_SIG(evLocalTime);
    RKH_TR_FWK_SIG(evRestart);
    RKH_TR_FWK_SIG(evSigLevel);
    RKH_TR_FWK_SIG(evRegTimeout);

    rkh_queue_init(&qDefer, (const void **)qDefer_sto, SIZEOF_QDEFER, 
                CV(0));

    RKH_TMR_INIT(&me->timer, &e_tout, NULL);
    RKH_TMR_INIT(&me->timerReg, &e_regTout, NULL);
    me->retryCount = 0;
}

/* ............................ Effect actions ............................. */
static void
open(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SMA_POST_FIFO(modMgr, &e_Open, conMgrGsm);

    modPwr_on();
}

static void
close(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SMA_POST_FIFO(modMgr, &e_Close, conMgrGsm);

    modPwr_off();
}

static void
defer(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;

    if( rkh_queue_is_full(&qDefer) != RKH_TRUE )
    	rkh_sma_defer(&qDefer, pe);
}

static void
setSigLevel(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    SigLevelEvt *p;
    (void)me;
    
    p = RKH_UPCAST(SigLevelEvt, pe);
    me->sigLevel = p->value;
}

static void
initializeInit(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    me->retryCount = 0;
}

static void
storeImei(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    ImeiEvt *p;

	(void)me;

    p = RKH_UPCAST(ImeiEvt, pe);
    strcpy(me->Imei, p->buf);

    config_clientId(me->Imei + IMEI_SNR_OFFSET);
    config_topic(me->Imei + IMEI_SNR_OFFSET);
}

static void
storeOper(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    OperEvt *p;

	(void)me;

    p = RKH_UPCAST(OperEvt, pe);
    strcpy(me->Oper, p->buf);
}

static void
checkRegStatus(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    ModCmd_getRegStatus();
}

static void
startRegStatus(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CHECK_REG_PERIOD);
}

static void
localTimeGet(ConMgrGsm *const me, RKH_EVT_T *pe)
{
	(void)me;
	(void)pe;

    ModCmd_getLocalTime();
}

static void 
rtimeSync(ConMgrGsm *const me, RKH_EVT_T *pe)
{
	(void)me;

    rtime_set(&(RKH_UPCAST(LocalTimeEvt, pe)->time));
}

static void
configureInit(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    me->retryCount = 0;
}

static void
configTry(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ++me->retryCount;
	ModCmd_init();
}

static void 
requestIp(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    ModCmd_requestIP();
}

static void
connectInit(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    me->retryCount = 0;
}

static void
connectTry(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ++me->retryCount;
}

static void
socketOpen(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    ModCmd_connect(CONNECTION_PROT, CONNECTION_DOMAIN, CONNECTION_PORT);
}

static void
socketClose(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    ModCmd_disconnect();
}

static void 
readData(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Received), evReceived);
    ModCmd_readData();
}

static void
sendRequest(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)me;

    me->psend = RKH_UPCAST(SendEvt, pe);

    ModCmd_sendDataRequest((rui16_t)(me->psend->size));
}

static void
flushData(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ModCmd_sendData(me->psend->buf, me->psend->size);
}

static void
sendOk(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    me->retryCount = 0;
    ConnectionTopic_publish(&e_Sent, me);
}

static void
recvOk(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    me->retryCount = 0;
    ConnectionTopic_publish(&e_Received, me);
}

static void
sendFail(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    ConnectionTopic_publish(&e_SendFail, me);
	ModCmd_init();
}

static void
recvFail(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    ConnectionTopic_publish(&e_RecvFail, me);
	ModCmd_init();
}

static void
tryGetStatus(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ++me->retryCount;
	ModCmd_init();
}

/* ............................. Entry actions ............................. */
static void
sendSync(ConMgrGsm *const me)
{
    ++me->retryCount;

    ModCmd_sync();
}

static void
sendInit(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_initStr();
}

static void
checkPin(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_getPinStatus();
}

static void
setPin(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_setPin(SIM_PIN_NUMBER);
}

static void
netTimeEnable(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_enableNetTime();
}

static void
getImei(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_getImei();
}

static void
cipShutdown(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_cipShutdown();
}

static void 
unregEntry(ConMgrGsm *const me)
{
    ModCmd_getRegStatus();

    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CHECK_REG_PERIOD);

    RKH_SET_STATIC_EVENT(&e_regTout, evRegTimeout);
    RKH_TMR_ONESHOT(&me->timerReg, RKH_UPCAST(RKH_SMA_T, me), REGISTRATION_TIME);
}

static void 
regEntry(ConMgrGsm *const me)
{
    (void)me;

    bsp_linkStatus(GSMNetwork, RegisteredSt);
}

static void 
failureEntry(ConMgrGsm *const me)
{
    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), FAILURE_TRY_DELAY);
    modPwr_off();
}

static void
waitRetryConfigEntry(ConMgrGsm *const me)
{
    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CONFIG_TRY_DELAY);
}

static void
setupManualGet(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_setManualGet();
}
   
static void
waitNetClockSyncEntry(ConMgrGsm *const me)
{
    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_NETCLOCK_TIME);
}

static void
getOper(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_getOper();
}
   
static void
setupAPN(ConMgrGsm *const me)
{
    Apn *apn;
    (void)me;

    apn = getAPNbyOper(me->Oper);
    ModCmd_setupAPN(apn->addr, apn->usr, apn->psw);
}
   
static void
startGPRS(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_startGPRS();
}

static void
getConnStatus(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_getConnStatus();
}

static void
isConnected(ConMgrGsm *const me)
{
    me->retryCount = 0;
}

static void
connectingEntry(ConMgrGsm *const me)
{
    (void)me;

    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CONNSTATUS_PERIOD);
}

static void
socketConnected(ConMgrGsm *const me)
{
    (void)me;

    me->retryCount = 0;
    ConnectionTopic_publish(&e_NetConnected, me);
    bsp_socketStatus(ETHNetwork, ConnectedSt);
}

static void
wReopenEntry(ConMgrGsm *const me)
{
    (void)me;

    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), REOPEN_DELAY);
}

static void
waitRetryConnEntry(ConMgrGsm *const me)
{
    (void)me;

    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CONNECT_TRY_DELAY);
}

static void
idleEntry(ConMgrGsm *const me)
{
    (void)me;
    
    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CONNSTATUS_PERIOD);
}


/* ............................. Exit actions ............................. */
static void 
unregExit(ConMgrGsm *const me)
{
    rkh_tmr_stop(&me->timer);
    rkh_tmr_stop(&me->timerReg);
}

static void 
regExit(ConMgrGsm *const me)
{
    (void)me;

    bsp_linkStatus(GSMNetwork, UnregisteredSt);
}

static void
waitNetClockSyncExit(ConMgrGsm *const me)
{
    rkh_tmr_stop(&me->timer);
}

static void
failureExit(ConMgrGsm *const me)
{
    (void)me;

    modPwr_on();
    ModCmd_init();
    rkh_tmr_stop(&me->timer);
}

static void
wReopenExit(ConMgrGsm *const me)
{
    (void)me;

    rkh_tmr_stop(&me->timer);
}

static void
waitRetryConnExit(ConMgrGsm *const me)
{
    (void)me;

    ModCmd_init();
    rkh_tmr_stop(&me->timer);
}

static void
connectingExit(ConMgrGsm *const me)
{
    (void)me;

    rkh_tmr_stop(&me->timer);
}

static void
socketDisconnected(ConMgrGsm *const me)
{
    (void)me;

    ConnectionTopic_publish(&e_NetDisconnected, me);
    bsp_socketStatus(ETHNetwork, DisconnectedSt);
}

static void
idleExit(ConMgrGsm *const me)
{
    (void)me;

    rkh_tmr_stop(&me->timer);
}

static void
getStatusExit(ConMgrGsm *const me)
{
    (void)me;

    rkh_sma_recall((RKH_SMA_T *)me, &qDefer);
}

/* ................................ Guards ................................. */
rbool_t
checkSyncTry(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_SYNC_RETRY) ? RKH_TRUE : RKH_FALSE;
}

rbool_t
checkConfigTry(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_CONFIG_RETRY) ? RKH_TRUE : RKH_FALSE;
}

rbool_t
checkConnectTry(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_CONNECT_RETRY) ? RKH_TRUE : RKH_FALSE;
}

rbool_t
checkConnectedFailCounter(ConMgrGsm *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_CONSTATUS_NORESP) ? RKH_TRUE : RKH_FALSE;
}

/* ---------------------------- Global functions --------------------------- */
ReceivedEvt *
ConMgrGsm_ReceiveDataGetRef(void)
{
    return &e_Received;
}

char *
ConMgrGsm_imei(void)
{
    ConMgrGsm *me;

    me = RKH_UPCAST(ConMgrGsm, conMgrGsm); 
    return me->Imei;
}

char *
ConMgrGsm_imeiSNR(void)
{
    ConMgrGsm *me;

    me = RKH_UPCAST(ConMgrGsm, conMgrGsm); 
    return (me->Imei + IMEI_SNR_OFFSET);
}

int
ConMgrGsm_sigLevel(void)
{
    ConMgrGsm *me;

    me = RKH_UPCAST(ConMgrGsm, conMgrGsm); 
    return me->sigLevel;
}

/* ------------------------------ End of file ------------------------------ */
