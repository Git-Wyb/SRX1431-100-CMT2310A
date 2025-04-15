/***********************************************************************/
/*  FILE        :initial.c                                             */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include  <iostm8l151g4.h>
//#include        "stm8l15x.h"
#include "Pin_define.h"		// 管脚定义
#include "ram.h"		// RAM定义
#include "adf7021.h"		// 初始化ADF7021
#include "uart.h"		// uart
#include "radio.h"

void RAM_clean(void){						// 清除RAM
	/*asm("ldw X,#0");
	asm("clear_ram:");
	asm("clr (X)");
	asm("incw X");
	asm("cpw X,#0x6ff");
	asm("jrule clear_ram");  */
}
void WDT_init(void)
{
  IWDG_KR=0xCC;
  IWDG_KR=0x55;
  IWDG_PR=3;
  IWDG_KR=0xAA;
}
void ClearWDT(void)
{
  IWDG_KR=0xAA;
}
//========================GPIO说明===============================================================
//  GPIO_Mode_In_FL_No_IT      = (uint8_t)0x00,   /*!< Input floating, no external interrupt */
//  GPIO_Mode_In_PU_No_IT      = (uint8_t)0x40,   /*!< Input pull-up, no external interrupt */
//  GPIO_Mode_In_FL_IT         = (uint8_t)0x20,   /*!< Input floating, external interrupt */
//  GPIO_Mode_In_PU_IT         = (uint8_t)0x60,   /*!< Input pull-up, external interrupt */
//  GPIO_Mode_Out_OD_Low_Fast  = (uint8_t)0xA0,   /*!< Output open-drain, low level, 10MHz */
//  GPIO_Mode_Out_PP_Low_Fast  = (uint8_t)0xE0,   /*!< Output push-pull, low level, 10MHz */
//  GPIO_Mode_Out_OD_Low_Slow  = (uint8_t)0x80,   /*!< Output open-drain, low level, 2MHz */
//  GPIO_Mode_Out_PP_Low_Slow  = (uint8_t)0xC0,   /*!< Output push-pull, low level, 2MHz */
//  GPIO_Mode_Out_OD_HiZ_Fast  = (uint8_t)0xB0,   /*!< Output open-drain, high-impedance level, 10MHz */
//  GPIO_Mode_Out_PP_High_Fast = (uint8_t)0xF0,   /*!< Output push-pull, high level, 10MHz */
//  GPIO_Mode_Out_OD_HiZ_Slow  = (uint8_t)0x90,   /*!< Output open-drain, high-impedance level, 2MHz */
//  GPIO_Mode_Out_PP_High_Slow = (uint8_t)0xD0    /*!< Output push-pull, high level, 2MHz */
//===降低功耗说明：①I/O没用，必须置Input pull-up    ②I/O外围有IC，但没用，必须置Input floating=====

void VHF_GPIO_INIT(void){					// CPU端口设置
/****************端口设置说明***************************
CR1寄存器  输出 Output（1=推挽、0=OC）
           输入 Input（1=上拉、0=浮动）
***************end************************************/
    HA_L_signal_direc = Input; // Input   HA 下限信号   低电平有效
    HA_L_signal_CR1=1;

    HA_ERR_signal_direc = Input;// Input   HA 异常信号  低电平有效
    HA_ERR_signal_CR1=1;

    HA_Sensor_signal_direc = Input;// Input   HA 传感器信号  低电平有效
    HA_Sensor_signal_CR1=1;

    Receiver_Login_direc = Input;// Input   受信机登录键   低电平有效
    Receiver_Login_CR1=1;

    //      Receiver_Buzzer_direc = Output;// Output   受信机蜂鸣器  高电平有效
    //      Receiver_Buzzer_CR1=1;
    //      Receiver_Buzzer=0;
    Receiver_vent_direc = Input;// Input   受信机换气联动ON/OFF
    Receiver_vent_CR1=1;

    PIN_BEEP_direc = Output;    // Output   蜂鸣器
    PIN_BEEP_CR1 = 1;
    PIN_BEEP=0;

    Receiver_LED_OUT_direc = Output;// Output   受信机继电器动作输出  高电平有效
    Receiver_LED_OUT_CR1=1;
    Receiver_LED_OUT=0;

    Receiver_LED_TX_direc = Output;// Output   受信机送信指示  高电平有效
    Receiver_LED_TX_CR1=1;
    Receiver_LED_TX=0;

    Receiver_LED_RX_direc = Output;// Output   受信机受信指示  高电平有效
    Receiver_LED_RX_CR1=1;
    Receiver_LED_RX=0;

    Inverters_OUT_direc=Input;    // 输入   继电器输出信号反向   低电平有效
    Inverters_OUT_CR1=1;
    if(Inverters_OUT==1){FG_allow_out=1;FG_NOT_allow_out=0;}
    else {FG_allow_out=0;FG_NOT_allow_out=1;}

    Receiver_OUT_OPEN_direc = Output;  // Output   受信机继电器OPEN  高电平有效
    Receiver_OUT_OPEN_CR1=1;
    Receiver_OUT_OPEN=FG_NOT_allow_out;

    Receiver_OUT_CLOSE_direc = Output;  // Output   受信机继电器CLOSE  高电平有效
    Receiver_OUT_CLOSE_CR1=1;
    Receiver_OUT_CLOSE=FG_NOT_allow_out;

    Receiver_OUT_STOP_direc = Output;  // Output   受信机继电器STOP  高电平有效
    Receiver_OUT_STOP_CR1=1;
    Receiver_OUT_STOP=FG_NOT_allow_out;

    Receiver_OUT_VENT_direc = Output;
    Receiver_OUT_VENT_CR1=1;
    Receiver_OUT_VENT=FG_NOT_allow_out;

    Receiver_test_direc=Input;    // 输入     test脚
    Receiver_test_CR1=1;
//------------------------------------------------------
    CG2214M6_GPIO_Init();
    if(Receiver_test == 0)
        BerExtiInit();
}

void CMT2310A_GPIO3_INT1_ON(void)
{
    CMT2310A_GPIO3_INT1_DDR = 0;   //Input mode
    CMT2310A_GPIO3_INT1_CR1 = 1;   //0:Floating input,1:Input with pull-up
    CMT2310A_GPIO3_INT1_CR2 = 1;   //开启中断 ;
    EXTI_CR1 &= (~MASK_EXTI_CR1_P1IS);
    EXTI_CR1 |= 0x08;   //0x08 Falling edge only,0x04 Rising edge only,0x0C  Rising and falling edge
}

void CMT2310A_GPIO3_INT1_OFF(void)
{
    CMT2310A_GPIO3_INT1_DDR = 0;   //Input mode
    CMT2310A_GPIO3_INT1_CR1 = 1;   //0:Floating input,1:Input with pull-up
    CMT2310A_GPIO3_INT1_CR2 = 0;   //关闭中断 ;
    EXTI_CR1 &= (~MASK_EXTI_CR1_P1IS);
    EXTI_CR1 |= 0x08;   //0x08 Falling edge only,0x04 Rising edge only,0x0C  Rising and falling edge
}

void CMT2310A_GPIO2_INT2_ON(void)
{
    CMT2310A_GPIO2_INT2_DDR = 0;   //Input mode
    CMT2310A_GPIO2_INT2_CR1 = 1;   //0:Floating input,1:Input with pull-up
    CMT2310A_GPIO2_INT2_CR2 = 1;   //开启中断 ;
    EXTI_CR1 &= (~MASK_EXTI_CR1_P0IS);
    EXTI_CR1 |= 0x02;   //0x02 Falling edge only,0x01 Rising edge only,0x03  Rising and falling edge
}

void CMT2310A_GPIO2_INT2_OFF(void)
{
    CMT2310A_GPIO2_INT2_DDR = 0;   //Input mode
    CMT2310A_GPIO2_INT2_CR1 = 1;   //0:Floating input,1:Input with pull-up
    CMT2310A_GPIO2_INT2_CR2 = 0;   //关闭中断 ;
    EXTI_CR1 &= (~MASK_EXTI_CR1_P0IS);
    EXTI_CR1 |= 0x02;
}

void CG2214M6_GPIO_Init(void)
{
    CG2214M6_VC1_DDR = Output; /* 设置数据方向寄存噿1为输出，0为输兿-查看STM8寄存器RM0031.pdf 10.9 */
    CG2214M6_VC1_CR1 = 1;      /* 设置推挽输出--查看STM8寄存器RM0031.pdf 10.9*/
    CG2214M6_VC1_CR2 = 1;      /* 设置输出频率 1丿0M＿丿M--查看STM8寄存噿pdf P89 */

    CG2214M6_VC2_DDR = Output; /* 设置数据方向寄存噿1为输出，0为输兿-查看STM8寄存噿RM0031.pdf 10.9 */
    CG2214M6_VC2_CR1 = 1;      /* 设置推挽输出--查看STM8寄存器RM0031.pdf 10.9*/
    CG2214M6_VC2_CR2 = 1;      /* 设置输出频率 1丿0M＿丿M--查看STM8寄存噿pdf P89 */
}

void BerExtiInit(void)
{
    CMT2310A_GPIO4_DDR = Input; //输入
    CMT2310A_GPIO4_CR1 = 0;     //1: Input with pull-up 0: Floating input
    CMT2310A_GPIO4_CR2 = 1;     //使能中断
    EXTI_CR2 &= (~MASK_EXTI_CR2_P4IS);
    EXTI_CR2 |= 0x01;//0x01 Rising edge only; 0x02 Falling edge only

    CMT2310A_GPIO5_DDR = Input; //输入
    CMT2310A_GPIO5_CR1 = 1;     //1: Input with pull-up 0: Floating input
    CMT2310A_GPIO5_CR2 = 0;     //禁止中断
}
void BerExtiUnInit(void)
{
    CMT2310A_GPIO4_DDR = Input; //输入
    CMT2310A_GPIO4_CR1 = 0;     //1: Input with pull-up 0: Floating input
    CMT2310A_GPIO4_CR2 = 0;     //禁止中断
    EXTI_CR2 &= (~MASK_EXTI_CR2_P4IS);

    CMT2310A_GPIO5_DDR = Input; //输入
    CMT2310A_GPIO5_CR1 = 1;     //1: Input with pull-up 0: Floating input
    CMT2310A_GPIO5_CR2 = 0;     //禁止中断
}
//============================================================================================
void SysClock_Init( void ){ 				// 系统时钟（外部时钟）
//    /* Infinite loop */
//    CLK_DeInit();                                         //时钟恢复默认
//    CLK_HSICmd(ENABLE);
//    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)==RESET);//等待直到LSI稳定
////    CLK_HSEConfig(CLK_HSE_ON);
////    CLK_HSEConfig(CLK_HSE_ON);
////    while(CLK_GetFlagStatus(CLK_FLAG_HSERDY)==RESET);//等待直到HSE稳定
//    CLK_SYSCLKSourceSwitchCmd(ENABLE);
////    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);
//    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
//    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
//    CLK_SYSCLKSourceSwitchCmd(DISABLE);
////    CLK_LSICmd(ENABLE);
////    while(CLK_GetFlagStatus(CLK_FLAG_LSIRDY)==RESET);//等待直到LSI稳定
////    CLK_HSEConfig(CLK_HSE_OFF);
////    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
////    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
////    CLK_SYSCLKSourceSwitchCmd(DISABLE);
//
//    //CLK_LSICmd(ENABLE);   //使能LSI时钟给看门狗用
//    //while(CLK_GetFlagStatus(CLK_FLAG_LSIRDY)==RESET);//等待直到LSI稳定


	CLK_ICKCR_HSION = 1;				// 使能内部RC OSC（16.00MHz）
	while(( CLK_ICKCR & 0x02 ) == 0 );		// 检查内部晶振
	CLK_SWR = 0x01;					// 指定HSI为主时钟
//	while(( CLK_SWCR & 0x08 ) == 0 );		// 等待HSI切换
	CLK_SWCR_SWEN = 1;						// 执行切换
	CLK_CKDIVR = 0x00;		// 设置时钟分频  f HSI= f HSI RC输出/1    f CPU= f MASTER
	//---------------------------------------- 外设
	//CLK_PCKENR1 = 0x84;						// T1,UART1
	CLK_PCKENR1 = 0x64;						// T4,UART1,beep
	CLK_PCKENR2 = 0x03;						// ADC,T1

	CLK_ICKCR_LSION = 1;				// 使能内部LSI OSC（38KHz）
	while(CLK_ICKCR_LSIRDY == 0 );		// 检查内部LSI OSC
}

void CLK_DeInit(void);
void SysClock_Init_HSE(void)
{
    CLK_DeInit();
    CLK_ECKR_HSEON = 1;     //Enable HSE

    while(( CLK_ECKR & 0x02 ) == 0 );      //Wait HSE clock ready

    CLK_CKDIVR = 0x00;   //1 prescaler
    CLK_SWR = 0x04;       //HSE selected as system clock source
    CLK_SWCR_SWEN = 1;						// 执行切换

    CLK_PCKENR1 = 0x64;						// T4,UART1,beep
	CLK_PCKENR2 = 0x03;						// ADC,T1

    CLK_ICKCR_LSION = 1;				// 使能内部LSI OSC（38KHz）
	while(CLK_ICKCR_LSIRDY == 0 );		// 检查内部LSI OSC
}

void beep_init( void )
{
     //BEEP_CSR=0x4E;
  BEEP_CSR2=0;
  BEEP_CSR2_BEEPDIV=3;
  BEEP_CSR2_BEEPSEL=1;
  CLK_CBEEPR_CLKBEEPSEL0=1;
  CLK_CBEEPR_CLKBEEPSEL1=0;
}

//===================Delayus()延时===============//    Crystal: 16M HSI
void Delayus(unsigned char timer)
{
unsigned char x;                   //延时T=((timer-1)*0.313+2 us
 for(x=0;x<timer;x++)
     __asm("nop");
}

/*
void RF_test_mode(void )
{
  UINT8 uart_data,Boot_i;
//    Receiver_LED_OUT=1;
//    for(time_3sec=0;time_3sec<9000;time_3sec++){
//      Delayus(250);   //80us
//      ClearWDT(); // Service the WDT
//    }
//    Receiver_LED_OUT=0;

  Receiver_LED_OUT=1;
  for(Boot_i=0;Boot_i<2;Boot_i++){
      for(time_3sec=0;time_3sec<6000;time_3sec++){
         Delayus(250);   //80us
         ClearWDT(); // Service the WDT
      }
      Receiver_LED_OUT=!Receiver_LED_OUT;
  }
  Receiver_LED_OUT=0;



    while(Receiver_test==0){
        ClearWDT(); // Service the WDT
        //if(HA_ERR_signal==0){      //test ADF7021 TX
	if(HA_ERR_signal==0){
	  if(HA_L_signal==0)Tx_Rx_mode=0;
	  else Tx_Rx_mode=1;
	}
	else{
	  if(HA_L_signal==0)Tx_Rx_mode=2;
	  else Tx_Rx_mode=3;
	}
	if((Tx_Rx_mode==0)||(Tx_Rx_mode==1)){
	  FG_test_rx=0;
	  Receiver_LED_RX=0;
	  FG_test_tx_off=0;
	  //if(HA_L_signal==0){    //发载波，无调制信号
	  if(Tx_Rx_mode==0){
	    Receiver_LED_OUT=1;
	    FG_test_mode=0;
	    FG_test_tx_1010=0;
	    if(FG_test_tx_on==0){FG_test_tx_on=1;ADF7021_DATA_direc=Input;dd_set_TX_mode_carrier();}
	  }
	  else {    //发载波，有调制信号
	    if(TIMER1s==0){
	      TIMER1s=500;
	      Receiver_LED_OUT=!Receiver_LED_OUT;
	    }
	    FG_test_mode=1;
	    FG_test_tx_on=0;
	    if(FG_test_tx_1010==0){FG_test_tx_1010=1;ADF7021_DATA_direc=Output;dd_set_TX_mode_1010pattern();}
	  }
	}
        //else  {           //test ADF7021 RX
	if((Tx_Rx_mode==2)||(Tx_Rx_mode==3)){
	  FG_test_rx=1;
	  Receiver_LED_OUT=0;
	  FG_test_mode=0;
	  FG_test_tx_on=0;
	  FG_test_tx_1010=0;
	  if(FG_test_tx_off==0){FG_test_tx_off=1;
                                dd_set_RX_mode_test();
                                ADF7021_DATA_direc=Input;
                               }
	  //if(HA_L_signal==0){
	  if(Tx_Rx_mode==2)
	    if(TIMER1s==0){
	      TIMER1s=500;
	      Receiver_LED_RX=!Receiver_LED_RX;
	    }
	  if(Tx_Rx_mode==3){
            if(X_COUNT >= 1200){
              X_COUNT = 0;
	      if(X_ERR>=60)Receiver_LED_RX=0;
	      else Receiver_LED_RX=1;
              uart_data = (X_ERR/1000) + 48;//48;//（X_ERR/1000) + 48;
	      Send_char(uart_data);
              X_ERR = X_ERR%1000;
              uart_data = (X_ERR/100) + 48;//X_ERR/256;
	      Send_char(uart_data);
              X_ERR = X_ERR%100;
              uart_data =(X_ERR/10) + 48;
	      Send_char(uart_data);
              X_ERR = X_ERR%10;
              uart_data = X_ERR +48;
	      Send_char(uart_data);
              uart_data = 13;//|字符
	      Send_char(uart_data);
              X_ERR = 0;
            }
	  }
	}
	PC_PRG();	       // PC控制
//	if((ADF7021_DATA_CLK==1)&&(FG_test_mode==1)&&(FG_test1==0)){
//           ADF7021_DATA_tx=!ADF7021_DATA_tx;
//           FG_test1=1;
//        }
//       if(ADF7021_DATA_CLK==0)FG_test1=0;

    }
    UART1_end();
    FG_test_rx=0;
    TIMER1s=0;
    Receiver_LED_TX=0;
    Receiver_LED_RX=0;
    FG_Receiver_LED_RX=0;
    Receiver_LED_OUT=0;

    FLAG_APP_RX=1;
    dd_set_RX_mode();
    //TIME_Fine_Calibration=9000;
    TIME_EMC=10;
}  */

//CMT2310A_CFG	g_radio;

void CMT2310A_FreqHopping_Select(u32 freq);
void CMT2310A_Test_Mode(void)
{
    UINT8 Boot_i;

    Receiver_LED_OUT=1;
    for(Boot_i=0;Boot_i<2;Boot_i++)
    {
        for(time_3sec=0;time_3sec<6000;time_3sec++)
        {
            Delayus(250);   //80us
            ClearWDT(); // Service the WDT
        }
        Receiver_LED_OUT=!Receiver_LED_OUT;
    }
    Receiver_LED_OUT=0;

    while(Receiver_test==0)
    {
        ClearWDT(); // Service the WDT
        //TX
        if(HA_ERR_signal==0)
        {
            if(HA_L_signal==0)Tx_Rx_mode=0;
            else Tx_Rx_mode=1;
        }
        else
        { //RX
            if(HA_L_signal==0)Tx_Rx_mode=2;
            else Tx_Rx_mode=3;
        }
        if((Tx_Rx_mode==0)||(Tx_Rx_mode==1))
        {
            FG_test_rx=0;
            Receiver_LED_RX=0;
            FG_test_tx_off=0;
            //发载波，无调制信号
            if(Tx_Rx_mode==0)
            {
                Receiver_LED_TX=1;
                FG_test_mode=0;
                FG_test_tx_1010=0;
                if(FG_test_tx_on==0)
                {
                    CG2214M6_USE_T;
                    FG_test_tx_on=1;
                    delay_nms(100);
                    bRadioGoStandby();
                    vRadioClearTxFifo();
                    vRadioClearInterrupt();

                    CMT2310A_SetDataMode(0); //0=direct mode
                    //CMT2310A_FreqHopping_Select(429175000);
                    CMT2310A_Freq_Select(429177000);
                    //CMT2310A_DataRate_Select(RATE_4_8K);
                    delay_nms(10);
                    bRadioGoTx();
                }
            }
            else
            {    //发载波，有调制信号
                if(TIMER1s==0)
                {
                    TIMER1s=500;
                    Receiver_LED_TX=!Receiver_LED_TX;
                }
                FG_test_mode=1;
                FG_test_tx_on=0;
                if(FG_test_tx_1010==0)
                {
                    CG2214M6_USE_T;
                    FG_test_tx_1010=1;
                    delay_nms(100);
                    bRadioGoStandby();
                    vRadioClearTxFifo();
                    vRadioClearInterrupt();
                    CMT2310A_SetDataLength(UHF_LEN);
                    vRadioSetInt1Sel(INT_SRC_TX_DONE);
                    CMT2310A_SetPreamble(0x55);
                    CMT2310A_SetSyncValue(0x55,0x55,0x55);
                    CMT2310A_SetDataMode(2); //2=packet mode
                    //CMT2310A_FreqHopping_Select(429175000);
                    CMT2310A_Freq_Select(429175000);
                    delay_nms(10);
                    //CMT2310A_DataRate_Select(RATE_4_8K);
                    for(Boot_i=0; Boot_i<UHF_LEN; Boot_i++)
                    {
                        radio_tx_buf[Boot_i] = 0x55;
                    }
                    vRadioWriteFifo(radio_tx_buf, UHF_LEN);
                    bRadioGoTx();
                }
                if(FG_test_tx_1010 == 1)
                {
                    if(CMT2310A_GPIO3 == 0)
                    {
                        Flag_TxDone = 0;
                        bRadioGoStandby();  //must
                        vRadioClearTxFifo();
                        vRadioClearInterrupt();
                        vRadioWriteFifo(radio_tx_buf, UHF_LEN); //must
                        bRadioGoTx();
                    }
                }
            }
        }

        if((Tx_Rx_mode==2)||(Tx_Rx_mode==3))
        {
            FG_test_rx=1;
            Receiver_LED_TX=0;
            FG_test_mode=0;
            FG_test_tx_on=0;
            FG_test_tx_1010=0;
            if(FG_test_tx_off==0)
            {
                CG2214M6_USE_R;
                FG_test_tx_off=1;
                delay_nms(100);
                bRadioGoStandby();
                vRadioClearTxFifo();
                vRadioClearInterrupt();
                CMT2310A_SetDataMode(0); //0=direct mode
                //CMT2310A_FreqHopping_Select(426075000);
                CMT2310A_Freq_Select(426075000);
                //CMT2310A_DataRate_Select(RATE_4_8K);
                delay_nms(10);
                bRadioGoRx();
            }
            if(Tx_Rx_mode==2)
            {
                if(TIMER1s==0)
                {
                    TIMER1s=500;
                    Receiver_LED_RX=!Receiver_LED_RX;
                }
            }
            if(Tx_Rx_mode==3)
            {
                RF_BRE_Check();
            }
        }
        PC_PRG();	       // PC控制
    }

    BerExtiUnInit();
    UART1_end();
    FG_test_rx=0;
    TIMER1s=0;
    Receiver_LED_TX=0;
    Receiver_LED_RX=0;
    FG_Receiver_LED_RX=0;
    Receiver_LED_OUT=0;

    FLAG_APP_RX=1;
    //dd_set_RX_mode();
    //TIME_Fine_Calibration=9000;
    //TIME_EMC=10;
}

void RF_BRE_Check(void)
{
    ClearWDT();
    if (X_COUNT >= 500)
    {
        if (X_ERR >= 25)
            Receiver_LED_RX = 0;
        else
            Receiver_LED_RX = 1;
        X_ERR = 0;
        X_COUNT = 0;
        X_ERRTimer = 1250;
    }
    if (X_ERRTimer == 0)
        Receiver_LED_RX = 0;
}
