/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    13-May-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include  <iostm8l151g4.h>				// CPU型号
//#include "stm8l15x.h"
#include "Pin_define.h"		// 管脚定义
#include "ram.h"		// RAM定义
#include "adf7021.h"		// 初始化ADF7021
#include "Timer.h"		// 定时器
#include "ID_Decode.h"
#include "eeprom.h"		// eeprom
#include "uart.h"		// uart
#include "radio.h"
/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void TIM4_Init_HSE(void);
void SysClock_Init_HSE(void);
void LED_TEST(void);
//u8 read_reg = 0;
void main(void)
{
    u8 mode = 0;
    _DI();		// 关全局中断
    //RAM_clean();       // 清除RAM
    WDT_init();
    SysClock_Init();
    InitialFlashReg();
    VHF_GPIO_INIT();
    eeprom_sys_load();
    TIM4_Init();
    SPI_Config_Init();
    //UART1_INIT();  // UART1 for PC Software
    _EI();       // 允许中断
    beep_init();
    if(Receiver_test == 0) mode = 1;
    else mode = 0;
    RF_CMT2310A_Init(mode);
    CMT2310A_GPIO3_INT1_OFF();
    CMT2310A_FreqHopping_Set();
    CMT2310A_Test_Mode();

    FLAG_APP_RX=1;
    //TIME_EMC=10;

    CMT2310A_SetRx();
    vRadioClearTxFifo();
    vRadioClearInterrupt();
    CMT2310A_GPIO3_INT1_ON();
    CMT2310A_GPIO2_INT2_ON();

  while (1)
  {
    ClearWDT(); // Service the WDT
    if(Flag_RxDone == 1)
    {
        Flag_RxDone = 0;
        Flag_FREQ_Scan = 0;
        //rssi = CMT2310A_Get_RSSI();
        //RAM_RSSI_SUM += rssi;
        //RSSI_Read_Counter++;
        //RAM_RSSI_AVG = RAM_RSSI_SUM / RSSI_Read_Counter;
        if(PROFILE_CH_FREQ_32bit_200002EC == 426075000)
        {
            if(Flag_TX_ID_load == 0) CMT2300A_ReadData(SPI_RECEIVE_BUFF,12);
            else CMT2300A_ReadData(SPI_RECEIVE_BUFF,24);
        }
        else CMT2300A_ReadData(SPI_RECEIVE_BUFF,28);
        vRadioClearRxFifo();
        vRadioClearInterrupt();
        bRadioGoRx();
        Flag_FREQ_Scan = 0;

        RX_ANALYSIS();
        Time_APP_blank_TX = 100;
    }

    //APP_TX_PACKET();
    if(FLAG_APP_RX == 1)
    {
        //CMT2310A_Freq_Scanning();
    }
    ID_Decode_IDCheck();
    if(time_Login_exit_256==0)ID_Decode_OUT();

    ID_learn();

    if (FG_Receiver_LED_RX == 1)
        Receiver_LED_RX = 1;
    else if (FG_Receiver_LED_RX == 0)
        Receiver_LED_RX = 0;
    //READ_RSSI_avg();
    //if((RAM_rssi_AVG>=60)||(FG_Receiver_LED_RX==1))Receiver_LED_RX=1;   //26   35
    //else if((RAM_rssi_AVG<=59)&&(FG_Receiver_LED_RX==0))Receiver_LED_RX=0;  //25  34
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
