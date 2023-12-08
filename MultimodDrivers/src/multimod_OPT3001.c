// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 functions

/************************************Includes***************************************/

#include "../multimod_OPT3001.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// OPT3001_Init
// Initializes OPT3001, configures it to continuous conversion mode.
// Return: void
void OPT3001_Init(void) {
    // Initialize I2C module
    I2C_Init(I2C_A_BASE);
    // Set the correct configuration byte for continuous conversions
    //uint16_t mask = (1 << 10));
    uint16_t config = OPT3001_ReadRegister(OPT3001_CONFIG_ADDR);
    OPT3001_WriteRegister(OPT3001_CONFIG_ADDR, 0xC610);
    return;
}

// OPT3001_WriteRegister
// Writes to a register in the OPT3001.
// Param uint8_t "addr": Register address of the OPT3001.
// Param uint16_t "data": 16-bit data to write to the register.
// Return: void
void OPT3001_WriteRegister(uint8_t addr, uint16_t data) {
    //splits data into respective bytes
    uint8_t byte1 = addr; //register address within the sensor
    uint8_t byte2 = (uint8_t)((data >> 8) & 0xFF);
    uint8_t byte3 = (uint8_t)(data & 0xFF);

    uint8_t bytes[3]= {byte1, byte2, byte3};
    //First the master will send a byte container the slave adress along with R/W bit
    //Without sending a stop signal, the master transmits the 8-bit register address
    //Without sending a stop signal, the master transmits the next 2 bytes
    I2C_WriteMultiple(I2C_A_BASE, OPT3001_ADDR, bytes, 3);
    return;

}

// OPT3001_ReadRegister
// Reads from a register in the OPT3001.
// Param uint8_t "addr": Register address of the OPT3001.
// Return: uint16_t
uint16_t OPT3001_ReadRegister(uint8_t addr) {
    //start a partial write operation on the wanted register address
    uint8_t data[2];
    I2C_WriteSingleCont(I2C_A_BASE, OPT3001_ADDR, addr);
    //Now we can read from the device
    I2C_ReadMultiple(I2C_A_BASE, OPT3001_ADDR, data, 2);

    uint8_t high = data[0]  & 0xFF;
    uint8_t low = (data[1] & 0xFF);

    // Combine the high and low bytes to form the 16-bit value
    uint16_t result = ((uint16_t)(high << 8) | low);

    return result ;

}

// OPT3001_GetResult
// Gets conversion result, calculates byte result based on datasheet
// and configuration settings.
// Return: uint32_t
uint32_t OPT3001_GetResult(void) {
    // Check if data is ready first


    uint16_t result = OPT3001_ReadRegister(OPT3001_RESULT_ADDR);

    result = LUX((result >> 12 & 0xF), (result & 0x0FFF));

    return result;
}

// OPT3001_SetLowLimit
// Sets the low limit register.
// Param uint16_t "exp": Exponential bound
// Param uint16_t "result": Conversion bound
// Return: void
void OPT3001_SetLowLimit(uint16_t exp, uint16_t result) {
    OPT3001_WriteRegister(OPT3001_LOWLIMIT_ADDR, (exp << OPT3001_RESULT_E_S | (result & 0xFFF)));

    return;
}

// OPT3001_SetHighLimit
// Sets the high limit register.
// Param uint16_t "exp": Exponential bound
// Param uint16_t "result": Conversion bound
// Return: void
void OPT3001_SetHighLimit(uint16_t exp, uint16_t result) {
    OPT3001_WriteRegister(OPT3001_HIGHLIMIT_ADDR, (exp << OPT3001_RESULT_E_S | (result & 0xFFF)));

    return;
}

// OPT3001_GetChipID
// Gets the chip ID of the OPT3001.
// Return: uint16_t
uint16_t OPT3001_GetChipID(void) {
    return OPT3001_ReadRegister(OPT3001_DEVICEID_ADDR);
}

/********************************Public Functions***********************************/
