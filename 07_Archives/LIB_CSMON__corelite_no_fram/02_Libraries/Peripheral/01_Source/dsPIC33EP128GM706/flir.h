/* *****************************************************************************
 * File:   flir.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 12 13:51
 * 
 * Description: Flir Camera
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef FLIR_H
#define	FLIR_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "stdint.h"
#include "hmlib.h"
#include "flirzip.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define IR_CAMERA_BYTES_PER_DOT_RAW14   2
#define IR_CAMERA_BYTES_PER_DOT_RGB888  3

#define IR_CAMERA_BYTES_PER_DOT_RAW         IR_CAMERA_BYTES_PER_DOT_RAW14 
#define IR_CAMERA_BYTES_PER_DOT_RGB         IR_CAMERA_BYTES_PER_DOT_RGB888 
#define IR_CAMERA_BYTES_PER_DOT_MAX         IR_CAMERA_BYTES_PER_DOT_RGB888 
#define IR_CAMERA_DOTS_PER_LINE_COUNT   80
#define IR_CAMERA_LINES_COUNT           60

#define FLIR_FIRST_PIXEL_ALWAYS_IN_RANGE    0   /* for byte zip */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint8_t Data[IR_CAMERA_BYTES_PER_DOT_MAX];
}sFlirDot_t;

typedef struct
{
    sFlirDot_t Dots[IR_CAMERA_DOTS_PER_LINE_COUNT];   
}sFlirRow_t;
typedef struct
{
    sFlirRow_t Rows[IR_CAMERA_LINES_COUNT];   
}sFlirImage_t;


typedef enum
{
    FLIR_RESET_RELSEASE,
    FLIR_RESET_HOLD,
    FLIR_RESET_COUNT
            
}eFlirReset_t;



/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern eFlirReset_t eFlirReset;
extern HMDRV_FILE_PNTR_MODIFIER sFlirImage_t FlirCameraData[2] __attribute__((space(eds)));



/* *****************************************************************************
 * Macros Definitions Compatibility
 **************************************************************************** */
#define FlirCameraData1 FlirCameraData[0]
#define FlirCameraData2 FlirCameraData[1]
#define FlirCameraData3 FlirCameraData2        



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void FLIR_vProcess(void);
void FLIR_vRowCompress(HMDRV_ADDR pSrc, HMDRV_ADDR pDst);

#endif	/* FLIR_H */
