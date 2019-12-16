/****************************************************************************
* File: fih_bbs_camera.c                                                    *
* Description: write camera error message to bbs log                        *
*                                                                           *
****************************************************************************/


#ifndef _FIH_BBS_CAMERA_H_
#define _FIH_BBS_CAMERA_H_

/****************************************************************************
*                               Include File                                *
****************************************************************************/

/****************************************************************************
*                         Public Constant Definition                        *
****************************************************************************/
/*NE1 component start*/
//front	S5K4H8	0x5A
//main	S5K4H8	0x20
//AF	GC9762	0x18

//Main
#define FIH_I2C_ADDR_S5K4H8_CAMERA 0x20  //FIH_I2C_ADDR_S5K4H8_EEPROM 0x20
#define FIH_I2C_ADDR_S5K4H8_ACTUATOR 0x18
//Front
#define FIH_I2C_ADDR_S5K4H8sub_CAMERA 0x5A //FIH_I2C_ADDR_S5K4H8sub_EEPROM is the same
#define FIH_I2C_ADDR_S5K4H8sub_ACTUATOR 0x18
/*NE1 component end*/

/*FRP component start*/
//front	GC2385	0x6E
//main	S5K5E8YX	0X20
//No AF

#define FIH_I2C_ADDR_S5K5E8_CAMERA 0x20 //FIH_I2C_ADDR_S5K5E8_EEPROM is the same
#define FIH_I2C_ADDR_GC2385_CAMERA 0x6E //FIH_I2C_ADDR_GC2385_EEPROM is the same
/*FRP component end*/

enum FIH_BBS_CAMERA_LOCATION
{
    FIH_BBS_CAMERA_LOCATION_MAIN,
    FIH_BBS_CAMERA_LOCATION_FRONT,
    FIH_BBS_CAMERA_LOCATION_SUB,
};

enum FIH_BBS_CAMERA_MODULE
{
    FIH_BBS_CAMERA_MODULE_IC,
    FIH_BBS_CAMERA_MODULE_EEPROM,
    FIH_BBS_CAMERA_MODULE_ACTUATOR,
    FIH_BBS_CAMERA_MODULE_OIS,
};


enum FIH_BBS_CAMERA_ERRORCODE
{
    FIH_BBS_CAMERA_ERRORCODE_POWER_UP,
    FIH_BBS_CAMERA_ERRORCODE_POWER_DW,
    FIH_BBS_CAMERA_ERRORCODE_MCLK_ERR,
    FIH_BBS_CAMERA_ERRORCODE_I2C_READ,
    FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE,
    FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE_SEQ,
    FIH_BBS_CAMERA_ERRORCODE_UNKOWN,
    FIH_BBS_CAMERA_ERRORCODE_MAX=255,
};

#define FIH_BBSUEC_MAIN_CAM_ID 9
#define FIH_BBSUEC_FRONT_CAM_ID 44

enum FIH_BBSUEC_CAMERA_ERRORCODE
{
    FIH_BBSUEC_CAMERA_ERRORCODE_SENSOR_I2C = 1,
    FIH_BBSUEC_CAMERA_ERRORCODE_SENSOR_POWER = 10,
    FIH_BBSUEC_CAMERA_ERRORCODE_ACTUATOR_POWER,
    FIH_BBSUEC_CAMERA_ERRORCODE_ACTUATOR_I2C,
    FIH_BBSUEC_CAMERA_ERRORCODE_EEPROM_I2C,
};

#endif

