/**
 *  \file       sim900parser.c
 *  \brief      ...
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.09  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  2018.05.09  LeFr  v1.0.00  Initial version
 *  2018.05.14  DaBa  v1.0.01  Sync response
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "sim900parser.h"
#include "signals.h"
#include "modmgr.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static rui8_t sim900parser;
SSP_DCLR_NORMAL_NODE at, waitOK, at_plus_c, at_plus_ci, at_plus_cip,
                     at_plus_cips, 
                     at_plus_cipsta, at_plus_cipstart, at_plus_ciprxget,
                     at_plus_ciprxget_2, at_plus_ciprxget_2_wdata,
                     at_plus_cipstatus, at_plus_cipclose,
                     at_plus_cipsend, at_plus_cipsending, at_plus_cipsent,
                     at_plus_cpin, at_plus_creg, pinStatus, wpinSet, pinSet,
                     plus_c, plus_creg, at_plus_cipstatus, at_plus_cifsr;

SSP_DCLR_TRN_NODE at_plus_ciprxget_data;

static rui8_t isURC;

/* ----------------------- Local function prototypes ----------------------- */
static void cmd_ok(unsigned char pos);
static void sim_pin(unsigned char pos);
static void sim_error(unsigned char pos);
static void sim_ready(unsigned char pos);
static void isURC_set(unsigned char pos);
static void registered(unsigned char pos);
static void registered(unsigned char pos);
static void no_registered(unsigned char pos);
static void ipInitial(unsigned char pos);
static void ipStart(unsigned char pos);
static void ipStatus(unsigned char pos);
static void ipGprsAct(unsigned char pos);
static void ipDone(unsigned char pos);
static void connected(unsigned char pos);
static void disconnected(unsigned char pos);
static void data_init(unsigned char pos);
static void data_collect(unsigned char c);
static void data_ready(unsigned char pos);

/* ---------------------------- Local functions ---------------------------- */

SSP_CREATE_NORMAL_NODE(rootCmdParser);
SSP_CREATE_BR_TABLE(rootCmdParser)
	SSPBR("AT", NULL,     &at),
	SSPBR("+C", NULL,     &plus_c),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at);
SSP_CREATE_BR_TABLE(at)
	SSPBR("E",  NULL,       &waitOK),
	SSPBR("+C", NULL,       &at_plus_c),
	SSPBR("OK\r\n", cmd_ok, &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_c);
SSP_CREATE_BR_TABLE(at_plus_c)
	SSPBR("PIN",            NULL,   &at_plus_cpin),
	SSPBR("REG?\r\n\r\n",   NULL,   &at_plus_creg),
	SSPBR("STT=",           NULL,   &waitOK),
	SSPBR("I",              NULL,   &at_plus_ci),
	SSPBR("\r\n",   NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_ci);
SSP_CREATE_BR_TABLE(at_plus_ci)
	SSPBR("FSR\r\n",      NULL,  &at_plus_cifsr),
	SSPBR("ICR\r\n",      NULL,  &waitOK),
	SSPBR("P",            NULL,  &at_plus_cip),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cip);
SSP_CREATE_BR_TABLE(at_plus_cip)
	SSPBR("S",            NULL,  &at_plus_cips),
	SSPBR("CLOSE",        NULL,  &at_plus_cipclose),
	SSPBR("RXGET",        NULL,  &at_plus_ciprxget),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cips);
SSP_CREATE_BR_TABLE(at_plus_cips)
	SSPBR("TA",          NULL,  &at_plus_cipsta),
	SSPBR("END",         NULL,  &at_plus_cipsend),
	SSPBR("\r\n",        NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipsta);
SSP_CREATE_BR_TABLE(at_plus_cipsta)
	SSPBR("TUS\r\n",      NULL,  &at_plus_cipstatus),
	SSPBR("RT",           NULL,  &at_plus_cipstart),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ---------------------------- AT+CPIN --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cpin);
SSP_CREATE_BR_TABLE(at_plus_cpin)
	SSPBR("?\r\n\r\n",    NULL,  &pinStatus),
	SSPBR("=",            NULL,  &wpinSet),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(pinStatus);
SSP_CREATE_BR_TABLE(pinStatus)
	SSPBR("SIM PIN", sim_pin,   &rootCmdParser),
	SSPBR("ERROR",   sim_error, &rootCmdParser),
	SSPBR("READY",   sim_ready, &rootCmdParser),
	SSPBR("\r\n",    NULL,      &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(wpinSet);
SSP_CREATE_BR_TABLE(wpinSet)
	SSPBR("\r\n\r\n",   NULL,   &pinSet),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(pinSet);
SSP_CREATE_BR_TABLE(pinSet)
	SSPBR("OK\r\n", cmd_ok, &rootCmdParser),
	SSPBR("\r\n",   NULL,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- AT+CREG --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_creg);
SSP_CREATE_BR_TABLE(at_plus_creg)
	SSPBR("1,",      NULL,  &plus_creg),
	SSPBR("\r\n",    NULL,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPRXGET -------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_ciprxget);
SSP_CREATE_BR_TABLE(at_plus_ciprxget)
	SSPBR("1\r\n\r\nOK\r\n", cmd_ok,  &rootCmdParser),
	SSPBR("2,",              NULL,    &at_plus_ciprxget_2),
	SSPBR("\r\n",            NULL,    &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPRXGET=2 ------------------------ */
SSP_CREATE_NORMAL_NODE(at_plus_ciprxget_2);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_2)
	SSPBR("+CIPRXGET: 2",      NULL,   &at_plus_ciprxget_2_wdata),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_ciprxget_2_wdata);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_2_wdata)
	SSPBR("\r\n",      data_init,   &at_plus_ciprxget_data),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(at_plus_ciprxget_data, data_collect);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_data)
	SSPBR("\r\nOK\r\n", data_ready,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPSTATUS ------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipstatus);
SSP_CREATE_BR_TABLE(at_plus_cipstatus)
	SSPBR("INITIAL", ipInitial,  &rootCmdParser),
	SSPBR("STATUS",  ipStatus,   &rootCmdParser),
	SSPBR("TART",   ipStart,    &rootCmdParser),
	SSPBR("GPRSACT", ipGprsAct,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------- AT+CIPSTART ------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipstart);
SSP_CREATE_BR_TABLE(at_plus_cipstart)
	SSPBR("CONNECT OK", connected,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------- AT+CIPSEND -------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipsend);
SSP_CREATE_BR_TABLE(at_plus_cipsend)
	SSPBR(">", cmd_ok,  &at_plus_cipsending),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipsending);
SSP_CREATE_BR_TABLE(at_plus_cipsending)
	SSPBR("\x1A", NULL,  &at_plus_cipsent),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipsent);
SSP_CREATE_BR_TABLE(at_plus_cipsent)
	SSPBR("SEND OK", cmd_ok,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------- AT+CIPCLOSE ------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipclose);
SSP_CREATE_BR_TABLE(at_plus_cipclose)
	SSPBR("CLOSE OK", disconnected,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* -------------------------- AT+CIFSR --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cifsr);
SSP_CREATE_BR_TABLE(at_plus_cifsr)
	SSPBR(".",       ipDone,      &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- initStr --------------------------- */
/* --------------------------- AT+CSTT --------------------------- */
/* --------------------------- AT+CIICT -------------------------- */
SSP_CREATE_NORMAL_NODE(waitOK);
SSP_CREATE_BR_TABLE(waitOK)
	SSPBR("OK\r\n",     cmd_ok, &rootCmdParser),
	SSPBR("ERROR\r\n",  NULL,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* -------------------- Unsolicited +CREG ... -------------------- */
SSP_CREATE_NORMAL_NODE(plus_c);
SSP_CREATE_BR_TABLE(plus_c)
	SSPBR("REG:",   isURC_set, &plus_creg),
	SSPBR("\r\n",   NULL,      &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(plus_creg);
SSP_CREATE_BR_TABLE(plus_creg)
	SSPBR("0",     no_registered, &rootCmdParser),
	SSPBR("1",     registered,    &rootCmdParser),
	SSPBR("2",     no_registered, &rootCmdParser),
	SSPBR("3",     no_registered, &rootCmdParser),
	SSPBR("4",     no_registered, &rootCmdParser),
	SSPBR("5",     registered,    &rootCmdParser),
	SSPBR("\r\n",  NULL,          &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
static void
sendModResp_noArgs(RKH_SIG_T sig)
{
    ModMgrResp *p;
    RKH_SIG_T ModMgrSignal = evResponse;
       
    if(isURC)
    {
        isURC = 0;
        ModMgrSignal = evURC;
    }

    p = RKH_ALLOC_EVT( ModMgrResp, ModMgrSignal, &sim900parser );
    p->fwdEvt = sig;
    RKH_SMA_POST_FIFO( modMgr, RKH_UPCAST(RKH_EVT_T, p), &sim900parser );
}

static void
isURC_set(unsigned char pos)
{
    (void)pos;

    isURC = 1;
}

static void
cmd_ok(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evOk);
}

static void
sim_pin(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evSimPin);
}

static void
sim_error(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evSimError);
}

static void
sim_ready(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evSimReady);
}

static void
registered(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evReg);
}

static void
no_registered(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evNoReg);
}

static void
ipInitial(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evIPInitial);
}

static void
ipStart(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evIPStart);
}

static void
ipStatus(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evIPStatus);
}

static void
ipGprsAct(unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(evIPGprsAct);
}

static void
ipDone(unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(evIP);
}

static void
connected(unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(evConnected);
}

static void
disconnected(unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(evDisconnected);
}

/* TODO: Resolve Packet reception */
#include <stdio.h>
static unsigned char rxbuff[128];
static unsigned char *prx;

static void
data_init(unsigned char pos)
{
    (void)pos;

    prx = rxbuff; 
}

static void
data_collect(unsigned char c)
{
    *prx = c;
    ++prx;
}

static void
data_ready(unsigned char pos)
{
    (void)pos;

   *prx = '\0'; 
   
   printf("\r\n------ RX PACKET: %s\r\n", rxbuff);

   sendModResp_noArgs(evOk);
}
/************************************/

/* ---------------------------- Global functions --------------------------- */
void
parser_init(void)
{
   RKH_TR_FWK_ACTOR(&sim900parser, "sim900parser");
}

/* ------------------------------ End of file ------------------------------ */
