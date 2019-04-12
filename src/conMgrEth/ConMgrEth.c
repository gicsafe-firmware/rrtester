/**
 *  \file       ConMgrEth.c
 *  \brief      Active object implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkhsma.h"
#include "signal.h"
#include "ConMgrEth.h"
#include "ConMgrEthAct.h"

/* ----------------------------- Local macros ------------------------------ */
/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(inactive ,NULL ,NULL ,RKH_ROOT, NULL);

RKH_CREATE_BASIC_STATE(unplugged ,NULL ,NULL ,&active, NULL);

RKH_CREATE_BASIC_STATE(idle ,NULL ,NULL ,&connected, NULL);

RKH_CREATE_BASIC_STATE(receiving ,NULL ,NULL ,&connected, NULL);

RKH_CREATE_BASIC_STATE(sending ,NULL ,NULL ,&connected, NULL);

RKH_CREATE_BASIC_STATE(waitServer ,ConMgrEth_enwaitServer ,NULL ,&connecting, NULL);

RKH_CREATE_BASIC_STATE(waitIp ,NULL ,NULL ,&plugged, NULL);

RKH_CREATE_COMP_REGION_STATE(active ,NULL ,NULL ,RKH_ROOT, &unplugged, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

RKH_CREATE_COMP_REGION_STATE(plugged ,NULL ,NULL ,&active, &waitIp, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

RKH_CREATE_COMP_REGION_STATE(connecting ,NULL ,NULL ,&plugged, &waitServer, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

RKH_CREATE_COMP_REGION_STATE(connected ,NULL ,NULL ,&connecting, &idle, NULL, RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

RKH_CREATE_TRANS_TABLE(inactive)
RKH_TRREG(evOpen, NULL, NULL, &active),
RKH_TRINT(evRecv, NULL, ConMgrEth_inactiveToinactiveLoc0),
RKH_TRINT(evSend, NULL, ConMgrEth_inactiveToinactiveLoc1),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(active)
RKH_TRREG(evClose, NULL, NULL, &inactive),
RKH_TRINT(evRecv, NULL, ConMgrEth_activeToactiveLoc2),
RKH_TRINT(evSend, NULL, ConMgrEth_activeToactiveLoc3),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(unplugged)
RKH_TRREG(evEthLinkConnect, NULL, NULL, &plugged),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(plugged)
RKH_TRREG(evEthLinkDisconnect, NULL, NULL, &unplugged),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(connecting)
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(connected)
RKH_TRREG(evDisconnected, NULL, NULL, &waitServer),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(idle)
RKH_TRREG(evRecv, NULL, ConMgrEth_idleToreceivingExt7, &receiving),
RKH_TRREG(evSend, NULL, ConMgrEth_idleTosendingExt8, &sending),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(receiving)
RKH_TRREG(evOk, NULL, ConMgrEth_receivingToidleExt10, &idle),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(sending)
RKH_TRREG(evOk, NULL, ConMgrEth_sendingToidleExt11, &idle),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(waitServer)
RKH_TRREG(evConnected, NULL, NULL, &connected),
RKH_END_TRANS_TABLE

RKH_CREATE_TRANS_TABLE(waitIp)
RKH_TRREG(evIPStatus, NULL, NULL, &connecting),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */
RKH_SMA_CREATE(ConMgrEth, conMgrEth, 2, HCAL, &inactive, ConMgrEth_ToinactiveExt0, NULL);
RKH_SMA_DEF_PTR(conMgrEth);

/* ------------------------------- Constants ------------------------------- */

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
