/**
  ******************************************************************************
  * @file    main.c 
  * @author  jinhao
  * @version V1.0.0
  * @date    22-April-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Includes -------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "sys_cfg.h"
//#include "sys_delay.h"

#include "include.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/
uint8_t firm_buf[1024];
				
/* Functions ------------------------------------------------------------------*/			

/*****************************************************************************
函数名称 : update_firmware
功能描述 : 固件升级
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
uint8_t update_firmware(void)
{
  uint32_t src,obj;
  uint8_t t;
  uint32_t offset;
  
  src = FIREWARE_ADDR_H;
  obj = FIREWARE_ADDR_L;
  
  for(t = 0; t < 48; t ++)
  {
    offset = t * 1024;
    STMFLASH_Read(src + offset, (unsigned int *)firm_buf, 1024/4);
    
    STMFLASH_Write(obj + offset,(unsigned int *)firm_buf, 1024/4);
  }
  
  return SUCCESS;
}

/*****************************************************************************
函数名称 : Reset
功能描述 : 系统复位
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void Reset(void)
{
  NVIC_SystemReset();
}

/*****************************************************************************
函数名称 : Exist_MainProgram
功能描述 : 判断程序是否存在
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
uint8_t Exist_MainProgram(uint32_t *pulApp)
{
  // See if the first location is 0xfffffffff or something that does not
  // look like a stack pointer, or if the second location is 0xffffffff or
  // something that does not look like a reset vector.
  //
  if((pulApp[0] == 0xffffffff) || ((pulApp[0] & 0xfff00000) != 0x20000000) ||
     (pulApp[1] == 0xffffffff) || ((pulApp[1] & 0xfff00001) != 0x08000001))
  {
    return 0;
  }
  else
    return 1;  
}

/*****************************************************************************
函数名称 : JumpToApp
功能描述 : 程序跳转
输入参数 : 无
返回参数 : 无
使用说明 : 无
*****************************************************************************/
void JumpToApp(uint32_t app_address)                  
{
  typedef  void (*pFunction)(void);
  pFunction Jump_To_Application;
  uint32_t JumpAddress;
  
  JumpAddress = *(__IO uint32_t*) (app_address + 4);
  Jump_To_Application = (pFunction) JumpAddress;
  
  __set_MSP(*(__IO uint32_t*) app_address);
  Jump_To_Application();
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    uint32_t magic_code;
    uint32_t app_address;
    
	/* stm32系统配置 */
	Sys_Config();
    
    STMFLASH_Read(PARA_ADDR, (unsigned int *)&magic_code, 4/4);

    if(magic_code == FIREWARE_UPDATE_FLAG)
    {
        if(update_firmware() != SUCCESS)
        {
            Reset();
        }
    }  
    
  //判断程序是否存在
  app_address = FIREWARE_ADDR_L;

  if(Exist_MainProgram((uint32_t *)app_address) == 1)
  {
    JumpToApp(app_address); 
  }
    
	while(1)
	{
//		LED1_ON;
//		delay_ms(500);

//		LED1_OFF;
//		delay_ms(500);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
