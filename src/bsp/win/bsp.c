/**
 *  \file       bsp.c
 *  \brief      BSP for 80x86 OS win32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "rkhfwk_pubsub.h"
#include "bsp.h"
#include "getopt.h"
#include "trace_io_cfg.h"
#include "wserial.h"
#include "wserdefs.h"
#include "failure.h"

#include "signals.h"
#include "topics.h"
#include "modcmd.h"
#include "modmgr.h"
#include "modpwr.h"
#include "din.h"
#include "anin.h"
#include "IOSampler.h"
#include "anSampler.h"
#include "mTime.h"
#include "publisher.h"
#include "eth.h"
#include "config.h"
#include "calc.h"


RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ESC                    0x1B
#define RRTESTER_CFG_OPTIONS   "st:f:p:b:q:m:h"

#define TEST_TX_PACKET      "----o Ping"
#define TEST_RX_PACKET      "o---- Pong"

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
SERIAL_T serials[ NUM_CHANNELS ] =
{
	{	"COM8",	19200, 8, PAR_NONE, STOP_1, 0 },	// COM1
};
static FILE *fGsmLog = NULL;

/* ---------------------------- Local variables ---------------------------- */
static rui8_t bsp;
static ModCmdRcvHandler cmdParser;
static char *opts = (char *)RRTESTER_CFG_OPTIONS;
static const char *helpMessage =
{
    "\nOption usage:\n"
	"\t -s silence\n"
    "\t -f File name for binary trace output\n"
    "\t -t ipaddr of TCP trace client\n"
    "\t -p port of TCP trace client\n"
	"\t -b ipaddr of MQTT Broker\n"
	"\t -q port of MQTT Broker\n"
    "\t -m GSM Module Serial Port\n"
    "\t -h (help)\n"
};

static RKH_ROM_STATIC_EVENT(e_Term, evTerminate);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);
static RKH_ROM_STATIC_EVENT(e_Ok, evOk);
static RKH_ROM_STATIC_EVENT(e_Recv, evRecv);
static RKH_ROM_STATIC_EVENT(e_publishTout, evWaitPublishTout);
static SendEvt e_Send;

static void ser_rx_isr(unsigned char byte);
static void ser_tx_isr(void);
static SERIAL_CBACK_T ser_cback =
{ ser_rx_isr, NULL, NULL, ser_tx_isr, NULL, NULL, NULL };

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
printBanner(void)
{
    printf("RRTESTER.\n\n");
    printf("RKH version      = %s\n", RKH_RELEASE);
    printf("Port version     = %s\n", rkhport_get_version());
    printf("Port description = %s\n\n", rkhport_get_desc());
	printf("Description: \n\n"
		"Sistema probador de relés ferroviarios\n");

    printf("1.- Press ESC to quit \n\n\n");
}

static void
processCmdLineOpts(int argc, char **argv)
{
    int c;

    while ((c = getopt(argc, argv, opts)) != EOF)
        switch (c)
        {
            case 'm':
                strcpy(serials[GSM_PORT].com_name, optarg);
                break;

			case 's':
                trace_io_silence();
				break;

            case 'f':
                trace_io_setFileName(optarg);
                break;

            case 't':
                trace_io_setTcpIpAddr(optarg);
                break;

            case 'p':
                trace_io_setTcpPort((short)atoi(optarg));
                break;

            case 'b':
            	config_brokerAddress(optarg);
            	break;

            case 'q':
            	config_brokerPort(optarg);
            	break;

            case '?':
            case 'h':
                printf(helpMessage);
                break;
        }
}

static
void
send_signalsFrame(void)
{
    AnSampleSet anSet;
    IOChg ioChg[NUM_DI_SAMPLES_GET];
    int n, l, i, j;
    char *p;

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Send), evSend);
   
    n = anSampler_getSet(&anSet, NUM_AN_SAMPLES_GET);
   
    p = (char *)(e_Send.buf);
    l = 0;

    for(i=0; i < NUM_AN_SIGNALS; ++i)
    {
        l += sprintf(p + l, "ts:%u, AN[%d]", anSet.timeStamp, i);

        for(j=0; j<n; ++j)
        {
            l += sprintf(p + l,", %d.%02d",
                               (char)((anSet.anSignal[i][j] & 0xFF00) >> 8),
                               (char)(anSet.anSignal[i][j] & 0x00FF));
        }

        l += sprintf(p + l, "\r\n");
    }

    n = IOSampler_get(ioChg, NUM_DI_SAMPLES_GET);
    
    for(i=0; i < n; ++i)
    {
        l += sprintf(p + l, "ts:%u, DI:%d\r\n", ioChg[i].timeStamp,
                                                    ioChg[i].signalValue );
    }

    e_Send.size = l;

    printf("Write GPRS Socket:\r\n");
    printf("%s\r\n", e_Send.buf);

    ConnectionTopic_publish(&e_Send, &bsp);
}

void
toggleRelayFailure()
{
    failure_set(! failure_get());
}

static
void
setDeviceID(void)
{
    char macAddress[6];
    uint32_t crc;
    char id[9];
    if (eth_getMACaddress(macAddress))
    {
        crc = rc_crc32(0, (char *) &macAddress, 6);
        snprintf(id,9,"%X",crc);
        config_clientId(id);
        config_topic(id);
    }
}

/* ---------------------------- Global functions --------------------------- */
void
bsp_init(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printBanner();

    processCmdLineOpts(argc, argv);

    setDeviceID();

    modPwr_init();
    dIn_init();
	anIn_init();
	failure_init();

    eth_init();
}

void
bsp_keyParser(int c)
{
    switch(c)
    {
        case ESC:
            rkhport_fwk_stop();
            break;

        case 'o':
            printf("Open GPRS Socket\r\n");
            ConnectionTopic_publish(&e_Open, &bsp);
            break;

        case 'c':
            printf("Close GPRS Socket\r\n");
            ConnectionTopic_publish(&e_Close, &bsp);
            break;

        case 'r':
            printf("Read GPRS Socket\r\n");
            ConnectionTopic_publish(&e_Recv, &bsp);
            break;

        case 's':
            RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Send), evSend);
            e_Send.size = strlen(TEST_TX_PACKET);

            memcpy(e_Send.buf, (unsigned char *)TEST_TX_PACKET, e_Send.size);

            printf("Write GPRS Socket:\r\n");

            ConnectionTopic_publish(&e_Send, &bsp);
            break;

        case 'a':
            send_signalsFrame();
            break;

		case 'j':
			set_dtr(0);
			break;

		case 'k':
			reset_dtr(0);
			break;

		case 'f':
			printf("Forced Publication.\r\n");
            RKH_SMA_POST_FIFO(mqttProt, &e_publishTout, &bsp);
			break;

        case 'x':
            toggleRelayFailure();
            break;

        default:
            break;
    }
}

void
bsp_timeTick(void)
{
    dIn_scan();
    mTime_tick();
}

static
void
ser_rx_isr( unsigned char byte )
{
    cmdParser(byte);
    if (fGsmLog != NULL)
    {
        fwrite(&byte, 1, 1, fGsmLog);
		fflush(fGsmLog);
    }
}

static
void
ser_tx_isr( void )
{
}

void
bsp_eth_open(void)
{
}

void
bsp_serial_open(int ch)
{
	if ((fGsmLog = fopen("UartGSM.log","w+b")) == NULL)
		printf("Can't open uart log file for GSM\n");

    cmdParser = ModCmd_init();
    init_serial_hard(ch, &ser_cback );
    connect_serial(ch);
	set_dtr(ch);
	Sleep(500);
    RKH_TR_FWK_ACTOR(&bsp, "bsp");
}

void
bsp_serial_close(int ch)
{
	set_dtr(ch);
	disconnect_serial(ch);
	deinit_serial_hard(ch);
	fclose(fGsmLog);
}

void
bsp_serial_puts(int ch, char *p)
{
    while(*p!='\0')
    {
		if (fGsmLog != NULL)
		{
			fwrite(p, 1, 1, fGsmLog);
			fflush(fGsmLog);
		}
        tx_data(ch, *p);
        ++p;
    }
}

void
bsp_serial_putnchar(int ch, unsigned char *p, ruint ndata)
{
    while(ndata && (ndata-- != 0))
    {
        tx_data(ch, *p);
        ++p;
    }
}

void
bsp_linkStatus(NetType_t t, Status_t status)
{
    if(t == GSMNetwork)
    {
        printf("\r\nGSM Network %s\r\n", 
            status == RegisteredSt ? "Registered" : "Unregistered");
    }
    else
    {
        printf("\r\nETH Link %s\r\n", 
            status == ConnectedSt ? "Connected" : "Disconnected");
    }
}

void 
bsp_socketStatus(NetType_t t, Status_t status)
{
    printf("\r\n%s Socket %s\r\n", 
            t == GSMNetwork ? "GSM" : "ETH",
            status == ConnectedSt ? "Connected" : "Disconnected");
}

void 
bsp_modStatusToggle(void)
{
}

/* ------------------------------ File footer ------------------------------ */
