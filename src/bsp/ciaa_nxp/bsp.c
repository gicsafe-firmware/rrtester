/**
 *  \file       bsp.c
 *  \brief      BSP for CIAA-NXP Prototype
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v0.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  CaMa  Carlos Mancón      manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "sapi.h"
#include "uartisr.h"
#include "bsp.h"

#include "config.h"
#include "signals.h"
#include "modpwr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "CirBuffer.h"
#include "mTime.h"
#include "din.h"
#include "DigIn.h"
#include "anin.h"
#include "anSampler.h"
#include "IOSampler.h"
#include "epoch.h"
#include "rtime.h"
#include "eth.h"
#include "emc.h"
#include "cr_section_macros.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
#define ModStatus_init()    gpioConfig(DO5, GPIO_OUTPUT)
#define ModStatus(b)        gpioWrite(DO5, b)
#define ModStatus_toggle()  gpioToggle(DO5)
#define LinkStatus_init()   gpioConfig(DO6, GPIO_OUTPUT)
#define LinkStatus(b)       gpioWrite(DO6, b)
#define SocketStatus_init() gpioConfig(DO7, GPIO_OUTPUT)
#define SocketStatus(b)     gpioWrite(DO7, b)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_TS_T tstamp;
static ModCmdRcvHandler cmdParser;

/* Example for SDRAM usage */
__NOINIT(RAM_EXT) uint8_t data_buffer[1024];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static
uint32_t
readUID(uint32_t uid[])
{
    /* Chip_IAP_ReadUID() implementation from LPCOpen 3.02
     * current version (v2.16) has a known bug
     * */
    uint32_t command[5], result[5];
    uint32_t i;

    command[0] = IAP_READ_UID_CMD;
    iap_entry(command, result);

    for (i=0; i < 4; i++)
        uid[i] = result[i + 1];

    return result[0];
}

static
uint32_t
rc_crc32(uint32_t crc, char *buf, size_t len)
{
    /* From rosettacode.org */
    static uint32_t table[256];
    uint32_t rem;
    uint8_t octet;
    int i, j;
    const char *p, *q;

    /* Calculate CRC table. */
    for (i = 0; i < 256; i++)
    {
        rem = i;  /* remainder from polynomial division */
        for (j = 0; j < 8; j++)
        {
            if (rem & 1)
            {
                rem >>= 1;
                rem ^= 0xedb88320;
            }
            else
            {
                rem >>= 1;
            }
        }
        table[i] = rem;
    }

    crc = ~crc;
    q = buf + len;
    for (p = buf; p < q; p++)
    {
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}

static
void
setDeviceID(void)
{
    uint32_t array[4];
    uint32_t crc;
    char id[9];
    if (0 == readUID(array))
    {
        crc = rc_crc32(0, (char *) &array, 16);
        snprintf(id, 9, "%X", crc);
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

    boardConfig();

#ifdef  __SDRAM_TEST__
    emc_pinInit();
    emc_dramInit();
    if(!emc_testRAM(CIAA_EMC_LPC43XX_SDRAM_BASE, CIAA_EMC_LPC43XX_SDRAM_SIZE))
    {
    	while (1);
    }
#endif

    setDeviceID();

    ModStatus_init();
    ModStatus(0);
    LinkStatus(UnregisteredSt);
    SocketStatus_init();
    SocketStatus(DisconnectedSt);

    modPwr_init();

    dIn_init();
    DigIn_init();

    anIn_init();
    rtime_init();

    eth_init();
}

void
bsp_timeTick(void)
{
    ++tstamp;

    dIn_scan();
    modPwr_ctrl();
    mTime_tick();
}

RKH_TS_T
rkh_trc_getts(void)
{
    return tstamp;
}

static
void
gsm_rx_isr(unsigned char byte)
{
    cmdParser(byte);
}

void
bsp_eth_open(void)
{
    //eth_init();
}

void
bsp_serial_open(int ch)
{
    (void)ch;

    cmdParser = ModCmd_init();
    uartConfig(UART_232, 19200);
    uartIsr_rxEnable(UART_232, gsm_rx_isr);
}

void
bsp_serial_close(int ch)
{
    (void)ch;
}

void
bsp_serial_puts(int ch, char *p)
{
    while (*p != '\0')
    {
        uartWriteByte(UART_232, *p);
        ++p;
    }
}

void
bsp_serial_putnchar(int ch, unsigned char *p, ruint ndata)
{
    while (ndata && (ndata-- != 0))
    {
        uartWriteByte(UART_232, *p);
        ++p;
    }
}

void
bsp_linkStatus(NetType_t t, Status_t status)
{
    LinkStatus(status);
}

void
bsp_socketStatus(NetType_t t, Status_t status)
{
    SocketStatus(status);
}

void
bsp_modStatusToggle(void)
{
    ModStatus_toggle();
}

/* ------------------------------ File footer ------------------------------ */
