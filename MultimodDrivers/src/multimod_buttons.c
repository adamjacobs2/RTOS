// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_buttons.h"

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init() {
    // Initialize this function & the relevant interrupt pin
    I2C_Init(I2C_A_BASE);

            // Enable relevant port for launchpad switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOInput(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    GPIOPadConfigSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t 
uint8_t MultimodButtons_Get() {
    I2C_WriteSingleCont(I2C_A_BASE, 0x23, 0);
    return I2C_ReadSingle(I2C_A_BASE, 0x23);

    // complete this function
}

