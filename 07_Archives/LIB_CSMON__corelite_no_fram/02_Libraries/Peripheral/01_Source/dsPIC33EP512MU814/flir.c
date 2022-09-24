/* *****************************************************************************
 * File:   flir.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 12 13:51
 * 
 * Description: Flir Camera
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "flir.h"
#include "boardcfg.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
eFlirReset_t eFlirReset = FLIR_RESET_COUNT;


HMDRV_FILE_PNTR_MODIFIER sFlirImage_t FlirCameraData[2] __attribute__((space(eds))) = 
{   /* sFlirImage_t array */
{   /* sFlirImage_t */
    {   /* sFlirRow_t Rows[] array */
        {   /* sFlirRow_t */
            {   /* sFlirDot_t Dots[] array */

                /* outside brackets sFlirDot_t */
                /*  inside brackets sFlirDot_t Data[] array */
                {{0x76, 0x54}},         
                {{0x21, 0x98}},         
                {{0x44, 0x55}},         
                {{0x16, 0x17}}          

            },
        },
        {   /* sFlirRow_t */
            {   /* sFlirDot_t Dots[] array */

                /* outside brackets sFlirDot_t */
                /*  inside brackets sFlirDot_t Data[] array */
                {{0x11, 0x12}},         
                {{0x13, 0x14}},         
                {{0x15, 0x16}},         
                {{0x17, 0x18}}          

            },
        },
    } ,  
},
};

//__eds__ sFlirImage_t FlirCameraData2 __attribute__((space(eds)));
//#define FlirCameraData2 FlirCameraData[1]



// to do free data For FlirCameraData3
//sFlirImage_t FlirCameraData3;
//#define FlirCameraData3 FlirCameraData2        
        

//sFlirImage FlirCameraData1;
//sFlirImage FlirCameraData1;
//HMDRV_U8 FlirCameraData[IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_BYTES_PER_DOT];
//HMDRV_U8 FlirCameraData1[IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_BYTES_PER_DOT];
//HMDRV_U8 FlirCameraData2[IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_BYTES_PER_DOT];
//HMDRV_U8 FlirCameraData3[IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_DOTS_PER_LINE_COUNT*IR_CAMERA_BYTES_PER_DOT];





/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */


/* *****************************************************************************
 * FLIR_vProcess
 * 
 * Description: FLIR Process 
 **************************************************************************** */
void FLIR_vProcess(void)
{
    switch (eFlirReset)
    {
        case FLIR_RESET_RELSEASE:
           FLIR_RESET_SetHigh();
           break;
           
        case FLIR_RESET_HOLD:
           FLIR_RESET_SetLOw();
           break;
           
        default:
           break;
           
    }
}



