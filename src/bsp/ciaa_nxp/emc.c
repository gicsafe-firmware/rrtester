/*
 *  --------------------------------------------------------------------------
 *
 *                               GICSAFe-Firmware
 *                               ----------------
 *
 *                      Copyright (C) 2019 CONICET-GICSAFe
 *          All rights reserved. Protected by international copyright laws.
 *
 *  Contact information:
 *  site: https://github.com/gicsafe-firmware
 *  e-mail: <someone>@<somewhere>
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file   emc.c
 *  \brief  Brief description and purpose of this file.
 *
 *  Long description and purpose of this file.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RiGr  Rick Grimes  rick.grimes@twd.com
 */

/* --------------------------------- Notes --------------------------------- */
/*
 *  (1) These notes could be referenced by number.
 *      (1.1) Sub-note.
 *  (2) Another note.
 */

/* ----------------------------- Include files ----------------------------- */
/* (1) #include <string.h>                                                   */
/* (2) #include <other system headers>                                       */
/* (3) #include "user header files"                                          */
#include "chip.h"
#include "emc.h"

/* ----------------------------- Local macros ------------------------------ */
/* (1) Function macro defines */

/*
 * These macros should be used to compute the value for the second argument of
 * `lpc18xx_iomux_config()` (`regval`). This value will be copied
 * into an SCU_SFS register.
 */
/*
 * Normal drive pins
 */
#define LPC18XX_IOMUX_CONFIG(func,epd,epun,ehs,ezi,zif) \
    ((func << LPC18XX_IOMUX_CONFIG_FUNC_BITS) | \
     (epd << LPC18XX_IOMUX_CONFIG_EPD_BIT) | \
     (epun << LPC18XX_IOMUX_CONFIG_EPUN_BIT) | \
     (ehs << LPC18XX_IOMUX_CONFIG_EHS_BIT) | \
     (ezi << LPC18XX_IOMUX_CONFIG_EZI_BIT) | \
     (zif << LPC18XX_IOMUX_CONFIG_ZIF_BIT))

/*
 * Pin settings for EMC pins
 */
#define LPC18XX_IOMUX_EMC_CONFIG(func) \
    (LPC18XX_IOMUX_CONFIG(func, 0, 1, 1, 1, 1))
/*
 * Pin settings for slow EMC pins (SPIFI)
 */
#define LPC18XX_IOMUX_SPIFI_CONFIG(func) \
    (LPC18XX_IOMUX_CONFIG(func, 0, 1, 0, 1, 1))
/* ------------------------------- Constants ------------------------------- */
/* (1) Macro defines                                                         */

/*
 * Bits and bit groups inside the SCU_SFS registers
 */
/* Selects pin function */
#define LPC18XX_IOMUX_CONFIG_FUNC_BITS  0
/* Enable pull-down resistor at pad */
#define LPC18XX_IOMUX_CONFIG_EPD_BIT    3
/* Disable pull-up resistor at pad */
#define LPC18XX_IOMUX_CONFIG_EPUN_BIT   4
/* Select Slew rate */
#define LPC18XX_IOMUX_CONFIG_EHS_BIT    5
/* Input buffer enable */
#define LPC18XX_IOMUX_CONFIG_EZI_BIT    6
/* Input glitch filter */
#define LPC18XX_IOMUX_CONFIG_ZIF_BIT    7

/* EMC Control enable */
#define LPC_EMC_CTRL_EN_MSK (1 << 0)

/* EMC Config endianness */
#define LPC_EMC_CONF_LITTLE_ENDIAN_MSK  0
#define LPC_EMC_CONF_BIG_ENDIAN_MSK     1

/*
 * All clock configuration registers
 */
#define LPC_CCU1_CLK_RUN_MSK    (1 << 0)
/*
 * CLK_M4_EMCDIV_CFG register
 */
/* Divider selector */
#define LPC_CCU1_CLK_EMCDIV_CFG_DIV2    (1 << 5)

/* EMC_CLK divided clock select */
#define LPC_CREG_CREG6_EMCCLKSEL_MSK    (1 << 16)

/*
 * IS42S16400F SDRAM: 16-bit, 4 banks, 12 row bits, 8 column bits.
 * See table 433 "Address mapping" on page 613 in the LPC43xx User Manual
 * (UM10503 Rev. 2.3).
 */
#define LPC_EMC_AM      0x05

/* Timings for 102 MHz SDRAM clock and IS42S16400F-6TL memory chip */

/* Active to read/write delay (RAS latency) */
#define SDRAM_RAS       2   /* tRCD = 18ns */
/* CAS latency (CL) */
#define SDRAM_CAS       2   /* CL = 2 */
/* Command delayed strategy, using EMCCLKDELAY */
#define SDRAM_RDCFG_RD  1
/* Precharge command period (tRP) */
#define SDRAM_T_RP      2   /* 18ns */
/* Active to precharge command period (tRAS) */
#define SDRAM_T_RAS     5   /* 42ns */
/* Self-refresh exit time (tSREX) */
#define SDRAM_T_SREX    7   /* We set this to the same as tXSR */
/* Last-data-out to active command time (tAPR) */
#define SDRAM_T_APR     6   /* Not found in the SDRAM datasheet */
/* Data-in to active command (tDAL) */
#define SDRAM_T_DAL     5   /* 5 cycles */
/* Write recovery time (tWR) */
#define SDRAM_T_WR      2   /* 2 cycles */
/* Active to active command period (tRC) */
#define SDRAM_T_RC      7   /* 60ns */
/* Auto-refresh period and auto-refresh to active command period (tRFC) */
#define SDRAM_T_RFC     7   /* 60ns */
/* Exit self-refresh to active command time (tXSR) */
#define SDRAM_T_XSR     7   /* 60ns */
/* Active bank A to active bank B latency (tRRD) */
#define SDRAM_T_RRD     2   /* 12ns */
/* Load mode register to active command time (tMRD) */
#define SDRAM_T_MRD     2   /* 2 cycles */

/*
 * Dynamic Memory Configuration register
 */
/* Address mapping */
#define LPC_EMC_DYCFG_AM_BITS       7
/* Buffer enable */
#define LPC_EMC_DYCFG_B_MSK     (1 << 19)
/*
 * Dynamic Memory RAS & CAS Delay register
 */
/* RAS latency */
#define LPC_EMC_DYRASCAS_RAS_BITS   0
/* CAS latency */
#define LPC_EMC_DYRASCAS_CAS_BITS   8

/*
 * Dynamic Memory Read Configuration register:
 *     Read data strategy (RD)
 */
#define LPC_EMC_DYRDCFG_RD_BITS     0

/*
 * Dynamic Memory Control register
 */
/* Dynamic memory clock enable (CE) */
#define LPC_EMC_DYCTRL_CE_MSK       (1 << 0)
/* Dynamic memory clock control (CS) */
#define LPC_EMC_DYCTRL_CS_MSK       (1 << 1)
/* SDRAM initialization (I) */
#define LPC_EMC_DYCTRL_I_BITS       7
#define LPC_EMC_DYCTRL_I_NORMAL     0
#define LPC_EMC_DYCTRL_I_MODE       1
#define LPC_EMC_DYCTRL_I_PALL       2   /* precharge all */
#define LPC_EMC_DYCTRL_I_NOP        3   /* no operation */

/*
 * Refresh timer.
 * Indicates the multiple of 16 CCLKs between SDRAM refresh cycles.
 */
/* 99 = 64000000[64ms] / 4096[rows] / 9.80[ns] / 16; round down */
#define SDRAM_REFRESH       99
/* Only for initialization */
#define SDRAM_REFRESH_FAST  1

/* (2) Enumerations                                                          */
/* (3) Local const modifier                                                  */
typedef enum
{
    WE,

    A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, BA0, BA1,

    D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15,

    CKE, CS,

    RAS, CAS, DQM0, DQM1,

    A0,

    /* SPIFI */
    SPIFI_SCK, SPIFI_SIO3, SPIFI_SIO2, SPIFI_MISO, SPIFI_MOSI, SPIFI_CS,
} emcMap_t;

typedef enum
{
    CLK0, CLK1, CLK2, CLK3
}emcClocks_t;

/* (4) Global (extern or non-static external const modifier)                 */

/* ---------------------------- Local data types --------------------------- */
/* (1) typedefs                                                              */
typedef struct
{
    int8_t port;
    int8_t pin;
} pinConfigLpc4337_t;

typedef struct
{
    pinConfigLpc4337_t pinName;
    uint8_t func;
} pinConfigEmcLpc4337_t;

typedef struct
{
    emcClocks_t clknum;
    uint8_t func;
} pinClockConfigEmcLpc4337_t;

/* ---------------------------- Global variables --------------------------- */
/* (1) Extern declarations of variables defined in other files.              */
/* (2) Non-static external definitions used in this file (and optionally     */
/*     in others if they are declared in those files using extern).          */

/* ---------------------------- Local variables ---------------------------- */
/* (1) Static external definitions used only in this file.                   */

static
const pinConfigEmcLpc4337_t emcPinsConfig[] =
{
    /*
     * EMC pins used for both the SDRAM and the NOR flash memory chips
     */
    /* P1.6 = WE# - SDRAM,NOR */
    {{0x1,  6}, LPC18XX_IOMUX_EMC_CONFIG(3)},

    /* P2.10 = A1 - SDRAM,NOR */
    {{0x2, 10}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P2.11 = A2 - SDRAM,NOR */
    {{0x2, 11}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P2.12 = A3 - SDRAM,NOR */
    {{0x2, 12}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P2.13 = A4 - SDRAM,NOR */
    {{0x2, 13}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.0 = A5 - SDRAM,NOR */
    {{0x1,  0}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P1.1 = A6 - SDRAM,NOR */
    {{0x1,  1}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P1.2 = A7 - SDRAM,NOR */
    {{0x1,  2}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P2.8 = A8 - SDRAM,NOR */
    {{0x2,  8}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P2.7 = A9 - SDRAM,NOR */
    {{0x2,  7}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P2.6 = A10 - SDRAM,NOR */
    {{0x2,  6}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P2.2 = A11 - SDRAM,NOR */
    {{0x2,  2}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P2.1 = A12 - SDRAM,NOR */
    {{0x2,  1}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P2.0 = BA0 for SDRAM (aka A13) - SDRAM,NOR */
    {{0x2,  0}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P6.8 = BA1 for SDRAM (aka A14) - SDRAM,NOR */
    {{0x6,  8}, LPC18XX_IOMUX_EMC_CONFIG(1)},

    /* P1.7 = D0 - SDRAM,NOR */
    {{0x1,  7}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.8 = D1 - SDRAM,NOR */
    {{0x1,  8}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.9 = D2 - SDRAM,NOR */
    {{0x1,  9}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.10 = D3 - SDRAM,NOR */
    {{0x1, 10}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.11 = D4 - SDRAM,NOR */
    {{0x1, 11}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.12 = D5 - SDRAM,NOR */
    {{0x1, 12}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.13 = D6 - SDRAM,NOR */
    {{0x1, 13}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P1.14 = D7 - SDRAM,NOR */
    {{0x1, 14}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P5.4 = D8 - SDRAM,NOR */
    {{0x5,  4}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.5 = D9 - SDRAM,NOR */
    {{0x5,  5}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.6 = D10 - SDRAM,NOR */
    {{0x5,  6}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.7 = D11 - SDRAM,NOR */
    {{0x5,  7}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.0 = D12 - SDRAM,NOR */
    {{0x5,  0}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.1 = D13 - SDRAM,NOR */
    {{0x5,  1}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.2 = D14 - SDRAM,NOR */
    {{0x5,  2}, LPC18XX_IOMUX_EMC_CONFIG(2)},
    /* P5.3 = D15 - SDRAM,NOR */
    {{0x5,  3}, LPC18XX_IOMUX_EMC_CONFIG(2)},

    /*
     * Configuration for EMC pins used only for SDRAM
     */
    /* P6.11 = CKE - SDRAM */
    {{0x6, 11}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P6.9 = CS# (nDYCS0) - SDRAM */
    {{0x6, 9}, LPC18XX_IOMUX_EMC_CONFIG(3)},

    /* P6.5 = RAS# - SDRAM */
    {{0x6, 5}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P6.4 = CAS# - SDRAM */
    {{0x6, 4}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P6.12 = DQM0 - SDRAM */
    {{0x6, 12}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P6.10 = DQM1 - SDRAM */
    {{0x6, 10}, LPC18XX_IOMUX_EMC_CONFIG(3)},

    /* P2.9 = A0 - SDRAM */
    {{0x2, 9}, LPC18XX_IOMUX_EMC_CONFIG(3)},

    /* P3.3 = SPIFI_SCK */
    {{0x3, 3}, LPC18XX_IOMUX_EMC_CONFIG(3)},
    /* P3.4 = SPIFI_SIO3 */
    {{0x3, 4}, LPC18XX_IOMUX_SPIFI_CONFIG(3)},
    /* P3.5 = SPIFI_SIO2 */
    {{0x3, 5}, LPC18XX_IOMUX_SPIFI_CONFIG(3)},
    /* P3.6 = SPIFI_MISO */
    {{0x3, 6}, LPC18XX_IOMUX_SPIFI_CONFIG(3)},
    /* P3.6 = SPIFI_MOSI */
    {{0x3, 7}, LPC18XX_IOMUX_SPIFI_CONFIG(3)},
    /* P3.6 = SPIFI_CS */
    {{0x3, 8}, LPC18XX_IOMUX_CONFIG(3, 0, 1, 0, 0, 0)},
};

static
const pinClockConfigEmcLpc4337_t emcClockConfig[] =
{
    /*
     * To use 16-bit wide and 32-bit wide SDRAM interfaces, select
     * the EMC_CLK function and enable the input buffer (EZI = 1)
     * in all four SFSCLKn registers in the SCU.
     */
    /* Imaginary P-0x18.0 = CLK (CLK0) - SDRAM */
    {CLK0, LPC18XX_IOMUX_EMC_CONFIG(0)},
    /* Imaginary P-0x18.1 = CLK1 - SDRAM */
    {CLK1, LPC18XX_IOMUX_EMC_CONFIG(0)},
    /* Imaginary P-0x18.2 = CLK2 - SDRAM */
    {CLK2, LPC18XX_IOMUX_EMC_CONFIG(0)},
    /* Imaginary P-0x18.3 = CLK3 - SDRAM */
    {CLK3, LPC18XX_IOMUX_EMC_CONFIG(0)},
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
emc_pinInit(void)
{
    /*
     * Set SDRAM clock output delay to ~3.5ns (0x7777),
     * the SDRAM chip does not work otherwise.
     */
    LPC_SCU->EMCDELAYCLK = 0x7777;

    /*
     * Enable EMC
     */
    LPC_EMC->CONTROL = LPC_EMC_CTRL_EN_MSK;

    /*
     * Little-endian mode
     */
    LPC_EMC->CONFIG = LPC_EMC_CONF_LITTLE_ENDIAN_MSK;

    /*
     * Initializing general pins
     */
    emcMap_t i;
    for (i=WE; i <= SPIFI_CS; i++)
    {
        Chip_SCU_PinMuxSet(
            (uint8_t) emcPinsConfig[i].pinName.port,
            (uint8_t) emcPinsConfig[i].pinName.pin,
            (uint16_t) emcPinsConfig[i].func
            );
    }

    /*
     * Initializing clock pins
     */
    emcClocks_t j;
    for (j=CLK0; j <= CLK3; j++)
    {
        Chip_SCU_ClockPinMuxSet(j, (uint16_t) emcClockConfig[j].func);
    }
}

void
emc_dramInit(void)
{
    /*
     * EMC_CLK_DIV = M4_CLK / 2
     *
     * (M4_CLK/1 is not supported)
     */
    LPC_CCU1->CLKCCU[CLK_MX_EMC_DIV].CFG |=
        (LPC_CCU1_CLK_RUN_MSK | LPC_CCU1_CLK_EMCDIV_CFG_DIV2);
    /*
     * If the EMC clock EMC_CCLK is configured for 1/2 == BASE_M4_CLK, the
     * CLK_M4_EMC_DIV branch clock must be configured for half-frequency clock
     * operation in both the CREG6 register and the CCU1 CLK_EMCDIV_CFG
     * register.
     */
    LPC_CREG->CREG6 |= LPC_CREG_CREG6_EMCCLKSEL_MSK;

    /* Enable EMC Clock */
    LPC_CCU1->CLKCCU[CLK_MX_EMC].CFG |= LPC_CCU1_CLK_RUN_MSK;

    /*
     * Wait to CLK to run
     */
    while (!(LPC_CCU1->CLKCCU[CLK_MX_EMC_DIV].STAT & 1))
        ;

    /*
     * Address mapping
     */
    LPC_EMC->DYNAMICCONFIG0 = (LPC_EMC_AM << LPC_EMC_DYCFG_AM_BITS);

    /*
     * Configure DRAM timing
     */
    LPC_EMC->DYNAMICRASCAS0 =
        (SDRAM_RAS << LPC_EMC_DYRASCAS_RAS_BITS) |
        (SDRAM_CAS << LPC_EMC_DYRASCAS_CAS_BITS);

    /*Configures the dynamic memory read strategy.*/
    LPC_EMC->DYNAMICREADCONFIG =
        (SDRAM_RDCFG_RD << LPC_EMC_DYRDCFG_RD_BITS);

    /* Dynamic Memory Precharge Command Period register */
    LPC_EMC->DYNAMICRP = SDRAM_T_RP - 1;
    /* Dynamic Memory Active to Precharge Command Period register */
    LPC_EMC->DYNAMICRAS = SDRAM_T_RAS - 1;
    /* Dynamic Memory Self-refresh Exit Time register */
    LPC_EMC->DYNAMICSREX = SDRAM_T_SREX - 1;
    /* Dynamic Memory Last Data Out to Active Time register */
    LPC_EMC->DYNAMICAPR = SDRAM_T_APR - 1;
    /* Dynamic Memory Data-in to Active Command Time register */
    LPC_EMC->DYNAMICDAL = SDRAM_T_DAL;
    /* Dynamic Memory Write Recovery Time register */
    LPC_EMC->DYNAMICWR = SDRAM_T_WR - 1;
    /* Dynamic Memory Active to Active Command Period register */
    LPC_EMC->DYNAMICRC = SDRAM_T_RC - 1;
    /* Dynamic Memory Auto-refresh Period register */
    LPC_EMC->DYNAMICRFC = SDRAM_T_RFC - 1;
    /* Dynamic Memory Exit Self-refresh register */
    LPC_EMC->DYNAMICXSR = SDRAM_T_XSR - 1;
    /* Dynamic Memory Active Bank A to Active Bank B Time register */
    LPC_EMC->DYNAMICRRD = SDRAM_T_RRD - 1;
    /* Dynamic Memory Load Mode register to Active Command Time */
    LPC_EMC->DYNAMICMRD = SDRAM_T_MRD - 1;

    /*--------------------------*/
    /* Optional delay of 100ms	*/
    /*--------------------------*/

    /*
     * Issue SDRAM NOP (no operation) command
     */
    LPC_EMC->DYNAMICCONTROL =
        LPC_EMC_DYCTRL_CE_MSK | LPC_EMC_DYCTRL_CS_MSK |
        (LPC_EMC_DYCTRL_I_NOP << LPC_EMC_DYCTRL_I_BITS);

    /*--------------------------*/
    /* Optional delay of 200ms	*/
    /*--------------------------*/

    /*
     * Pre-charge all with fast refresh
     */
    LPC_EMC->DYNAMICCONTROL =
        LPC_EMC_DYCTRL_CE_MSK | LPC_EMC_DYCTRL_CS_MSK |
        (LPC_EMC_DYCTRL_I_PALL << LPC_EMC_DYCTRL_I_BITS);
    LPC_EMC->DYNAMICREFRESH = SDRAM_REFRESH_FAST;

    /*--------------------------*/
    /* Optional delay of 1ms	*/
    /*--------------------------*/

    /*
     * Set refresh period
     */
    LPC_EMC->DYNAMICREFRESH = SDRAM_REFRESH;

    /*
     * Load mode register
     */
    /*
     * Original code had the following after the execution
     * of the command:
     * tmp32 = *(volatile u32 *)(CONFIG_SYS_RAM_BASE |
     * (SDRAM_MODEREG_VALUE << LPC18XX_EMC_MODEREG_ADDR_SHIFT));
     * */
    LPC_EMC->DYNAMICCONTROL =
        LPC_EMC_DYCTRL_CE_MSK | LPC_EMC_DYCTRL_CS_MSK |
        (LPC_EMC_DYCTRL_I_MODE << LPC_EMC_DYCTRL_I_BITS);

    /*
     * Normal mode
     */
    LPC_EMC->DYNAMICCONTROL =
        (LPC_EMC_DYCTRL_I_NORMAL << LPC_EMC_DYCTRL_I_BITS);

    /*
     * Enable DRAM buffer
     */
    LPC_EMC->DYNAMICCONFIG0 =
        (LPC_EMC_AM << LPC_EMC_DYCFG_AM_BITS) |
        LPC_EMC_DYCFG_B_MSK;
}

bool
emc_testRAM(uint32_t baseAddress, uint32_t size)
{
    int i = 0;
    uint32_t fbytes = size, *addr = baseAddress;

    /* Must be 32-bit algined */
    if ((((uint32_t) addr & 0x3) != 0) || ((fbytes & 0x3) != 0))
    {
        return false;
    }

    /* Write walking 0 pattern */
    while (fbytes > 0)
    {
        *addr = ~(1 << i);

        addr++;
        fbytes -= 4;
        i++;
        if (i >= 32)
        {
            i = 0;
        }
    }

    /* Verify walking 0 pattern */
    i = 0;
    fbytes = size;
    addr = baseAddress;
    while (fbytes > 0)
    {
        if (*addr != ~(1 << i))
        {
            return false;
        }

        addr++;
        fbytes -= 4;
        i++;
        if (i >= 32)
        {
            i = 0;
        }
    }

    return true;
}

/* ------------------------------ End of file ------------------------------ */
