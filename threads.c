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


//updates the bars and returns to bottom of screen
void Floor_Thread(void){
    while(1){

        ST7789_DrawRectangle(0, 280, 240, 20, ST7789_RED);
        ST7789_DrawRectangle(ball_x_position, ball_y_position, 20, 20, ST7789_BLACK);
        ST7789_DrawRectangle(0, floor_one_y, 240 , 10, ST7789_BLACK);
        ST7789_DrawRectangle(0, floor_two_y, 240 , 10, ST7789_BLACK);
        ST7789_DrawRectangle(0, floor_three_y, 240 , 10, ST7789_BLACK);

        ball_x_position += boost * ball_x_offset;

        G8RTOS_WaitSemaphore(&sem_Render);
        floor_one_y += 1 * game_time;
       if (floor_one_y >= 310){
           floor_one_y = 20;
       }

       floor_two_y += 1 * game_time;
       if (floor_two_y >= 310){
           floor_two_y = 20;
       }

       floor_three_y += 1  * game_time;
       if (floor_three_y >= 310){
           floor_three_y = 20;
       }
       G8RTOS_SignalSemaphore(&sem_Render);
        ball_y_position += ball_y_offset;
        if (ball_y_position == 310){
            ball_y_position = 20;
        }


     //white row
     // G8RTOS_WaitSemaphore(&sem_SPIA);
     //ST7789_DrawRectangle(0, floor_three_y, FloorThree, 10, 0xFFFF);
     //G8RTOS_SignalSemaphore(&sem_SPIA);
     //G8RTOS_WaitSemaphore(&sem_SPIA);
    // ST7789_DrawRectangle(FloorThree + 40 , floor_three_y, 200 - FloorThree, 10, 0xFFFF);
     //G8RTOS_SignalSemaphore(&sem_SPIA);


     //red row
     //ST7789_DrawRectangle(0, floor_one_y, FloorOne, 10, ST7789_RED);
     //ST7789_DrawRectangle(FloorOne + 40 , floor_one_y, 200 - FloorOne, 10, ST7789_RED);

    //blue row
    //ST7789_DrawRectangle(0, floor_two_y, FloorTwo, 10, ST7789_BLUE);
    //ST7789_DrawRectangle(FloorTwo + 40 , floor_two_y, 200 - FloorTwo, 10, ST7789_BLUE);


   G8RTOS_WaitSemaphore(&sem_SPIA);
  // ST7789_DrawRectangle(ball_x_position, ball_y_position, 20, 20, 0xFFFF);
   G8RTOS_SignalSemaphore(&sem_SPIA);


   sleep(20);
   }
}


void Draw_Thread(void){
    while(1){



    ST7789_UpdateScreen(gameNew, gamePrev);
    sleep(20);


    }


}

void Init_GameData(void){
    gameNew.ball.x = 100;
    gameNew.ball.y = 271;

    gameNew.row1.x = 0;
    gameNew.row1.y = 256;

    gameNew.row2.x = 0;
    gameNew.row2.y = 192;
    gameNew.row2.x = 0;

    gameNew.row3.y = 128;
    gameNew.row3.x = 0;

    gameNew.row4.y = 64;
    gameNew.row4.x = 0;

    gameNew.row5.x = 0;
    gameNew.row5.y = 0;

    gameNew.gameTime = 0;
    gameNew.level = 1;
}
void Increment_Thread(void){



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
    if(gameNew.row1.y++ >= 310){
        gameNew.row1.y =0;
    }

    if(gameNew.row2.y++ >= 310){
        gameNew.row2.y =0;
     }

        if(gameNew.row3.y++ >= 310){
            gameNew.row3.y =0;
     }

    if(gameNew.row4.y++ >= 310){
        gameNew.row4.y =0;
     }

    if(gameNew.row5.y++ >= 310){
       gameNew.row5.y = 0;
    }
    gameNew.gameTime++;
    gameNew.level = 1;




}



//void move_Thread(void){
//    while(1) {
//
//            // Get result from joystick
//            //check if ball position is not touching the rows
//            G8RTOS_WaitSemaphore(&sem_Render);
//            if (jump == 3){
//                ball_y_offset = 8;
//                jump--;
//            }
//            else if (jump == 2){
//                ball_y_offset = 7;
//                jump--;
//            }
//            else if (jump == 1){
//                ball_y_offset = 5;
//                jump--;
//            }
//                //check to see if ball is on floor 2
//            else if (ball_y_position - floor_two_y  <= 12 &&
//                       ball_y_position - floor_two_y + 10 >= 0){
//
//                   //check to see if it is over gap
////                   //if(ball_x_position >= FloorTwo && ball_x_position <= FloorTwo + 40){
//                       //ball_y_offset = -2;
//                   //}
////                   //ball is rising if not over gap
////                   else{
////                           ball_y_offset = 1;
////                           ball_y_position = floor_two_y + 10;
////                    }
//
//
//               }
//               //check to see if ball is on floor 3
//               else if(ball_y_position - floor_three_y  <= 12 &&
//                   ball_y_position - floor_three_y  >= 0){
//                   //check to see if ball is over gap (make it fall)
//                   if(ball_x_position >= FloorThree && ball_x_position <= FloorThree + 40){
//                         ball_y_offset = -2;
//                    }
//                     //if the ball is not over the gap the to see if jump is high
//                     else{
//
//                         if(jump  == 3){
//                               jump--;
//                               ball_y_offset = 5;
//                           }
//
//                         else{
//                         ball_y_offset = 1;
//                         ball_y_position = floor_three_y + 10;
//                         }
//                     }
//    }
//               //the ball is falling
//               else{
//                   ball_y_offset = -2;
//               }
//            uint32_t joy_val = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
//            uint16_t x_val = joy_val >> 16; // x left is max x right is min need to fix
//            uint16_t y_val = joy_val;
//            // If joystick axis within deadzone, set to 0. Otherwise normalize it.
//            float normalized_y_Value = ((float) y_val - 2048) / ((float) 2048);
//            float normalized_x_Value = (((float) x_val - 2048) / 2048);
//
//            if(normalized_x_Value < 0.2 && normalized_x_Value > -0.2 ){
//                normalized_x_Value = 0;
//            }
//
//
//            //world_camera_pos.x = x_val;
//            normalized_x_Value *= -1;
//
//            // Update world camera position. Update y/z coordinates depending on the joystick toggle.
//
//            if(normalized_x_Value > 0) ball_x_offset = 2;
//            else if (normalized_x_Value < 0) ball_x_offset = -2;
//            else ball_x_offset = 0;
//            G8RTOS_SignalSemaphore(&sem_Render);
//            // sleep
//            sleep(10);
//
//        }
//}
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
