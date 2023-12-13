// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Shapes/inc/cube.h"
#include "./MiscFunctions/LinAlg/inc/linalg.h"
#include "./MiscFunctions/LinAlg/inc/quaternions.h"
#include "./MiscFunctions/LinAlg/inc/vect3d.h"
#include "./MultimodDrivers/multimod_ST7789.h"
#include "./G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod_ST7789.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "driverlib/systick.h"



/*********************************Global Variables**********************************/





int16_t ball_x_offset = 0;
int16_t ball_y_offset = 0;








gameData_t gameNew;
gameData_t gamePrev;







// How many cubes?
uint8_t num_cubes = 0;

// y-axis controls z or y
uint8_t joystick_y = 1;

// Kill a cube?
uint8_t kill_cube = 0;
uint8_t globalFlag = 0;

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/
void Lose(void){
    if(globalFlag){
        while(1){
            ST7789_Fill(ST7789_RED);
        }
    }
}



void Draw_Thread(void){
    while(1){

    G8RTOS_WaitSemaphore(&sem_Render);
    ST7789_UpdateScreen(gameNew, gamePrev, gameNew.rowColor);
    G8RTOS_SignalSemaphore(&sem_Render);

    }
}

void Init_GameData(void){
    gameNew.ball.x = 100;
    gameNew.ball.y = 65;

    gameNew.rows[0].x = 0;
    gameNew.rows[0].y = 256;
    gameNew.rows[0].hole = 80;

    gameNew.rows[1].x = 0;
    gameNew.rows[1].y = 192;
    gameNew.rows[1].hole = 120;

    gameNew.rows[2].y = 128;
    gameNew.rows[2].x = 0;
    gameNew.rows[2].hole = 40;

    gameNew.rows[3].y = 64;
    gameNew.rows[3].x = 0;
    gameNew.rows[3].hole = 120;

    gameNew.rows[4].x = 0;
    gameNew.rows[4].y = 0;
    gameNew.rows[4].hole = 40;

    gameNew.gameScore = 0;
    gameNew.rowColor = ST7789_GREEN;
    gameNew.gameTime = 0;
    gameNew.level = 1;

    gameNew.velocity = 1;
}

void Increment_Thread(void){
    while(1){
        G8RTOS_WaitSemaphore(&sem_Render);

        uint16_t x_val = G8RTOS_ReadFIFO(JOYSTICK_FIFO);

        // If joystick axis within deadzone, set to 0. Otherwise normalize it.
        if(x_val < 2448 && x_val > 1648 ){
            x_val = 2048;
        }
        //Joystick Parsing
        if(x_val > 2048) ball_x_offset =  -3;
        else if (x_val < 2048) ball_x_offset = 3;
        else ball_x_offset = 0;

        //save the current state of the game in the previoius game structure
        gamePrev = gameNew;

        //update the X position ofthe ball with the joystick data
        gameNew.ball.x += ball_x_offset;



        //move the horizontal rows depending on the level
        for(uint8_t i = 0; i < 5; i++){
            gameNew.rows[i].y += gamePrev.level;
            if(gameNew.rows[i].y >= 310){
                gameNew.rows[i].hole = rand() % (200 - 40 + 1) + 40;
                gameNew.rows[i].y =0;
            }
        }

        //If the ball is already on a surface, make sure it's not over the gap
        if (gamePrev.velocity == gamePrev.level){ //level updates for next game state
            for(uint8_t i = 0; i < 5; i++){
                if(gamePrev.ball.y == gameNew.rows[i].y){
                    if(gameNew.ball.x >= gameNew.rows[i].hole && gameNew.ball.x <= gameNew.rows[i].hole + 25){
                        gameNew.velocity = gameNew.level* -2;
                        gameNew.gameScore++;
                    }
                    break;
                }
            }

        }
        //detect collisions when the ball is in free fall
        else if(gamePrev.velocity != gamePrev.level){
            //check to see if the ball would have fell through the floor between states
            for(uint8_t i = 0; i < 5; i++){
                if(gamePrev.ball.y + 1 >= gamePrev.rows[i].y && gameNew.ball.y <= gameNew.rows[i].y){
                    if(gameNew.ball.x >= gamePrev.rows[i].hole && gameNew.ball.x <= gameNew.rows[i].hole + 25){
                        gameNew.velocity = -2;
                        gameNew.gameScore++;
                    }
                    else{
                        gameNew.velocity = gamePrev.level;
                        gameNew.ball.y = gameNew.rows[i].y;
                    }
                    break;
                 }
            }
        }


        gameNew.ball.y += gameNew.velocity;
        if(gameNew.ball.y <= 5) gameNew.ball.y = 10;
        if(gameNew.ball.y >= 295) gameNew.ball.y = 295;
        gameNew.gameTime++;
        if(gameNew.gameTime == 1000) {
            gameNew.level += 1;
            gameNew.rowColor = 0b0000011111111111;
        }

        else if(gameNew.gameTime == 2000) {
                gameNew.level += 1;
                gameNew.rowColor = ST7789_RED;
         }


        G8RTOS_SignalSemaphore(&sem_Render);
        sleep(2);

    }





}

// Thread1, reads gyro_x data, adjusts RED led duty cycle.
void Read_Gyroscope(void) {
    int16_t gyro;

    G8RTOS_WaitSemaphore(&sem_I2CA);
    gyro = (int16_t) ((float) BMI160_GyroXGetResult());

    G8RTOS_WriteFIFO(GYRO_FIFO, gyro);

    G8RTOS_SignalSemaphore(&sem_I2CA);


}




void Idle_Thread(void) {
    time_t t;
    srand((unsigned) time(&t));
    while(1);
}


void Read_Buttons() {
    // Initialize / declare any variables here
    uint8_t buttons;

    while(1) {
        // Wait for a signal to read the buttons on the Multimod board.
        // Sleep to debounce
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        //for(uint32_t i = 0 ; i < 5000000; i++);
        sleep(40);
        // Read the buttons status on the Multimod board.
        buttons = MultimodButtons_Get();


        if(buttons == 253){
            //SW1 is pressed
            //JUMP


//

        }
        else if(buttons == 251){

        }
        else if(buttons == 247){
            //SW3 is pressed
            UARTprintf("SW3\n");


        }
        else if(buttons == 239){
            //SW4 is pressed

            UARTprintf("SW4\n");
        }

        // Clear the interrupt
        // Re-enable the interrupt so it can occur again.

        GPIOIntClear(BUTTONS_INT_GPIO_BASE,GPIO_PIN_4);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE,GPIO_PIN_4);
        sleep(9);
    }
}



/********************************Periodic Threads***********************************/


void Get_Joystick(void) {
    // Read the joystick
    // Send through FIFO.
    uint16_t TotalXY = JOYSTICK_GetX();
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, TotalXY);


}



/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler() {
    // Disable interrupt
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE,GPIO_PIN_4);
    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);

}

void GPIOD_Handler() {
    // Disable interrupt
    GPIOIntDisable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
}
