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

uint8_t jump = 0;




uint8_t boost = 6;
uint8_t game_time = 1;


int16_t ball_x_position = 100;
int16_t ball_y_position = 130;

int16_t ball_x_offset = 0;
int16_t ball_y_offset = 0;

int16_t ball_prev_x_position = 99;
int16_t ball_prev_y_position = 0;



int8_t floor_one_y = 120;
uint8_t floor_two_y = 70;
uint8_t floor_three_y = 20;



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
    ST7789_UpdateScreen(gameNew, gamePrev);
    G8RTOS_SignalSemaphore(&sem_Render);

    }
}

void Init_GameData(void){
    gameNew.ball.x = 100;
    gameNew.ball.y = 65;

    gameNew.rows[0].x = 0;
    gameNew.rows[0].y = 256;
    gameNew.rows[0].hole = 40;

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

    gameNew.gameTime = 0;
    gameNew.level = 1;

    gameNew.velocity = 1;
}

void Increment_Thread(void){
    while(1){
        G8RTOS_WaitSemaphore(&sem_Render);
        uint32_t joy_val = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
        uint16_t x_val = joy_val >> 16; // x left is max x right is min need to fix
        uint16_t y_val = joy_val;
        // If joystick axis within deadzone, set to 0. Otherwise normalize it.
        float normalized_y_Value = ((float) y_val - 2048) / ((float) 2048);
        float normalized_x_Value = (((float) x_val - 2048) / 2048);

        if(normalized_x_Value < 0.2 && normalized_x_Value > -0.2 ){
            normalized_x_Value = 0;
        }


        //world_camera_pos.x = x_val;
        normalized_x_Value *= -1;

        // Update world camera position. Update y/z coordinates depending on the joystick toggle.

        if(normalized_x_Value > 0) ball_x_offset = 2;
        else if (normalized_x_Value < 0) ball_x_offset = -2;
        else ball_x_offset = 0;



        gamePrev = gameNew;

        gameNew.ball.x += ball_x_offset;
        if(gameNew.rows[0].y++ >= 310){
            gameNew.rows[0].y =0;
        }

        if(gameNew.rows[1].y++ >= 310){
            gameNew.rows[1].y =0;
         }

        if(gameNew.rows[2].y++ >= 310){
            gameNew.rows[2].y =0;
     }

        if(gameNew.rows[3].y++ >= 310){
            gameNew.rows[3].y =0;
         }

        if(gameNew.rows[4].y++ >= 310){
           gameNew.rows[4].y = 0;
        }


        //If the ball is already on a surface, make sure it's not over the gap
        if (gamePrev.velocity == gamePrev.level){ //level updates for next game state
            //ball is in contact with rows[0]

            for(uint8_t i = 0; i < 5; i++){
                if(gameNew.ball.y - gameNew.rows[i].y == 0){
                    if(gameNew.ball.x >= gameNew.rows[i].hole && gameNew.ball.x <= gameNew.rows[i].hole + 25){
                        gameNew.velocity = -2;
                        gameNew.gameScore++;
                    }
                    break;
                }
            }

        }
        else if(gamePrev.velocity != gamePrev.level){ //the ball is currently in free fall
            //check to see if the ball would have fell through the floor between states
            for(uint8_t i = 0; i < 5; i++){
                if(gameNew.ball.y + 1 <= gameNew.rows[i].y && gameNew.ball.y + 1 >= gamePrev.rows[i].y){
                    if(gameNew.ball.x >= gameNew.rows[i].hole && gameNew.ball.x <= gameNew.rows[i].hole + 25){
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
        if(gameNew.ball.y <= 1) gameNew.ball.y = 5;
        gameNew.gameTime++;
        gameNew.level = 1;

        G8RTOS_SignalSemaphore(&sem_Render);
        sleep(2);

    }





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
            jump = 3;
        }
        else if(buttons == 247){
            //SW3 is pressed
            UARTprintf("SW3\n");
            boost *= 2;

        }
        else if(buttons == 239){
            //SW4 is pressed
            boost *= 0.5;
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
    uint32_t TotalXY = JOYSTICK_GetXY();
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
