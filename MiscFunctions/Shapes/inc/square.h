/*
 * square.h
 *
 *  Created on: Nov 8, 2023
 *      Author: diamond
 */


#include <math.h>
#include "../../LinAlg/inc/quaternions.h"
#include "../../LinAlg/inc/vect3d.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/***********************************Structures**************************************/

typedef struct {
    int16_t x_pos;
    int16_t y_pos;
    uint8_t width;
    uint8_t height;

} square_t;

/***********************************Structures**************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void Square_Generate(square_t* square);

/********************************Public Functions***********************************/
