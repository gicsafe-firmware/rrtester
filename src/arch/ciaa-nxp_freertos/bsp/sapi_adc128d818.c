/* Copyright 2019, Gustavo Ramoscelli.
 * Copyright 2019, Alejandro Permingeat.
 * Copyright 2016, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Date: 2019-02-04
 */

/*==================[inclusions]=============================================*/

#include "sapi_adc128d818.h"
#include "sapi.h"            /* <= sAPI header */
/*==================[macros and definitions]=================================*/

#define MAX_ITER_NOT_READY 10
#define ADC128D818_I2C_SPEED	100000 /*100 KHz*/

/*==================[internal data declaration]==============================*/

const int8_t ADC128D818_ADDR[] = {0x1d, 0x1e, 0x1f, 0x2d, 0x2e, 0x2f, 0x35, 0x36, 0x37};
double my_ref_voltage[] = {2.56f, 2.56f, 2.56f, 2.56f, 2.56f, 2.56f, 2.56f, 2.56f, 2.56f};
static uint8_t i2cInitialized = FALSE;

/*==================[internal functions declaration]=========================*/

char *intToHex(uint64_t value, uint8_t digits);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
void log(char *str);
void logError(char *str);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

// private methods
//

/*
From sapi_i2c.h: 

bool_t i2cWrite( i2cMap_t  i2cNumber,
                 uint8_t  i2cSlaveAddress,
                 uint8_t* transmitDataBuffer,
                 uint16_t transmitDataBufferSize,
                 bool_t   sendWriteStop );
bool_t i2cRead( i2cMap_t  i2cNumber,
                uint8_t  i2cSlaveAddress,
                uint8_t* dataToReadBuffer,
                uint16_t dataToReadBufferSize,
                bool_t   sendWriteStop,
                uint8_t* receiveDataBuffer,
                uint16_t receiveDataBufferSize,
                bool_t   sendReadStop );

*/

bool_t ADC128D818_setRegisterAddress(uint8_t address, uint8_t reg_addr) 
{
    return i2cWrite(I2C0, address, &reg_addr, 1, 1);
}

/** @brief read the channel in "raw" format
*	@return none
*/
bool_t ADC128D818_setRegister(uint8_t address, uint8_t reg_addr, uint8_t value) 
{
    uint8_t data[2];
    data[0] = reg_addr;
    data[1] = value;
    return i2cWrite(I2C0, address, data, 2, 1);
}

/** @brief read the channel in "raw" format
*	@return none
*/

/** @brief read the channel in "raw" format
*	@return none
*/
uint8_t ADC128D818_readRegister(uint8_t address, uint8_t reg_addr) 
{
    uint8_t val;
    i2cRead(I2C0, address, &reg_addr, 1, 0, &val, 1, 1); 
    return val;
}

uint16_t ADC128D818_readChannel(uint8_t address, uint8_t channel)
{
    
    uint8_t buff[2];
    uint8_t adr_channel = ADC128D818_REG_Channel_Readings_Registers + channel;
    uint16_t result;
    
    if (channel>7) { 
        return 0;
    }
    
    i2cRead(I2C0, address, &adr_channel, 1, 0, buff, 2, 1);
    
    result = buff[0];
    result = (result << 8) | buff[1];
    return (result >> 4);
}

/** @brief add new ADC128D818 device
*	@return TRUE = Success
*/
bool_t ADC128D818_init(ADC128D818_ADDRESS address,                    
                       ADC128D818_OPERATION_MODE op_mode,
                       ADC128D818_RATE rate,
                       ADC128D818_VREF ref_mode,
                       double ref_voltage,
                       ADC128D818_ENABLE enabled_mask)
{
    /* HINT:
    *
    * Quick Start
    * 1. Power on the device, then wait for at least 33ms.
    * 2. Read the Busy Status Register (address 0Ch). If the 'Not Ready' bit = 1, then increase the wait time until 'Not Ready' bit = 0 before proceeding to the next step.
    * 3. Program the Advanced Configuration Register -- Address 0Bh:
    * - a. Choose to use the internal or external VREF (bit 0).
    * - b. Choose the mode of operation (bits [2:1]).
    * 4. Program the Conversion Rate Register (address 07h).
    * 5. Choose to enable or disable the channels using the Channel Disable Register (address 08h).
    * 6. Using the Interrupt Mask Register (address 03h), choose to mask or not to mask the interrupt status from propagating to the interrupt output pin, INT.
    * 7. Program the Limit Registers (addresses 2Ah - 39h).
    * 8. Set the 'START' bit of the Configuration Register (address 00h, bit 0) to 1.
    * 9. Set the 'INT_Clear' bit (address 00h, bit 3) to 0. If needed, program the 'INT_Enable' bit (address 00h, bit 1) to 1 to enable the INT output.
    */
	if (FALSE == i2cInitialized)
	{
		i2cConfig(I2C0, ADC128D818_I2C_SPEED);
		i2cInitialized = TRUE;
	}
	delayInaccurate(35);

    uint8_t busy_reg = 0x00;
    int cont = 0;
    do {
        cont++;
        if (cont >= MAX_ITER_NOT_READY) {
            return FALSE; 
        }
        delayInaccurate(35);
        busy_reg = ADC128D818_readRegister(address, ADC128D818_REG_Busy_Status_Register);

    } while (busy_reg&( ADC128D818_STATUS_NOT_READY_BIT ));

    /* reset chip to default */
    ADC128D818_setRegister(address, ADC128D818_REG_Configuration_Register, 0x80);

    /** Setting Advanced Configuration Register */
    ADC128D818_setRegister(address, ADC128D818_REG_Advanced_Configuration_Register, ref_mode | (op_mode << 1));


    /** program conversion rate register */
    ADC128D818_setRegister(address, ADC128D818_REG_Conversion_Rate_Register, rate);

    /** program enabled channels */
    ADC128D818_setRegister(address, ADC128D818_REG_Channel_Disable_Register, enabled_mask);

    // program limit regs
    // currently noop!

    /* set start bit in configuration (interrupts disabled) */
    ADC128D818_setRegister(address, ADC128D818_REG_Configuration_Register, 1);

    return TRUE;
}

