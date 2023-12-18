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

#define GYRO_FIFO           0
#define JOYSTICK_FIFO       1

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_I2CA;
semaphore_t sem_UART;
semaphore_t sem_Render;

semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;
semaphore_t sem_Joystick;
semaphore_t sem_Pause;
semaphore_t sem_GameOver;


// Stores





/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/


/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Lose(void);


void CamMove_Thread(void);
void Read_Buttons(void);
void Read_JoystickPress(void);
void Floor_Thread(void);
void Update_Ball(void);
void Increment_Thread(void);
void Init_GameData(void); //helper function
void Draw_Thread(void);
void Convert_Joystick(void);
void Read_Gyroscope(void);
void Menu_Thread(void);
void Draw_Title(uint16_t color); //helper function
void Draw_Play(uint16_t color);
void Draw_Settings(uint16_t color);
void Draw_Credits(uint16_t color);
void Lose_Thread(void);
void Pause_Thread(void);

void Draw_Button(enum BUTTON button, uint16_t color);
void Init_Screen(void);
/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Print_WorldCoords(void);
void Get_JoystickX(void);
void Get_JoystickY(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler(void);
void GPIOD_Handler(void);

/*******************************Aperiodic Threads***********************************/


#endif /* THREADS_H_ */

;
