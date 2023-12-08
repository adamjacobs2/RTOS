// multimod_i2c.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for I2C functions

/************************************Includes***************************************/

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// I2C_Init
// Initializes specified I2C module
// Param uint32_t "mod": base address of module
// Return: void
void I2C_Init(uint32_t mod) {
    // Note:    The multimod board uses multiple I2C modules
//              to communicate with different devices. You can use
    //          the 'mod' parameter to choose which module to initialize
    //          and use.
    // mod n -> I2Cn
    // Enable clock to relevant I2C and GPIO modules


    // Configure pins for I2C module
    if (mod == I2C_B_BASE){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
        // Wait for the I2C0 module to be ready
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
        GPIOPinConfigure(GPIO_PB2_I2C0SCL);
        GPIOPinConfigure(GPIO_PB3_I2C0SDA);

        GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
        GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
        // Configure I2C SCL speed, set as master- same as the system clock
        I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
    }
    else if (mod == I2C_A_BASE){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
        // Wait for the I2C0 module to be ready
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));
        GPIOPinConfigure(GPIO_PA6_I2C1SCL);
        GPIOPinConfigure(GPIO_PA7_I2C1SDA);
        GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
        GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
        // Configure I2C SCL speed, set as master- same as the system clock
        I2CMasterInitExpClk(I2C_A_BASE, SysCtlClockGet(), true);
    }


}

// I2C_WriteSingle
// Writes a single byte to an address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t "byte": byte to send
// Return: void
void I2C_WriteSingle(uint32_t mod, uint8_t addr, uint8_t byte) {
    // Set the address in the slave address register (shifting handled)
    I2CMasterSlaveAddrSet(mod, addr, false);
    // Input data into I2C module
    I2CMasterDataPut(mod, byte);
    // Trigger I2C module send (start condition)
    I2CMasterControl(mod, I2C_MASTER_CMD_SINGLE_SEND);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));

    return;
}

void I2C_WriteSingleCont(uint32_t mod, uint8_t addr, uint8_t byte) {
    // Set the address in the slave address register (shifting handled)
    I2CMasterSlaveAddrSet(mod, addr, false);
    // Input data into I2C module
    I2CMasterDataPut(mod, byte);
    // Trigger I2C module send (start condition)
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));

    return;
}



// I2C_ReadSingle
// Reads a single byte from address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Return: uint8_t
uint8_t I2C_ReadSingle(uint32_t mod, uint8_t addr) {
    // Set the device address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, true);
    // Trigger I2C module receive
    I2CMasterControl (mod, I2C_MASTER_CMD_SINGLE_RECEIVE);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));
    // Return received data
    return I2CMasterDataGet(mod);
}

// I2C_WriteMultiple
// Write multiple bytes to a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to transmit
// Return: void
void I2C_WriteMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    int i = 1;
    // Set the address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, false);
    // Input First byte into I2C module
    I2CMasterDataPut(mod, data[0]);
    num_bytes--;
    // Trigger I2C module send and take control of the bus
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));

    while(num_bytes > 1){
        // Input next byte into I2C module
        I2CMasterDataPut(mod, data[i++]);
        // Trigger I2C module send while sill in control of the bus
        I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_CONT);
        // Wait until I2C module is no longer busy
        while(I2CMasterBusy(mod));
        num_bytes--;
    }

    // Input last byte into I2C module
    I2CMasterDataPut(mod, data[i]);
    // Trigger final I2C module send and send stop condition
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));

    return;
}

// I2C_ReadMultiple
// Read multiple bytes from a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void I2C_ReadMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // Set the device address in the slave address register
    I2CMasterBurstLengthSet(mod, num_bytes);
    I2CMasterSlaveAddrSet(mod, addr, true);
    // Trigger I2C module receive
    I2CMasterControl (mod, I2C_MASTER_CMD_BURST_RECEIVE_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));
    // store received data in arr
    data[0] = I2CMasterDataGet(mod);
    num_bytes--;
    // While num_bytes > 1
    int i = 1;
    while(num_bytes > 1){
        // Trigger I2C module receive
        I2CMasterControl (mod, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        // Wait until I2C module is no longer busy
        while(I2CMasterBusy(mod));
        // Read received data
        data[i++]= I2CMasterDataGet(mod);
        num_bytes--;
    }

    // Trigger I2C module receive
    I2CMasterControl (mod, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod));
    // Read received data
    data[i++]= I2CMasterDataGet(mod);
    num_bytes--;
    return;
}

/********************************Public Functions***********************************/

