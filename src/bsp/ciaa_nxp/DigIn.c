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
 *  \file   DigIn.c
 *  \brief  Digital Input module implementation.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  CaMa  Carlos Mancón     manconci@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/*  There are 6 usable GPIOs in P12 connector. Two of them share functions
 * with other peripherals and can't be used. In the constants section, the
 * signals for each pin are defined.
 */

/* ----------------------------- Include files ----------------------------- */
#include "DigIn.h"
#include "sapi_gpio.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
typedef enum
{
    /* P12 header */
    PHY_nRESET, FAILURE, CLKx6, SPI_CS, CLK, CLKx3,
} digInMap_t;

static
const pinConfigGpioLpc4337_t digInPinsConfig[] =
{
    /*                             # | NAME  | CONN | RRTESTER	*/
    /*----------------------------------------------------------*/
    {{6, 1}, FUNC0, {3, 0}},    /* 0 | GPIO0 | P12  | PHY_nRESET*/
    {{2, 5}, FUNC4, {5, 5}},    /* 1 | GPIO1 | P12  | FAILURE	*/
    {{7, 0}, FUNC0, {3, 8}},    /* 2 | GPIO2 | P12  | CLKx6		*/
    {{6, 7}, FUNC4, {5,15}},    /* 3 | GPIO3 | P12  | SPI_CS	*/
    {{6, 3}, FUNC0, {3, 2}},    /* 4 | GPIO7 | P12  | CLK		*/
    {{6, 6}, FUNC0, {0, 5}},    /* 5 | GPIO8 | P12  | CLKx3		*/
};
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static
void
digInObtainPinConfig(digInMap_t pin, int8_t *gpioPort, int8_t *gpioPin)
{
    *gpioPort    = digInPinsConfig[pin].gpio.port;
    *gpioPin     = digInPinsConfig[pin].gpio.pin;
}

static
void
digInObtainFullPinConfig(digInMap_t pin,
                         int8_t *pinNamePort, int8_t *pinNamePin,
                         int8_t *func, int8_t *gpioPort,
                         int8_t *gpioPin)
{
    *pinNamePort = digInPinsConfig[pin].pinName.port;
    *pinNamePin  = digInPinsConfig[pin].pinName.pin;
    *func        = digInPinsConfig[pin].func;

    digInObtainPinConfig(pin, gpioPort, gpioPin);
}

static
void
digInConfigInput(digInMap_t pin)
{
    int8_t pinNamePort = 0;
    int8_t pinNamePin  = 0;

    int8_t func        = 0;

    int8_t gpioPort    = 0;
    int8_t gpioPin     = 0;

    digInObtainFullPinConfig(pin, &pinNamePort, &pinNamePin, &func,
                             &gpioPort, &gpioPin);

    Chip_SCU_PinMux(
        pinNamePort,
        pinNamePin,
        SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS,
        func
        );
    Chip_GPIO_SetDir(LPC_GPIO_PORT, gpioPort, (1 << gpioPin), GPIO_INPUT);
}

static
bool_t
digInRead(digInMap_t pin)
{
    bool_t ret_val     = OFF;

    int8_t gpioPort    = 0;
    int8_t gpioPin     = 0;

    digInObtainPinConfig(pin, &gpioPort, &gpioPin);

    ret_val = (bool_t) Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, gpioPort, gpioPin);

    return ret_val;
}

/* ---------------------------- Global functions --------------------------- */
void
DigIn_init(void)
{
    digInConfigInput(FAILURE);
    digInConfigInput(CLK);
    digInConfigInput(CLKx3);
    digInConfigInput(CLKx6);
}

DigIn
DigIn_get(void)
{
    struct DigIn ret;

    ret.clk     = digInRead(CLK)        & 1;
    ret.clkX3   = digInRead(CLKx3)      & 1;
    ret.clkX6   = digInRead(CLKx6)      & 1;
    ret.failure = digInRead(FAILURE)    & 1;

    return ret;
}
/* ------------------------------ End of file ------------------------------ */
