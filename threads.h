// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define SPAWNCOOR_FIFO      0
#define JOYSTICK_FIFO       1

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_I2CA;
semaphore_t sem_UART;
semaphore_t sem_Render;

semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;
semaphore_t sem_KillCube;
semaphore_t sem_Camera;

// Stores
typedef struct gameData_t {
    uint;
    struct ptcb_t *previousPTCB;
    struct ptcb_t *nextPTCB;
    uint32_t period;
    uint32_t executeTime;
    uint32_t currentTime;
} gameData_t ;

typedef struct postion {
    uint8_t x;

} ptcb_t;


/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/


/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Lose(void);
void Enemy(void);
void Ball(void);
void Spaceship_Thread(void);
void move_Thread(void);
void Cube_Thread(void);
void CamMove_Thread(void);
void Read_Buttons(void);
void Read_JoystickPress(void);
void Floor_Thread(void);
void Update_Ball(void);
void Increment_Thread(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Print_WorldCoords(void);
void Get_Joystick(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler(void);
void GPIOD_Handler(void);

/*******************************Aperiodic Threads***********************************/


#endif /* THREADS_H_ */

