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
/* ------------------------------ End of file ------------------------------ */
