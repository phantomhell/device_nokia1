/****************************************************************************
* File: fih_bbs_camera.c                                                    *
* Description: write camera error message to bbs log                        *
*                                                                           *
****************************************************************************/

/****************************************************************************
*                               Include File                                *
****************************************************************************/
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/kernel.h> /* printk */
#include <linux/module.h>
#include "fih_camera_bbs.h"


/****************************************************************************
*                               Module I2C ADDR                             *
****************************************************************************/
struct fih_bbs_camera_module_list {
	u8 i2c_addr;
	u8 type;
	char module_name[32];
};

/****************************************************************************
*                          Private Type Declaration                         *
****************************************************************************/
#ifdef CONFIG_FIH_PROJECT_NE1
struct fih_bbs_camera_module_list fih_camera_list[] = {
	/*NE1 start*/
	{
	.i2c_addr = FIH_I2C_ADDR_S5K4H8_CAMERA,
	.type = ((FIH_BBS_CAMERA_LOCATION_MAIN << 4) | FIH_BBS_CAMERA_MODULE_IC),
	.module_name = "Camera/EEPROM (S5K4H8)",
	},
	{
	.i2c_addr = FIH_I2C_ADDR_S5K4H8_ACTUATOR,
	.type = ((FIH_BBS_CAMERA_LOCATION_MAIN << 4) | FIH_BBS_CAMERA_MODULE_ACTUATOR),
	.module_name = "Actuator (S5K4H8)",
	},
	{
	.i2c_addr = FIH_I2C_ADDR_S5K4H8sub_CAMERA,
	.type = ((FIH_BBS_CAMERA_LOCATION_FRONT << 4) | FIH_BBS_CAMERA_MODULE_IC),
	.module_name = "Camera/EEPROM (S5K4H8sub)",
	},
	{
	.i2c_addr = FIH_I2C_ADDR_S5K4H8sub_ACTUATOR,
	.type = ((FIH_BBS_CAMERA_LOCATION_FRONT << 4) | FIH_BBS_CAMERA_MODULE_ACTUATOR),
	.module_name = "Actuator (S5K4H8sub)",
	},
	/*NE1 end*/
};
#else //FRTP
struct fih_bbs_camera_module_list fih_camera_list[] = {
	/*FRT start*/
	{
	.i2c_addr = FIH_I2C_ADDR_S5K5E8_CAMERA,
	.type = ((FIH_BBS_CAMERA_LOCATION_MAIN << 4) | FIH_BBS_CAMERA_MODULE_IC),
	.module_name = "Camera/EEPROM (S5K5E8)",
	},
	{
	.i2c_addr = FIH_I2C_ADDR_GC2385_CAMERA,
	.type = ((FIH_BBS_CAMERA_LOCATION_FRONT << 4) | FIH_BBS_CAMERA_MODULE_IC),
	.module_name = "Camera/EEPROM (GC2385)",
	},
	/*FRT end*/
};
#endif
/**********************************************************************
                         Public Function                              *
**********************************************************************/

void fih_bbs_camera_msg(int module, int error_code)
{
  char param1[32]; // Camera ic, actuator, EEPROM, OIS,¡K
  char param2[64]; // I2C flash error, power up fail, ¡K

	switch (module) {
		case FIH_BBS_CAMERA_MODULE_IC: strcpy(param1, "Camera ic"); break;
		case FIH_BBS_CAMERA_MODULE_EEPROM: strcpy(param1, "EEPROM"); break;
		case FIH_BBS_CAMERA_MODULE_ACTUATOR: strcpy(param1, "Actuator"); break;
		case FIH_BBS_CAMERA_MODULE_OIS: strcpy(param1, "OIS"); break;
		default: strcpy(param1, "Unknown"); break;
  }

	switch (error_code) {
		case FIH_BBS_CAMERA_ERRORCODE_POWER_UP: strcpy(param2, "Power up fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_POWER_DW: strcpy(param2, "Power down fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_MCLK_ERR: strcpy(param2, "MCLK error"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_READ: strcpy(param2, "i2c read err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE: strcpy(param2, "i2c write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE_SEQ: strcpy(param2, "i2c seq write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_UNKOWN: strcpy(param2, "unknow error"); break;
		default: strcpy(param2, "Unknown"); break;
	}

  printk("BBox::UPD;88::%s::%s\n", param1, param2);

  return ;
}
EXPORT_SYMBOL(fih_bbs_camera_msg);

void fih_bbs_camera_msg_by_type(int type, int error_code)
{
  char param1[32]; // Camera ic, actuator, EEPROM, OIS,¡K
  char param2[64]; // I2C flash error, power up fail, ¡K
  int isList=0;
  int i=0;

  for(i=0; i<(sizeof(fih_camera_list)/sizeof(struct fih_bbs_camera_module_list)); i++)
  {
    if(type == fih_camera_list[i].type)
    {
      strcpy(param1, fih_camera_list[i].module_name);
      isList = 1;
      break;
    }
  }

  //Do not find in list.
  if(!isList)
  {
    strcpy(param1, "Unknown ");
  }

	switch (error_code) {
		case FIH_BBS_CAMERA_ERRORCODE_POWER_UP: strcpy(param2, "Power up fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_POWER_DW: strcpy(param2, "Power down fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_MCLK_ERR: strcpy(param2, "MCLK error"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_READ: strcpy(param2, "i2c read err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE: strcpy(param2, "i2c write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE_SEQ: strcpy(param2, "i2c seq write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_UNKOWN: strcpy(param2, "unknow error"); break;
		default: strcpy(param2, "Unknown"); break;
	}

  printk("BBox::UPD;88::%s::%s\n", param1, param2);

  return ;
}
EXPORT_SYMBOL(fih_bbs_camera_msg_by_type);

void fih_bbs_camera_msg_by_addr(int addr, int error_code)
{
  char param1[32]; // Camera ic, actuator, EEPROM, OIS,¡K
  char param2[64]; // I2C flash error, power up fail, ¡K
  int isList=0;
  int i=0;
  u8 bbs_id=99,bbs_err=99,camera=0,module=0;

  for(i=0; i<(sizeof(fih_camera_list)/sizeof(struct fih_bbs_camera_module_list)); i++)
  {
	//printk("[MIMI@@]addr:0x%x  i2c_addr:0x%x\n", addr, fih_camera_list[i].i2c_addr);
    //if(addr == (fih_camera_list[i].i2c_addr>>1))
	if(addr == (fih_camera_list[i].i2c_addr))
    {
      camera = fih_camera_list[i].type >> 4;
      module = fih_camera_list[i].type & 0xf;
	  //printk("[MIMI@@]camera:0x%x(0:main 1:front)  module:0x%x(0:camIC 1:EEPROM 2:AF)\n", camera, module);
      if(camera == FIH_BBS_CAMERA_LOCATION_MAIN)
        bbs_id = FIH_BBSUEC_MAIN_CAM_ID;
      else if(camera == FIH_BBS_CAMERA_LOCATION_FRONT)
        bbs_id = FIH_BBSUEC_FRONT_CAM_ID;
      strcpy(param1, fih_camera_list[i].module_name);
	  //printk("[MIMI@@]bbs_id:%d (main:9 front:44)\n", bbs_id);
	  //printk("[MIMI@@]param1 module_name:%s\n", fih_camera_list[i].module_name);
      isList = 1;
      break;
    }
  }

  //Do not find in list.
  if(!isList)
    strcpy(param1, "Unknown module");

	switch (error_code) {
		case FIH_BBS_CAMERA_ERRORCODE_POWER_UP: strcpy(param2, "Power up fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_POWER_DW: strcpy(param2, "Power down fail"); break;
		case FIH_BBS_CAMERA_ERRORCODE_MCLK_ERR: strcpy(param2, "MCLK error"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_READ: strcpy(param2, "i2c read err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE: strcpy(param2, "i2c write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE_SEQ: strcpy(param2, "i2c seq write err"); break;
		case FIH_BBS_CAMERA_ERRORCODE_UNKOWN: strcpy(param2, "unknow error"); break;
		default: strcpy(param2, "Unknown"); break;
	}
	switch (error_code) {
		case FIH_BBS_CAMERA_ERRORCODE_POWER_UP:
		case FIH_BBS_CAMERA_ERRORCODE_POWER_DW:
			if (module == FIH_BBS_CAMERA_MODULE_IC)
				bbs_err = FIH_BBSUEC_CAMERA_ERRORCODE_SENSOR_POWER;
			else if (module == FIH_BBS_CAMERA_MODULE_ACTUATOR)
				bbs_err = FIH_BBSUEC_CAMERA_ERRORCODE_ACTUATOR_POWER;
			break;
		case FIH_BBS_CAMERA_ERRORCODE_MCLK_ERR:
			break;
		case FIH_BBS_CAMERA_ERRORCODE_I2C_READ:
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE:
		case FIH_BBS_CAMERA_ERRORCODE_I2C_WRITE_SEQ:
			if (module == FIH_BBS_CAMERA_MODULE_IC)
				bbs_err = FIH_BBSUEC_CAMERA_ERRORCODE_SENSOR_I2C;
			else if (module == FIH_BBS_CAMERA_MODULE_EEPROM)
				bbs_err = FIH_BBSUEC_CAMERA_ERRORCODE_EEPROM_I2C;
			else if (module == FIH_BBS_CAMERA_MODULE_ACTUATOR)
				bbs_err = FIH_BBSUEC_CAMERA_ERRORCODE_ACTUATOR_I2C;
			break;
		case FIH_BBS_CAMERA_ERRORCODE_UNKOWN:
		default:
			break;
	}

  printk("BBox::UPD;88::%s::%s\n", param1, param2);
  printk("BBox::UEC;%d::%d\n", bbs_id, bbs_err);

  return ;
}
EXPORT_SYMBOL(fih_bbs_camera_msg_by_addr);

/************************** End Of File *******************************/
