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
    ST7789_DrawRectangle(0, 300, 240, 20, ST7789_RED);




    G8RTOS_InitFIFO(JOYSTICK_FIFO);

    //Idle Thread
    G8RTOS_AddThread(Idle_Thread, 254, "idle", globalID++);
    //Background Threads


    //G8RTOS_AddThread(Spaceship_Thread, 210, "square", globalID++);

    G8RTOS_AddThread(move_Thread, 100, "move", globalID++);
    G8RTOS_AddThread(Floor_Thread, 99, "floor", globalID++);




    //Aperiodic Threads
    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 5, 20);
    //Periodic Threads
    G8RTOS_Add_PeriodicEvent(Get_Joystick, 10, 2);
    G8RTOS_Add_PeriodicEvent(Increment_Thread, 10000, 1000);
   // G8RTOS_Add_PeriodicEvent(Update_Floor, 100, 3);





    //Semaphores
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 1);
    G8RTOS_InitSemaphore(&sem_Render, 1);

    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 1);
    G8RTOS_InitSemaphore(&sem_UART, 1);

    G8RTOS_AddThread(Read_Buttons, 101, "buttons\0", globalID++);



    // Add threads, semaphores, here
    G8RTOS_Launch();
    while (1);
}

/************************************MAIN*******************************************/
;
;
