// RTOS Projcet
// Created: 2023-12-01
// Updated: 2023-12-07



/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"
#include "driverlib/timer.h"
#include "./threads.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_nvic.h"
#include <time.h>






threadID_t globalID = 0;


int t0count  = 0;
int t1count =1;


/************************************MAIN*******************************************/
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();

    multimod_init();
    //set background
    ST7789_Fill(ST7789_BLACK);

    //sets the current position and velocity of game objects
    Init_GameData();
    Init_Screen();

    //Background Threads
    G8RTOS_AddThread(Idle_Thread, 254, "idle", 99);
    G8RTOS_AddThread(Read_Buttons, 151, "buttons\0", 9);
    G8RTOS_AddThread(Menu_Thread, 150, "buttons\0", 10);


    //Aperiodic Threads
    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 5, 20);


    //Periodic Threads
    G8RTOS_Add_PeriodicEvent(Get_JoystickY, 20, 1);


    //Semaphores
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_Render, 1);
    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_Joystick, 1);
    G8RTOS_InitSemaphore(&sem_Pause, 0);
    G8RTOS_InitSemaphore(&sem_GameOver, 0);


    //FIFO / Interprocess communication
    G8RTOS_InitFIFO(JOYSTICK_FIFO);
    G8RTOS_InitFIFO(GYRO_FIFO);

    G8RTOS_Launch();

    while (1);
}

/************************************MAIN*******************************************/
;
