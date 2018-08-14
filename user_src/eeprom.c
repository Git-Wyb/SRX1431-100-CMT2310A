/***********************************************************************/
/*  FILE        :eeprom.c                                              */
/*  DATE        :June, 2011                                            */
/*  DESCRIPTION :routine for VHF60-2011                                */
/*  CPU TYPE    :STM8S207C8                                            */
/*  Programmer	:Gong Dong Sheng                                       */
/*  Mark        :STM8S207C8的CODE空间为64K                             */
/*              :STM8S207C8的EEPROM的大小为1536字节,即:3页,512节/页    */
/***********************************************************************/
#include  <iostm8l151g6.h>				// CPU型号
//#include "stm8l15x.h"
#include "Pin_define.h"		// 管脚定义
#include "initial.h"		// 初始化  预定义
#include "ram.h"		// RAM定义
#include "eeprom.h"		// eeprom
#include "ID_Decode.h"
/***********************************************************************/
/*                    FLASH & EEPROM 寄存器及控制位                    */
/***********************************************************************/
#define FIRST_SECURITY_KEY        0xAE
#define SECOND_SECURITY_KEY       0x56 
#define ADD_EEPROM_S8             0x1000      

///* FLASH_CR2 */ 
//#define OPT               7   /* 对选项字节进行写操作 */ 
//#define WPRG              6   /* 字编程 */ 
//#define ERASE             5   /* 块擦除 */ 
//#define FPRG              4   /* 快速块编程 */  
////#define NC              3 
////#define NC              2 
////#define NC              1 
//#define PRG               0   /* 标准块编程 */ 
//
///* FLASH_NCR2 */ 
//#define NOPT              7   /* 对选项字节进行写操作 */ 
//#define NWPRG             6   /* 字编程 */ 
//#define NERASE            5   /* 块擦除 */ 
//#define NFPRG             4   /* 快速块编程 */ 
////#define NC              3 
////#define NC              2 
////#define NC              1 
//#define NPRG              0   /* 标准块编程 */ 
//
///* FLASH_FPR */ 
////#define NC              7 
////#define NC              6 
//#define WPB5              5   /* 用户启动代码保护位 */ 
//#define WPB4              4 
//#define WPB3              3
//#define WPB2              2
//#define WPB1              1
//#define WPB0              0 
//
///* FLASH_NFPR */  
////#define NC              7 
////#define NC              6 
//#define NWPB5             5   /* 用户启动代码保护位 */ 
//#define NWPB4             4
//#define NWPB3             3
//#define NWPB2             2
//#define NWPB1             1
//#define NWPB0             0 
//
///* FLASH_PUKR */ 
//#define MASS_PRG_KEY7     7   /* 主程序存储器解锁密钥 */ 
//#define MASS_PRG_KEY6     6
//#define MASS_PRG_KEY5     5
//#define MASS_PRG_KEY4     4
//#define MASS_PRG_KEY3     3
//#define MASS_PRG_KEY2     2
//#define MASS_PRG_KEY1     1
//#define MASS_PRG_KEY0     0
//
///* FLASH_DUKR */ 
//#define MASS_DATA_KEY7    7   /* DATA EEPROM解锁密钥 */
//#define MASS_DATA_KEY6    6
//#define MASS_DATA_KEY5    5
//#define MASS_DATA_KEY4    4
//#define MASS_DATA_KEY3    3
//#define MASS_DATA_KEY2    2
//#define MASS_DATA_KEY1    1
//#define MASS_DATA_KEY0    0
//
/* FLASH_IAPSR */ 
//#define NC              7 
#define HVOFF             6   /* 高压结束标志 */ 
//#define NC              5 
//#define NC              4 
#define DUL               3   /* DATA EEPROM区域解锁标志 */ 
#define EOP               2   /* 编程结束(写或擦除操作)标志 */ 
#define PUL               1   /* 快速程序存储器结束标志 */ 
#define WR_PG_DIS         0   /* 试图向被保护页进行写操作的标志 */ 

#define FLASH_CR1_RESET_VALUE   (( uchar )0x00 )
#define FLASH_CR2_RESET_VALUE   (( uchar )0x00 )
#define FLASH_NCR2_RESET_VALUE  (( uchar )0xFF )
#define FLASH_IAPSR_RESET_VALUE (( uchar )0x40 )
#define FLASH_PUKR_RESET_VALUE  (( uchar )0x00 )
#define FLASH_DUKR_RESET_VALUE  (( uchar )0x00 )



#define FLASH_PUKR_PUK          ((uchar)0xFF) /*!< Flash Program memory unprotection mask */
#define FLASH_DUKR_DUK          ((uchar)0xFF) /*!< Data EEPROM unprotection mask */

//#define UNLOCK_FLASH_TYPE       (( uchar )0x00 )
//#define UNLOCK_EEPROM_TYPE      (( uchar )0x01 )

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void InitialFlashReg( void ){					// 初始化闪存寄存器组
	FLASH_CR1 = FLASH_CR1_RESET_VALUE;
	FLASH_CR2 = FLASH_CR2_RESET_VALUE;
	//FLASH_NCR2 = FLASH_NCR2_RESET_VALUE;
	FLASH_IAPSR &= ( uchar )(~( 1 << DUL ));	// 清除只读DATA区解锁
	FLASH_IAPSR &= ( uchar )(~( 1 << PUL ));	// 清除程序区解锁
}
//------------------------------------------------
//  注: 2个密钥的操作序列正好相反  
void UnlockFlash(unsigned char Type){			// 解锁flash
	if( Type == UNLOCK_FLASH_TYPE){				// 解锁程序区
		FLASH_DUKR = SECOND_SECURITY_KEY;
		FLASH_DUKR = FIRST_SECURITY_KEY;  
	}
	else{										// 解锁eeprom
		FLASH_DUKR = FIRST_SECURITY_KEY;
		FLASH_DUKR = SECOND_SECURITY_KEY;
	}
}
//------------------------------------------------
void LockFlash(unsigned char Type ){			// 锁定存储器
	if( Type == UNLOCK_FLASH_TYPE ){
		FLASH_IAPSR &= ~( 1 << PUL );  
	}
	else{
		FLASH_IAPSR &= ~( 1 << DUL );
	}
}
//------------------------------------------------
uchar ReadByteEEPROM( ulong Addr ){				// 从eeprom中读取1字节
	return(*(( __far uchar* ) Addr ));			// Read byte
}
//------------------------------------------------
void WriteByteToFLASH(ulong Addr, uchar Dat){	// 写入一字节到eeprom
	*(( __far uchar * ) Addr ) = Dat;
}
//------------------------------------------------
void EraseByteFLASH( uint Addr){				// 擦除eeprom中内容
	*(( __near uchar * ) Addr) = 0x00; 
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void eeprom_save(void){
	unsigned char	i,eeprom_sys_buff;  
	
	UnlockFlash( UNLOCK_EEPROM_TYPE );
	for (i=0;i<16;i++)
		WriteByteToFLASH( addr_eeprom_sys+i, eeprom_sys_buff);
	LockFlash( UNLOCK_EEPROM_TYPE );
}  
void eeprom_sys_load(void){
	//unsigned char	i;  
	//for (i=0;i<0X1F;i++)
	//	eeprom_sys_buff[i] = ReadByteEEPROM( addr_eeprom_sys+i );
	//--------------------------------------
  
    UINT16 i,j;
    UINT8 xm[3]={0};
    uni_rom_id xn;
    for(i=0;i<256;i++)ID_Receiver_DATA[i]=0;//ID_Receiver_DATA[ID_DATA_PCS]=0;
    xm[0] = ReadByteEEPROM( addr_eeprom_sys+0x3FE );
    xm[1] = ReadByteEEPROM( addr_eeprom_sys+0x3FF );
    ID_DATA_PCS=xm[0]*256+xm[1];
    if(ID_DATA_PCS==0xFFFF)ID_DATA_PCS=0;
    else if(ID_DATA_PCS>256)ID_DATA_PCS=256;
    for(i=0;i<ID_DATA_PCS;i++){
        j=3*i;
        xm[0] = ReadByteEEPROM( addr_eeprom_sys+j);
	j++;
        xm[1] = ReadByteEEPROM( addr_eeprom_sys+j);
	j++;
        xm[2] = ReadByteEEPROM( addr_eeprom_sys+j);	
        xn.IDB[0]=0;
        xn.IDB[1]=xm[0];
        xn.IDB[2]=xm[1];
        xn.IDB[3]=xm[2];
        ID_Receiver_DATA[i]=xn.IDL;
    }
} 


void ID_EEPROM_write(void)
{
    UINT8 xm[3]={0};
    UINT16 m1;
    uni_rom_id xn;
     ID_DATA_PCS++;
     xm[0]=ID_DATA_PCS%256;
     xm[1]=ID_DATA_PCS/256;
     
     UnlockFlash( UNLOCK_EEPROM_TYPE );
	WriteByteToFLASH( addr_eeprom_sys+0x3FE, xm[1]);
	WriteByteToFLASH( addr_eeprom_sys+0x3FF, xm[0]);
     LockFlash( UNLOCK_EEPROM_TYPE );
	
     ID_Receiver_DATA[ID_DATA_PCS-1]=ID_Receiver_Login;
     xn.IDL=ID_Receiver_Login;
     xm[0]=xn.IDB[1];
     xm[1]=xn.IDB[2];
     xm[2]=xn.IDB[3];
     m1=(ID_DATA_PCS-1)*3;
     UnlockFlash( UNLOCK_EEPROM_TYPE );
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[0]);
	m1++;
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[1]);
	m1++;
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[2]);
     LockFlash( UNLOCK_EEPROM_TYPE );

}


void ID_EEPROM_write_0x00(void)
{
   UINT8 xm[3]={0};
   UINT16 m1;
     xm[0]=0;
     xm[1]=0;
     xm[2]=0;
     ID_Receiver_DATA[INquiry]=0x00;
     m1=INquiry*3;
     UnlockFlash( UNLOCK_EEPROM_TYPE );
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[0]);
	m1++;
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[1]);
	m1++;
	WriteByteToFLASH( addr_eeprom_sys+m1, xm[2]);
     LockFlash( UNLOCK_EEPROM_TYPE );
}





void ID_learn(void)
{
//    UINT16 i;
// #if defined(__Product_PIC32MX2_Receiver__)
 if(FG_10ms){
     FG_10ms = 0;

//     if(time_3sec)--time_3sec;
//     if(TIME_EMC)--TIME_EMC;

//     if(rssi_TIME)--rssi_TIME;
//     if(TIMER60s)--TIMER60s;
//     if(TIMER_err_1s)--TIMER_err_1s;
//     if(TIMER_Sensor_open_1s)--TIMER_Sensor_open_1s;
//     if(TIMER_Sensor_close_1s)--TIMER_Sensor_close_1s;
     if(TIME_Receiver_Login_restrict)--TIME_Receiver_Login_restrict;
       else if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1));
          else {TIME_Receiver_Login=0;COUNT_Receiver_Login=0;}

         if(Receiver_Login==0){
             TIME_Receiver_Login++;
             TIME_Receiver_Login_restrict=350;
             if(COUNT_Receiver_Login>=2){FLAG_ID_Login=1;TIME_Login_EXIT_rest=6000;}
             if(((FLAG_ID_Erase_Login==1)&&(COUNT_Receiver_Login>=1))||
                ((FLAG_ID_Login==1)&&(COUNT_Receiver_Login>=3)))ID_Login_EXIT_Initial();
         }
         if(Receiver_Login==1){
             if(TIME_Receiver_Login>3)COUNT_Receiver_Login++;
             if(FLAG_ID_Login_EXIT==1){FLAG_ID_Login_EXIT=0;COUNT_Receiver_Login=0;}
             TIME_Receiver_Login=0;
         }
         if(TIME_Receiver_Login>=300){
             TIME_Receiver_Login=0;
             FLAG_ID_Erase_Login=1;
             FLAG_ID_Erase_Login_PCS=1;    //追加多次ID登录
             TIME_Login_EXIT_rest=6000;
         }
         if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1)){
             TIME_Receiver_Login_led++;
             if(TIME_Receiver_Login_led>=30){
                 TIME_Receiver_Login_led=0;
                 Receiver_LED_OUT=!Receiver_LED_OUT;
             }
             if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0)){
                 //FLAG_ID_Login_OK_bank=1;             //追加多次ID登录
                 FLAG_ID_Login_OK=0;                   //追加多次ID登录
                 if(FLAG_IDCheck_OK==1)FLAG_IDCheck_OK=0;
                 else{
                     BEEP_and_LED();
                     TIME_Login_EXIT_rest=6000;       //追加多次ID登录
                     if(FLAG_ID_Login==1)ID_EEPROM_write();
                     else if(FLAG_ID_Erase_Login==1){
                         if(FLAG_ID_Erase_Login_PCS==1){FLAG_ID_Erase_Login_PCS=0;ID_DATA_PCS=0;}      //追加多次ID登录
                         ID_EEPROM_write();
                     }
                 }//end else
             }//  end  if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0))
             if(TIME_Login_EXIT_rest)--TIME_Login_EXIT_rest;
              else ID_Login_EXIT_Initial();
         } //end if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1))
 }
//#endif

}
	



void ID_Login_EXIT_Initial(void)
{
// #if defined(__Product_PIC32MX2_Receiver__)
     FLAG_ID_Login_EXIT=1;
     FLAG_ID_Login_OK=0;
     FLAG_ID_Login_OK_bank=0;
     FLAG_ID_Login=0;
     FLAG_ID_Erase_Login=0;
     Receiver_LED_OUT=0;
//#endif
//#if defined(__Product_PIC32MX2_WIFI__)
//     FLAG_ID_Login_EXIT=1;
//     FLAG_ID_Login_OK=0;
//     FLAG_ID_Login_OK_bank=0;
//     FLAG_ID_Login=0;
//     FLAG_ID_Erase_Login=0;
//     WIFI_LED_RX=0;
//#endif
}
