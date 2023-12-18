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
#include "./MultimodDrivers/GFX_Library.h"
#include "./G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod_ST7789.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "driverlib/systick.h"


#define offset_4 25
#define offset_3 20



#define buttonColor 0x2eaed1



/*********************************Global Variables**********************************/

uint8_t lose = 0;
uint8_t inMenu = 1;
uint8_t inGame = 0;
int16_t ball_x_offset = 0;
int16_t ball_y_offset = 0;

uint8_t select = 0;
gameData_t gameNew;
gameData_t gamePrev;


// How many cubes?
uint8_t num_cubes = 0;

// y-axis controls z or y
uint8_t joystick_y = 1;


/*********************************Global Variables**********************************/

/*************************************Threads***************************************/




void Draw_Thread(void){
    while(1){

    G8RTOS_WaitSemaphore(&sem_Render);
    ST7789_UpdateScreen(gameNew, gamePrev, gameNew.rowColor);

    if(gameNew.gameScore > gamePrev.gameScore){
        display_drawChar(90, 280, (((gamePrev.gameScore % 100) / 10 )+'0'), 0x0, 0x0, 5);
        display_drawChar(120, 280, (gamePrev.gameScore%10)+'0', 0x0, 0x0, 5);
    }


    display_drawChar(90, 280, (((gameNew.gameScore % 100) / 10 ) +'0'), 0xFFFF, 0xFFFF, 5);
    display_drawChar(120, 280, (gameNew.gameScore%10)+'0', 0xFFFF, 0xFFFF, 5);


    G8RTOS_SignalSemaphore(&sem_Render);
    sleep(1);

    }
}

//this thread will spawn the neccesary threads to start the game, then kill itself
void Menu_Thread(void){
    uint16_t y_val;
    int8_t prevButton = PLAY;
    int8_t newButton  = PLAY;
    inMenu = 1;
    select = 0;

    //0 -- PLAY
    //1 -- SETTINGS
    //2 -- CREDITS

    while(1){
        //draw the Play Button (PLAY)
        y_val = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
        if(y_val < 2800 && y_val > 1400 ){
               y_val = 2048;
         }


        if(y_val < 2048) newButton += 1;
        else if(y_val > 2048) newButton -= 1;

        if(newButton > 3) newButton = CREDITS;
        else if(newButton < 1) newButton = PLAY;

        if(newButton !=  prevButton){
            Draw_Button(prevButton, 0xFFFF);
            Draw_Button(newButton, display_color565(0xd1, 0xae, 0x2e));
            prevButton = newButton;
            G8RTOS_InitFIFO(JOYSTICK_FIFO);
            sleep(650);
        }

        //TODO else
        //check for press
        else if(select == 1 && newButton == PLAY){
            Draw_Button(PLAY, 0);
            Draw_Button(SETTINGS, 0);
            Draw_Button(CREDITS, 0);
            Draw_Title(0);



            G8RTOS_AddThread(Draw_Thread, 250, "draw", 0);
            G8RTOS_AddThread(Increment_Thread, 249, "inc", 1);
            G8RTOS_AddThread(Read_Buttons, 101, "buttons\0", 2);
            RemovePThread();
            G8RTOS_Add_PeriodicEvent(Get_JoystickX, 25, GetSystemTime() + 5);
            G8RTOS_KillSelf();

        }


        else if(select == 1 && newButton == CREDITS){
                Draw_Button(PLAY, 0);
                Draw_Button(SETTINGS, 0);
                Draw_Button(CREDITS, 0);
                Draw_Title(0);
                ST7789_Select();
                ST7789_ClearRow(gameNew.rows[1]);
                ST7789_Deselect();


                display_drawChar(40 + 0 *(offset_3), 280, 'E', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 1 *(offset_3), 280, 'n', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 2 *(offset_3), 280, 'g', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 3 *(offset_3), 280, 'i', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 4 *(offset_3), 280, 'n', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 5 *(offset_3), 280, 'e', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 6 *(offset_3), 280, 'e', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 7 *(offset_3), 280, 'r', 0xFFFF, 0xFFFF, 3);
                display_drawChar(40 + 8 *(offset_3), 280, ':', 0xFFFF, 0xFFFF, 3);



                display_drawChar(10 + 0 *(offset_3), 250, 'A', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 1 *(offset_3), 250, 'd', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 2 *(offset_3), 250, 'a', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 3 *(offset_3), 250, 'm', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 4 *(offset_3), 250,  ' ', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 5 *(offset_3), 250, 'J', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 6 *(offset_3), 250, 'a', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 7 *(offset_3), 250, 'c', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 8 *(offset_3), 250, 'o', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 9 *(offset_3), 250, 'b', 0xFFFF, 0xFFFF, 3);
                display_drawChar(10 + 10 *(offset_3),250, 's', 0xFFFF, 0xFFFF, 3);


                display_drawChar(90, 170, 'U', display_color565(0x0, 0xA5, 0xFF), display_color565(0, 0xA5, 0xFF),  6);
                display_drawChar(135, 170, 'F', display_color565(0x0, 0xA5, 0xFF), display_color565(0, 0xA5, 0xFF),  6);
                //display_drawChar(15 + 5 *(offset_4), 270, 'F', color, color, 4);

                display_drawChar(70, 115, 'U', display_color565(0x5D, 0x1A, 0), display_color565(0x5D, 0x1A, 0),  6);
                display_drawChar(115, 115, 'P', display_color565(0x5D, 0x1A, 0), display_color565(0x5D, 0x1A, 0),  6);
                display_drawChar(160, 115, '2', display_color565(0x5D, 0x1A, 0), display_color565(0x5D, 0x1A, 0),  6);

                inMenu = 0;
                G8RTOS_KillSelf();
       }



        sleep(2);
    }
}





void Init_Screen(void){
    //draw the title (Fall Down!)
   Draw_Title(0xFFFF);
   Draw_Play(display_color565(0xd1, 0xae, 0x2e));

   //draw a row for the start menu
   ST7789_Select();
   ST7789_DrawRow(gameNew.rows[1], gameNew.rows[0].hole, ST7789_GREEN);
   ST7789_Deselect();

   //draw the settings button
   Draw_Settings(0xFFFF);

   //Draw the credits button
   Draw_Credits(0xFFFF);
}

void Delete_Menu(void){
    //draw over the title in black(Fall Down!)
   Draw_Title(0xFFFF);
   Draw_Play(display_color565(0xd1, 0xae, 0x2e));

   //draw a row for the start menu
   ST7789_Select();
   ST7789_DrawRow(gameNew.rows[1], gameNew.rows[0].hole, ST7789_GREEN);
   ST7789_Deselect();

   //draw the settings button
   Draw_Settings(0xFFFF);

   //Draw the credits button
   Draw_Credits(0xFFFF);
}
void Draw_Title(uint16_t color){

    display_drawChar(15 + 0 *(offset_4), 270, 'F', color, color, 4);
    display_drawChar(15 + 1 *(offset_4), 270, 'A', color, color, 4);
    display_drawChar(15 + 2* (offset_4), 270, 'L', color, color, 4);
    display_drawChar(15 + 3 *(offset_4), 270, 'L', color, color, 4);
    display_drawChar(15 + 4 *(offset_4), 270, 'D', color, color, 4);
    display_drawChar(15 + 5 *(offset_4), 270, 'O', color, color, 4);
    display_drawChar(15 + 6 *(offset_4), 270, 'W', color, color, 4);
    display_drawChar(15 + 7 *(offset_4), 270, 'N', color, color, 4);
    display_drawChar(15 + 8 *(offset_4), 270, '!', color, color, 4);
}

void Draw_Play(uint16_t color){
    display_drawChar(80 + 0 *(offset_3), 160, 'P', color, color, 3);
    display_drawChar(80 + 1 *(offset_3), 160, 'L', color, color, 3);
    display_drawChar(80 + 2* (offset_3), 160, 'A', color, color, 3);
    display_drawChar(80 + 3 *(offset_3), 160, 'Y', color, color, 3);
}


void Draw_Settings(uint16_t color){
    display_drawChar(40 + 0 *(offset_3), 120, 'S', color, color, 3);
    display_drawChar(40 + 1 *(offset_3), 120, 'E', color, color, 3);
    display_drawChar(40 + 2* (offset_3), 120, 'T', color, color, 3);
    display_drawChar(40 + 3 *(offset_3), 120, 'T', color, color, 3);
    display_drawChar(40 + 4 *(offset_3), 120, 'I', color, color, 3);
    display_drawChar(40 + 5 *(offset_3), 120, 'N', color, color, 3);
    display_drawChar(40 + 6* (offset_3), 120, 'G', color, color, 3);
    display_drawChar(40 + 7 *(offset_3), 120, 'S', color, color, 3);
}

void Draw_Credits(uint16_t color){
    display_drawChar(50 + 0 *(offset_3), 80, 'C', color, color, 3);
    display_drawChar(50 + 1 *(offset_3), 80, 'R', color, color, 3);
    display_drawChar(50 + 2* (offset_3), 80, 'E', color, color, 3);
    display_drawChar(50 + 3 *(offset_3), 80, 'D', color, color, 3);
    display_drawChar(50 + 4 *(offset_3), 80, 'I', color, color, 3);
    display_drawChar(50 + 5 *(offset_3), 80, 'T', color, color, 3);
    display_drawChar(50 + 6* (offset_3), 80, 'S', color, color, 3);
}

void Draw_Button(enum BUTTON button, uint16_t color){
    if(button == PLAY) Draw_Play(color);
    else if(button == SETTINGS) Draw_Settings(color);
    else Draw_Credits(color);
}


void Init_GameData(void){
    gameNew.ball.x = 100;
    gameNew.ball.y = 193;

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
        if(gameNew.ball.y >= 295) {
            lose = 1;
            gameNew.ball.y = 310; //off screen

            G8RTOS_AddThread(Lose_Thread, 10, "lose", 77);
            G8RTOS_KillThread(0);
            G8RTOS_KillSelf();

        }
        gameNew.gameTime++;
        if(gameNew.gameTime == 1000) {
            gameNew.level += 1;
            gameNew.rowColor = 0b0000011111111111;
        }

        else if(gameNew.gameTime == 2500) {
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


void Lose_Thread(){
    //when the lose condition first goes to 1, draw the
    ST7789_Fill(0x0);
    ST7789_Select();
    Draw_Play(0xFFFF);
    ST7789_Deselect();
    while(1){

        ST7789_Select();
        Draw_Play(0xFFFF);
        ST7789_Deselect();
        sleep(199);

}
}



void Read_Buttons() {
    // Initialize / declare any variables here
    uint8_t buttons;

    while(1) {
        // Wait for a signal to read the buttons on the Multimod board.
        // Sleep to debounce
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        sleep(40);
        // Read the buttons status on the Multimod board.
        buttons = MultimodButtons_Get();

        //SW1 is pressed to pause the game
        if(buttons == 253){
            G8RTOS_SignalSemaphore(&sem_Pause);
        }
        else if(buttons == 251){

        }
        else if(buttons == 247){
            //SW3 is pressed
            if(inMenu == 0 && inGame == 0){
                ST7789_Fill(0);
                Init_Screen();
                select = 0;
                inMenu = 1;
                G8RTOS_AddThread(Menu_Thread, 150, "buttons\0", 10);
            }

        }

       //press button 4 to select option on menu
        else if(buttons == 239){
            select = 1;
        }

        // Clear the interrupt
        // Re-enable the interrupt so it can occur again.

        GPIOIntClear(BUTTONS_INT_GPIO_BASE,GPIO_PIN_4);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE,GPIO_PIN_4);
        sleep(9);
    }
}



/********************************Periodic Threads***********************************/


void Get_JoystickX(void) {
    // Read the joystick
    // Send through FIFO.
    int16_t TotalXY = JOYSTICK_GetX();
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, TotalXY);
}


void Get_JoystickY(void) {
    // Read the joystick
    // Send through FIFO.
    int16_t TotalXY = JOYSTICK_GetY();
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

void GameState(){
    G8RTOS_WaitSemaphore(&sem_Pause);
    //while(1);
}
