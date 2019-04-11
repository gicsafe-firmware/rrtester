/**
 *  \file       main.c
 *  \brief      Example application.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <string.h>
#include <stdio.h>
#include "rkh.h"
#include "rkhfwk_dynevt.h"
#include "bsp.h"
#include "signals.h"
#include "mqttProt.h"
#include "rrtesterCfg.h"
#include "ethMgr.h"
#include "conmgr.h"
#include "modmgr.h"
#include "mTime.h"
#include "epoch.h"
#include "anSampler.h"
#include "ioChgDet.h"
#include "CirBuffer.h"
#include "publisher.h"

/* ----------------------------- Local macros ------------------------------ */
#define MQTTPROT_QSTO_SIZE  16
#define ETHMGR_QSTO_SIZE    8
#define CONMGR_QSTO_SIZE    8
#define MODMGR_QSTO_SIZE    8

#define SIZEOF_EP0STO       16
#define SIZEOF_EP0_BLOCK    sizeof(RKH_EVT_T)
#define SIZEOF_EP1STO       128
#define SIZEOF_EP1_BLOCK    sizeof(ModCmd)
#define SIZEOF_EP2STO       512
#define SIZEOF_EP2_BLOCK    sizeof(ModMgrEvt)

/* ------------------------------- Constants ------------------------------- */
#ifdef __NO_OFFICIAL_PORT__
#define modmgr_stk_size         512
#define conmgr_stk_size         512
#define mqttprot_stk_size       512
#define ethmgr_stk_size         512
#else
#define modmgr_stk_size         0
#define conmgr_stk_size         0
#define mqttprot_stk_size       0
#define ethmgr_stk_size         0
#endif

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
#ifdef __NO_OFFICIAL_PORT__
static RKH_THREAD_STK_TYPE ModMgrStack[MODMGR_STK_SIZE];
static RKH_THREAD_STK_TYPE ConMgrStack[CONMGR_STK_SIZE];
static RKH_THREAD_STK_TYPE EthMGRStack[ETHMGR_STK_SIZE];
static RKH_THREAD_STK_TYPE MQTTProtStack[MQTTPROT_STK_SIZE];
#else
#define ModMgrStack     0
#define ConMgrStack     0
#define EthMGRStack     0
#define MQTTProtStack   0
#endif

static RKH_EVT_T *ModMgr_qsto[MODMGR_QSTO_SIZE];
static RKH_EVT_T *ConMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T *EthMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T *MQTTProt_qsto[MQTTPROT_QSTO_SIZE];

static rui8_t evPool0Sto[SIZEOF_EP0STO],
              evPool1Sto[SIZEOF_EP1STO],
              evPool2Sto[SIZEOF_EP2STO];

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static MQTTProtCfg mqttProtCfg;

/* ----------------------- Local function prototypes ----------------------- */
static void setupTraceFilters(void);
static void rrtesterStartup(void);

/* ---------------------------- Local functions ---------------------------- */
static void
setupTraceFilters(void)
{
    RKH_FILTER_ON_GROUP(RKH_TRC_ALL_GROUPS);
    RKH_FILTER_ON_EVENT(RKH_TRC_ALL_EVENTS);
    RKH_FILTER_OFF_EVENT(MODCMD_USR_TRACE);
    RKH_FILTER_OFF_EVENT(ETH_USR_TRACE);
    RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_USR);
    RKH_FILTER_OFF_EVENT(RKH_TE_TMR_TOUT);
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_STATE);
    /* RKH_FILTER_OFF_EVENT(RKH_TE_SMA_FIFO); */
    /* RKH_FILTER_OFF_EVENT(RKH_TE_SMA_LIFO); */
    /* RKH_FILTER_OFF_EVENT(RKH_TE_SM_TS_STATE); */
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_DCH);
    RKH_FILTER_OFF_SMA(modMgr);
    RKH_FILTER_OFF_SMA(conMgr);
    RKH_FILTER_OFF_SMA(ethMgr);
    RKH_FILTER_OFF_SMA(mqttProt);
    RKH_FILTER_OFF_ALL_SIGNALS();
}

static void
rrtesterStartup(void)
{
    setupTraceFilters();

    RKH_TRC_OPEN();

    rkh_dynEvt_init();
    rkh_fwk_registerEvtPool(evPool0Sto, SIZEOF_EP0STO, SIZEOF_EP0_BLOCK);
    rkh_fwk_registerEvtPool(evPool1Sto, SIZEOF_EP1STO, SIZEOF_EP1_BLOCK);
    rkh_fwk_registerEvtPool(evPool2Sto, SIZEOF_EP2STO, SIZEOF_EP2_BLOCK);

    mqttProtCfg.publishTime = MAX_PUBLISH_TIME;
    mqttProtCfg.syncTime = 4;
    mqttProtCfg.keepAlive = 400;
    mqttProtCfg.qos = 1;
    strcpy(mqttProtCfg.clientId, "");
    strcpy(mqttProtCfg.topic, "");
    MQTTProt_ctor(&mqttProtCfg, publishrrtester);

    RKH_SMA_ACTIVATE(conMgr, ConMgr_qsto, CONMGR_QSTO_SIZE,
                     ConMgrStack, CONMGR_STK_SIZE);
    RKH_SMA_ACTIVATE(ethMgr, EthMgr_qsto, ETHMGR_QSTO_SIZE,
                     EthMGRStack, ETHMGR_STK_SIZE);
    RKH_SMA_ACTIVATE(modMgr, ModMgr_qsto, MODMGR_QSTO_SIZE,
                     ModMgrStack, MODMGR_STK_SIZE);
    RKH_SMA_ACTIVATE(mqttProt, MQTTProt_qsto, MQTTPROT_QSTO_SIZE,
                     MQTTProtStack, MQTTPROT_STK_SIZE);

    RKH_SMA_POST_FIFO(conMgr, &e_Open, 0);
    RKH_SMA_POST_FIFO(ethMgr, &e_Open, 0);
}

/* ---------------------------- Global functions --------------------------- */
void
rrtesterCfg_clientId(char *pid)
{
    strcpy(mqttProtCfg.clientId, pid);
}

void
rrtesterCfg_topic(char *t)
{
    sprintf(mqttProtCfg.topic, "/rrtester/%s", t);
}

#ifdef __NO_OFFICIAL_PORT__
void
rkh_startupTask(void *pvParameter)
{
    rrtesterStartup();

    rkh_fwk_enter();

    vTaskDelete(NULL);
}
#endif

int
main(int argc, char *argv[])
{
    bsp_init(argc, argv);

    anSampler_init();
    IOChgDet_init();
    epoch_init();
    mTime_init();

    rkh_fwk_init();

#ifdef __NO_OFFICIAL_PORT__
    vTaskStartScheduler();
#else
    rrtesterStartup();
    
    rkh_fwk_enter();

    RKH_TRC_CLOSE();
#endif

    return 0;
}

/* ------------------------------ End of file ------------------------------ */
