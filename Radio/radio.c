#include "radio.h"
#include "ram.h"
#include "Pin_define.h"

CMT2310A_CFG	g_radio = {0};					//
uint32_t 		g_chip_id = 0x00000000;
Wireless_Body Struct_DATA_Packet_Contro,Struct_DATA_Packet_Contro_buf;
Wireless_Body Uart_Struct_DATA_Packet_Contro,Last_Uart_Struct_DATA_Packet_Contro;
u8 Struct_DATA_Packet_Contro_fno=0;

unsigned char g_reg_read_buf[128];
u8 radio_tx_buf[UHF_LEN] = {0};
u8 radio_rx_buf[UHF_LEN] = {0};

u8 SPI_RECEIVE_BUFF[SPI_REV_BUFF_LONG] = {0};
u32 SPI_Receive_DataForC[7] = {0};
u8 APP_TX_freq = 0;
u8 Radio_Date_Type = 1;

/*
RSSI Compare TH = -90 dBm --> g_cmt2310a_page1[99] = 0xA6;
RSSI Compare TH = -98 dBm --> g_cmt2310a_page1[99] = 0x9E;
RSSI Compare TH = -99 dBm --> g_cmt2310a_page1[99] = 0x9D;
RSSI Compare TH = -100 dBm --> g_cmt2310a_page1[99] = 0x9C;
RSSI Compare TH = -101 dBm --> g_cmt2310a_page1[99] = 0x9B;
RSSI Compare TH = -102 dBm --> g_cmt2310a_page1[99] = 0x9A;
RSSI Compare TH = -103 dBm --> g_cmt2310a_page1[99] = 0x99;
RSSI Compare TH = -104 dBm --> g_cmt2310a_page1[99] = 0x98;
RSSI Compare TH = -105 dBm --> g_cmt2310a_page1[99] = 0x97;
RSSI Compare TH = -106 dBm --> g_cmt2310a_page1[99] = 0x96;
RSSI Compare TH = -107 dBm --> g_cmt2310a_page1[99] = 0x95;
RSSI Compare TH = -108 dBm --> g_cmt2310a_page1[99] = 0x94;
RSSI Compare TH = -109 dBm --> g_cmt2310a_page1[99] = 0x93;
RSSI Compare TH = -110 dBm --> g_cmt2310a_page1[99] = 0x92;
RSSI Compare TH = -111 dBm --> g_cmt2310a_page1[99] = 0x91;
RSSI Compare TH = -112 dBm --> g_cmt2310a_page1[99] = 0x90;
RSSI Compare TH = -113 dBm --> g_cmt2310a_page1[99] = 0x8F;
RSSI Compare TH = -114 dBm --> g_cmt2310a_page1[99] = 0x8E;
RSSI Compare TH = -115 dBm --> g_cmt2310a_page1[99] = 0x8D;
RSSI Compare TH = -116 dBm --> g_cmt2310a_page1[99] = 0x8C;
RSSI Compare TH = -117 dBm --> g_cmt2310a_page1[99] = 0x8B;
RSSI Compare TH = -120 dBm --> g_cmt2310a_page1[99] = 0x88;
RSSI Compare TH = -127 dBm --> g_cmt2310a_page1[99] = 0x81;
*/

/******************************
**Name:  vRadioInit
**Func:  Radio config spi & reset
**Input: None
*Output: None
********************************/
void RF_CMT2310A_Init(u8 mode)
{   /*
    while(1)
	{
        vRadioSoftReset();
        vRadioPowerUpBoot();
        delay1ms(10);
        g_chip_id = lRadioChipVersion();
        if(0x00231000==(g_chip_id&0x00FFFF00))
            break;
    }*/

    vRadioInit(mode);
}

void vRadioInit(u8 mode)
{
	//byte fw_rev;
	vRadioSoftReset();
    g_cmt2310a_page1[99] = 0x8D; //RSSI Compare TH = -115 dBm
	vRadioConfigPageReg(0, g_cmt2310a_page0, CMT2310A_PAGE0_SIZE);		//config page 0
	vRadioConfigPageReg(1, g_cmt2310a_page1, CMT2310A_PAGE1_SIZE);   	//config page 1

	vRadioSetNirq(CMT2310A_nIRQ_SEL);	//for TCXO need cofig as nIRQ pin at first
	//vRadioTcxoDrvSel(0);				//drive power
    /*
	fw_rev = (byte)g_chip_id;			//dealwith Xtal
	switch(fw_rev)
		{
		case 0xC0: vRadioXoWaitCfg(RADIO_CGU_DIV4); break;
		default: break;
		}
          */

	vRadioPowerUpBoot();
	delay1ms(10);

	bRadioGoStandby();
	delay1ms(2);
	bRadioApiCommand(0x02);				//
	delay1ms(10);
	bRadioApiCommand(0x01);				//IR Calibration, need some times

	//vRadioCapLoad(2);					//Xo Cap


    if(mode == 1) //test mode
    {
        //GPIOn default setting
        vRadioSetGpio0(CMT2310A_GPIO0_DCLK);
        vRadioSetGpio1(CMT2310A_GPIO1_DOUT);
        vRadioSetGpio2(CMT2310A_GPIO2_SEL);
        vRadioSetGpio3(CMT2310A_GPIO3_INT1);
        vRadioSetGpio4(CMT2310A_GPIO4_DCLK);//CMT2310A_GPIO4_DCLK
        vRadioSetGpio5(CMT2310A_GPIO5_DOUT);//CMT2310A_GPIO5_DOUT
        vRadioSetInt1Polar(TRUE); //FALSE:Interrupt ative-high,TRUE:Interrupt ative-low
        vRadioSetInt2Polar(TRUE);
        vRadioSetInt3Polar(TRUE);
    }
    else //work mode
    {
        //GPIOn default setting
        vRadioSetGpio0(CMT2310A_GPIO0_SEL);
        vRadioSetGpio1(CMT2310A_GPIO1_SEL);
        vRadioSetGpio2(CMT2310A_GPIO2_INT2);
        vRadioSetGpio3(CMT2310A_GPIO3_INT1);
        vRadioSetGpio4(CMT2310A_GPIO4_SEL);
        vRadioSetGpio5(CMT2310A_GPIO5_SEL);

        //INT1 = RX_FIFO_WBYTE,   INT2 = PKT_DONE
        vRadioSetInt1Sel(INT_SRC_PKT_DONE);
        vRadioSetInt2Sel(INT_SRC_TX_DONE);
        vRadioSetInt1Polar(TRUE); //FALSE:Interrupt ative-high,TRUE:Interrupt ative-low
        vRadioSetInt2Polar(TRUE);
        vRadioSetInt3Polar(TRUE);
    }
        //interrupt source enable config
        g_radio.int_src_en._BITS.PKT_DONE_EN   		= 1;
        g_radio.int_src_en._BITS.CRC_PASS_EN   		= 0;
        g_radio.int_src_en._BITS.ADDR_PASS_EN  		= 0;
        g_radio.int_src_en._BITS.SYNC_PASS_EN  		= 1;
        g_radio.int_src_en._BITS.PREAM_PASS_EN 		= 1;
        g_radio.int_src_en._BITS.TX_DONE_EN    		= 1;
        g_radio.int_src_en._BITS.RX_TOUT_EN    		= 0;
        g_radio.int_src_en._BITS.LD_STOP_EN    		= 0;
        g_radio.int_src_en._BITS.LBD_STOP_EN   		= 0;
        g_radio.int_src_en._BITS.LBD_STAT_EN   		= 0;
        g_radio.int_src_en._BITS.PKT_ERR_EN    		= 0;
        g_radio.int_src_en._BITS.RSSI_COLL_EN  		= 0;
        g_radio.int_src_en._BITS.OP_CMD_FAILED_EN 	= 0;
        g_radio.int_src_en._BITS.RSSI_PJD_EN   		= 0;
        g_radio.int_src_en._BITS.SEQ_MATCH_EN  		= 0;
        g_radio.int_src_en._BITS.NACK_RECV_EN       = 0;
        g_radio.int_src_en._BITS.TX_RESEND_DONE_EN  = 0;
        g_radio.int_src_en._BITS.ACK_RECV_FAILED_EN = 0;
        g_radio.int_src_en._BITS.TX_DC_DONE_EN      = 0;
        g_radio.int_src_en._BITS.CSMA_DONE_EN       = 0;
        g_radio.int_src_en._BITS.CCA_STAT_EN        = 0;
        g_radio.int_src_en._BITS.API_DONE_EN        = 0;
        g_radio.int_src_en._BITS.TX_FIFO_TH_EN		= 1;
        g_radio.int_src_en._BITS.TX_FIFO_NMTY_EN	= 1;
        g_radio.int_src_en._BITS.TX_FIFO_FULL_EN	= 1;
        g_radio.int_src_en._BITS.RX_FIFO_OVF_EN		= 0;
        g_radio.int_src_en._BITS.RX_FIFO_TH_EN		= 0;
        g_radio.int_src_en._BITS.RX_FIFO_NMTY_EN	= 0;
        g_radio.int_src_en._BITS.RX_FIFO_FULL_EN 	= 0;
        vRadioInterruptSoucreCfg(&g_radio.int_src_en);

        //packet preamble config
        g_radio.preamble_cfg.PREAM_LENG_UNIT = 0;					//8-bits mode
        g_radio.preamble_cfg.PREAM_VALUE     = 0x55;				//
        g_radio.preamble_cfg.RX_PREAM_SIZE   = 3;					//2*8 = 16bit
        g_radio.preamble_cfg.TX_PREAM_SIZE   = 12;                  //12*8 = 96bit
        vRadioCfgPreamble(&g_radio.preamble_cfg);
        /*
        //packet syncword config
        g_radio.sync_cfg.SYN_CFG_u._BITS.SYNC_MAN_EN   = 0;			//disable syncword manchester coding
        g_radio.sync_cfg.SYN_CFG_u._BITS.SYNC_SIZE     = 2;			//(N+1).enable 3 bytes for syncword
        g_radio.sync_cfg.SYN_CFG_u._BITS.SYNC_TOL      = 0;
        g_radio.sync_cfg.SYN_CFG_u._BITS.SYNC_MODE_SEL = 0;			//normal packet
        g_radio.sync_cfg.SYNC_VALUE[0] = 0x55;
        g_radio.sync_cfg.SYNC_VALUE[1] = 0x55;
        g_radio.sync_cfg.SYNC_VALUE[2] = 0x55;
        g_radio.sync_cfg.SYNC_VALUE_SEL= 0;							//select SYN_VAL
        vRadioCfgSyncWord(&g_radio.sync_cfg); */

    /*
	//packet node address config
	g_radio.addr_cfg.ADDR_CFG_u._BITS.ADDR_DET_MODE = 0;		//disable Node Address
	vRadioCfgNodeAddr(&g_radio.addr_cfg);

	//packet crc config
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_EN         = 0;			//enable crc
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_BIT_ORDER  = 0;
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_REFIN      = 0;
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_RANGE      = 0;
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_BIT_INV    = 0;
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_BYTE_SWAP  = 0;
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_REFOUT     = 0;			//whole payload
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRCERR_CLR_FIFO_EN  = 0;	//note: need ative FIFO_AUTO_CLR_RX_EN = 1 or call vRadioFifoAutoClearGoRx(1)
	g_radio.crc_cfg.CRC_CFG_u._BITS.CRC_SIZE       = 1;			//crc-16 mode
	g_radio.crc_cfg.CRC_POLY_u.u32_POLY = 0x10210000;
	g_radio.crc_cfg.CRC_SEED_u.u32_SEED = 0x00000000;
	vRadioCfgCrc(&g_radio.crc_cfg);

	//packet coding format
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.MANCH_EN          = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.MANCH_TYPE        = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.WHITEN_EN         = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.WHITEN_TYPE       = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.WHITEN_SEED_TYP   = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.FEC_EN            = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.FEC_RSC_NRNSC_SEL = 0;
	g_radio.coding_format_cfg.CODING_FORMAT_CFG_u._BITS.FEC_TICC          = 0;
	g_radio.coding_format_cfg.WHITEN_SEED  = 0x01FF;
	g_radio.coding_format_cfg.FEC_PAD_CODE = 0;
	vRadioCfgCodeFormat(&g_radio.coding_format_cfg);

	//packet frame format
	g_radio.frame_cfg.DATA_MODE = 2;								//0=direct mode, 	2=packet mode
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.PKT_TYPE 	       = 0;		//fixd-length packet mode
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.PAYLOAD_BIT_ORDER = 0;		//msb first
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.ADDR_LEN_CONF     = 0;
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.PAGGYBACKING_EN   = 0;
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.LENGTH_SIZE 	   = 0;
	g_radio.frame_cfg.FRAME_CFG1_u._BITS.INTERLEAVE_EN     = 0;		//note: when FEC enable, INTERLEAVE_EN should be set 1

	g_radio.frame_cfg.FRAME_CFG2_u._BITS.TX_PREFIX_TYPE    = TX_PREFIX_SEL_PREAMBLE;		//transmit preamble
	g_radio.frame_cfg.FRAME_CFG2_u._BITS.SEQNUM_EN  	   = 0;
	g_radio.frame_cfg.FRAME_CFG2_u._BITS.SEQNUM_AUTO_INC   = 0;
	g_radio.frame_cfg.FRAME_CFG2_u._BITS.SEQNUM_SIZE	   = 0;
	g_radio.frame_cfg.FRAME_CFG2_u._BITS.SEQNUM_MACH_EN    = 0;
	g_radio.frame_cfg.FRAME_CFG2_u._BITS.FCS2_EN    	   = 0;

	g_radio.frame_cfg.TX_PKT_NUM     = 0;
	g_radio.frame_cfg.TX_PKT_GAP     = 0;
	g_radio.frame_cfg.FCS2_TX_IN     = 0;
	g_radio.frame_cfg.PAYLOAD_LENGTH = UHF_LEN;
	vRadioCfgFrameFormat(&g_radio.frame_cfg);
    */
	//Run Mode Config
	g_radio.word_mode_cfg.WORK_MODE_CFG1_u._BITS.TX_DC_EN          = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG1_u._BITS.TX_ACK_EN         = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG1_u._BITS.TX_DC_PERSIST_EN  = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG1_u._BITS.TX_AUTO_HOP_EN    = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG1_u._BITS.TX_EXIT_STATE     = EXIT_TO_READY;

	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.RX_DC_EN          = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.RX_AUTO_HOP_EN    = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.RX_ACK_EN         = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.RX_TIMER_EN       = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.RX_EXIT_STATE     = EXIT_TO_READY; //wei:Must be set,otherwise the test sensitivity cannot be received
	g_radio.word_mode_cfg.WORK_MODE_CFG2_u._BITS.CSMA_EN           = 0;

	g_radio.word_mode_cfg.WORK_MODE_CFG3_u._BITS.PKT_DONE_EXIT_EN  = 0;			//depend on RX_EXIT_STATE
	g_radio.word_mode_cfg.WORK_MODE_CFG3_u._BITS.RX_HOP_SLP_MODE   = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG3_u._BITS.SLP_MODE          = 0;

	g_radio.word_mode_cfg.WORK_MODE_CFG4_u._BITS.LFCLK_OUT_EN      = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG4_u._BITS.LFCLK_SEL         = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG4_u._BITS.SLEEP_TIMER_EN    = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG4_u._BITS.TIMER_RAND_MODE   = 0;

	g_radio.word_mode_cfg.WORK_MODE_CFG5_u._BITS.CSMA_CCA_MODE     = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG5_u._BITS.CSMA_CCA_WIN_SEL  = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG5_u._BITS.CSMA_CCA_INT_SEL  = 0;
	g_radio.word_mode_cfg.WORK_MODE_CFG5_u._BITS.CSMA_PERSIST_EN   = 0;

	g_radio.word_mode_cfg.FREQ_CHANL_NANU  = 0;
	g_radio.word_mode_cfg.FREQ_DONE_TIMES  = 0;
	g_radio.word_mode_cfg.FREQ_SPACE       = 25;  //set FREQ_SPACE = 25KHz.
	g_radio.word_mode_cfg.FREQ_TIMES       = 0;
	g_radio.word_mode_cfg.SLEEP_TIMER_M    = 0;
	g_radio.word_mode_cfg.SLEEP_TIMER_R    = 0;
	g_radio.word_mode_cfg.RX_TIMER_T1_M    = 0;					//M*2^(R+1)*5us=M*2^R*10us,
	g_radio.word_mode_cfg.RX_TIMER_T1_R    = 0;					//R=7, unit=0.64ms
	g_radio.word_mode_cfg.RX_TIMER_T2_M    = 0;
	g_radio.word_mode_cfg.RX_TIMER_T2_R    = 0;
	g_radio.word_mode_cfg.RX_TIMER_CSMA_M  = 0;
	g_radio.word_mode_cfg.RX_TIMER_CSMA_R  = 0;
	g_radio.word_mode_cfg.TX_DC_TIMES      = 0;
	g_radio.word_mode_cfg.TX_RS_TIMES      = 0;
	g_radio.word_mode_cfg.CSMA_TIMES       = 0;
	g_radio.word_mode_cfg.SLEEP_TIMER_CSMA_M = 0;
	g_radio.word_mode_cfg.SLEEP_TIMER_CSMA_R = 0;
    g_radio.word_mode_cfg.WORK_MODE_CFG6_u._BITS.FREQ_HOP_MANU_EN = 1;  //Frequency hopping setting
	vRadioCfgWorkMode(&g_radio.word_mode_cfg);

	//FIFO Init
	vRadioFifoMerge(FALSE);
	vRadioSetFifoTH(30);
	vRadioClearRxFifo();										//reset & clear fifo
	vRadioClearTxFifo();

	vRadioFifoAutoClearGoRx(TRUE);								//when crc error, need to auto clear fifo, should enable

	vRadioRssiUpdateSel(CMT2310A_RSSI_UPDATE_ALWAYS);           //Read Rssi must set

	vRadioSetAntSwitch(FALSE, FALSE);							//

	//vRadioDcdcCfg(TRUE);										//dc-dc off
}

void vRadioClearInterrupt(void)
{
	vRadioInterruptSoucreFlag(&g_radio.int_src_flag);

	g_radio.int_src_clear._BITS.SLEEP_TMO_CLR = g_radio.int_src_flag._BITS.SLEEP_TMO_FLG;
	g_radio.int_src_clear._BITS.RX_TMO_CLR    = g_radio.int_src_flag._BITS.RX_TMO_FLG;
	g_radio.int_src_clear._BITS.TX_DONE_CLR   = g_radio.int_src_flag._BITS.TX_DONE_FLG;

	g_radio.int_src_clear._BITS.PKT_DONE_CLR  = g_radio.int_src_flag._BITS.PKT_DONE_FLG;
	g_radio.int_src_clear._BITS.CRC_PASS_CLR  = g_radio.int_src_flag._BITS.CRC_PASS_FLG;
	g_radio.int_src_clear._BITS.ADDR_PASS_CLR = g_radio.int_src_flag._BITS.ADDR_PASS_FLG;
	g_radio.int_src_clear._BITS.SYNC_PASS_CLR = g_radio.int_src_flag._BITS.SYNC_PASS_FLG|g_radio.int_src_flag._BITS.SYNC1_PASS_FLG;
	g_radio.int_src_clear._BITS.PREAM_PASS_CLR= g_radio.int_src_flag._BITS.PREAM_PASS_FLG;

	g_radio.int_src_clear._BITS.LBD_STAT_CLR      = g_radio.int_src_flag._BITS.LBD_STATUS_FLG;
	g_radio.int_src_clear._BITS.PKT_ERR_CLR       = g_radio.int_src_flag._BITS.PKT_ERR_FLG;
	g_radio.int_src_clear._BITS.RSSI_COLL_CLR     = g_radio.int_src_flag._BITS.RSSI_COLL_FLG;
	g_radio.int_src_clear._BITS.OP_CMD_FAILED_CLR = g_radio.int_src_flag._BITS.OP_CMD_FAILED_FLG;
	g_radio.int_src_clear._BITS.ANT_LOCK_CLR      = g_radio.int_src_flag._BITS.ANT_LOCK_FLG;

	g_radio.int_src_clear._BITS.SEQ_MATCH_CLR       = g_radio.int_src_flag._BITS.SEQ_MATCH_FLG;
	g_radio.int_src_clear._BITS.NACK_RECV_CLR       = g_radio.int_src_flag._BITS.NACK_RECV_FLG;
	g_radio.int_src_clear._BITS.TX_RESEND_DONE_CLR  = g_radio.int_src_flag._BITS.TX_RESEND_DONE_FLG ;
	g_radio.int_src_clear._BITS.ACK_RECV_FAILED_CLR = g_radio.int_src_flag._BITS.ACK_RECV_FAILED_FLG;
	g_radio.int_src_clear._BITS.TX_DC_DONE_CLR      = g_radio.int_src_flag._BITS.TX_DC_DONE_FLG;
	g_radio.int_src_clear._BITS.CSMA_DONE_CLR       = g_radio.int_src_flag._BITS.CSMA_DONE_FLG;
	g_radio.int_src_clear._BITS.CCA_STATUS_CLR      = g_radio.int_src_flag._BITS.CCA_STATUS_FLG;
	g_radio.int_src_clear._BITS.API_DONE_CLR        = g_radio.int_src_flag._BITS.API_DONE_FLG;

 	vRadioInterruptSoucreClear(&g_radio.int_src_clear);
}

void vRadioReadAllStatus(void)
{
	bRadioGetState();									//read work status
	vRadioFifoGetStatus(&g_radio.fifo_status_flag);		//read fifo status
	vRadioInterruptSoucreFlag(&g_radio.int_src_flag);	//read interrupt flag

	bRadioReadReg(CMT2310A_CTL_REG_04);					//get GPIO1/GPIO0 selection
	bRadioReadReg(CMT2310A_CTL_REG_05);					//get GPIO3/GPIO2 selection
	bRadioReadReg(CMT2310A_CTL_REG_06);					//get GPIO5/GPIO4 selection

	bRadioReadReg(CMT2310A_CTL_REG_16);					//get INT1 selection
	bRadioReadReg(CMT2310A_CTL_REG_17);					//get INT2 selection
}

void vRadioCmpReg(byte const wr_ptr[], byte rd_ptr[], byte cmp_ptr[], byte length)
{
	byte i;

	for(i=0; i<length; i++)
		{
		if(wr_ptr[i]!=rd_ptr[i])
			cmp_ptr[i] = 0xFF;
		else
			cmp_ptr[i] = 0x00;
		}
}

void CMT2310A_FreqHopping_Set(void) //Frequency hopping setting
{
    bSpiWriteByte(CMT2310A_CTL_REG_12,25); //set FREQ_SPACE = 25KHz.
    CMT2310A_SetReg_Bits(0,CMT2310A_CTL_REG_22,1,7);//set FREQ_HOP_MANU_EN = 1
}

void CMT2310A_FreqHopping_Select(u32 freq)
{   /*FREQ = BASE_FREQ + 1KHz X FREQ_SPACE X FREQ_CHANNEL_MANU,
      BASE_FREQ = 426075000; FREQ_SPACE = 25.*/
    u8 num = (freq - 426075000) / 25000;
    bSpiWriteByte(CMT2310A_CTL_REG_03,num);
    /*
    switch(freq)
    {
        case 426075000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,0);
            break;

        case 426750000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,27);
            break;

        case 429175000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,124);
            break;

        case 429200000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,125);
            break;

        case 429350000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,131);
            break;

        case 429550000:
            bSpiWriteByte(CMT2310A_CTL_REG_03,139);
            break;

        default:
            bSpiWriteByte(CMT2310A_CTL_REG_03,0);
            break;
    } */
}

u8 rreg = 0;
void CMT2300A_PramePass(void)
{
    rreg = CMT2310A_ReadReg(0,0x1A);

    if((rreg & 0x10) == 0x10) //PREAM_PASS_FLG
    {
        TIMER18ms = 550;
        Flag_FREQ_Scan = 1;
        FG_Receiver_LED_RX = 1;
        TIMER300ms = 500;
        CG2214M6_USE_R;
        //Not needed here  bRadioGoStandby()
        if(PROFILE_CH_FREQ_32bit_200002EC == 426075000)
        {
            if(Flag_TX_ID_load == 0)
            {
                CMT2310A_SetDataLength(12);
            }
            else
            {
                CMT2310A_SetDataLength(24);
            }
        }
        vRadioClearInterrupt();
        bRadioGoRx();
    }
    rreg = 0;
    EXTI_SR1_P1F = 1;
}
void CMT2310A_SetRx(void)
{
    CG2214M6_USE_R;
    CMT2310A_GPIO3_INT1_OFF();
    CMT2310A_GPIO2_INT2_OFF();
    bRadioGoStandby();
    CMT2310A_SetDataLength(UHF_LEN);
    vRadioSetInt1Sel(INT_SRC_PREAM_PASS);//INT_SRC_PKT_DONE ,INT_SRC_PREAM_PASS,INT_SRC_RSSI_PJD_VALID
    vRadioSetInt2Sel(INT_SRC_PKT_DONE);
    //CMT2310A_Freq_Select(426075000);
    CMT2310A_FreqHopping_Select(426075000);
    CMT2310A_DataRate_Select(RATE_1_2K);
    bRadioGoRx();
    CMT2310A_GPIO3_INT1_ON();
    CMT2310A_GPIO2_INT2_ON();
}

void CMT2310A_SetTx(void)
{
    CG2214M6_USE_T;
    CMT2310A_GPIO3_INT1_OFF();
    CMT2310A_GPIO2_INT2_OFF();
    bRadioGoStandby();
    //CMT2310A_Freq_Select(429350000);
    CMT2310A_FreqHopping_Select(429350000);
    CMT2310A_DataRate_Select(RATE_4_8K);
    vRadioSetInt2Sel(INT_SRC_TX_DONE);
    CMT2310A_SetDataLength(28);
    CMT2310A_GPIO3_INT1_ON();
    CMT2310A_GPIO2_INT2_ON();
}

void CMT2300A_ReadData(u8 *rxbuff,u8 len)
{
    vRadioReadFifo(rxbuff, len);
    vRadioClearRxFifo();
    vRadioClearInterrupt();
    bRadioGoRx();
}

void CMT2300A_TxData(u8 *txbuff,u8 len)
{
    vRadioWriteFifo(txbuff, len);
    bRadioGoTx();
}
void CMT2300A_TRxDone(void)
{
    EXTI_SR1_P0F = 1;

    rreg = CMT2310A_ReadReg(0,0x1A);
    if((rreg & 0x01) == 0x01) //PKT_DONE_FLG
    {
        Flag_RxDone = 1;
        vRadioClearInterrupt();
    }

    rreg = CMT2310A_ReadReg(0,0x18);
    if((rreg & 0x08) == 0x08) //TX_DONE_FLG
    {
        Flag_TxDone = 1;
        Time_APP_blank_TX = 10;
        //bRadioGoStandby();
        vRadioClearTxFifo();
        vRadioClearInterrupt();
    }
    rreg = 0;
}
void CMT2300A_TxDone(void)
{

}

u8 CMT2310A_ReadReg(u8 page,u8 addr)
{
    u8 read = 0;
    vRadioRegPageSel(page);
    read = bSpiReadByte(addr);
    vRadioRegPageSel(0);
    return read;
}

u8 CMT2310A_WriteReg(u8 page,u8 addr,u8 val)
{
    u8 read = 0;
    vRadioRegPageSel(page);
    bSpiWriteByte(addr,val);
    vRadioRegPageSel(0);
    return read;
}

void CMT2310A_SetSyncValue(u8 sync2,u8 sync1,u8 sync0)
{
    CMT2310A_WriteReg(0,0x34,sync2);
    CMT2310A_WriteReg(0,0x33,sync1);
    CMT2310A_WriteReg(0,0x32,sync0);
}

void CMT2310A_SetPreamble(u8 pram)
{
    g_radio.preamble_cfg.PREAM_VALUE  = pram;
    vRadioCfgPreamble(&g_radio.preamble_cfg);
}

void CMT2310A_SetDataMode(u8 mode)
{
    if(mode == 0) CMT2310A_WriteReg(0,0x28,CMT2310A_ReadReg(0,0x28) & 0xFC); //0 = direct mode.
    else CMT2310A_WriteReg(0,0x28,(CMT2310A_ReadReg(0,0x28) & 0xFC) | mode); //2 = Packet mode;1/3 = NA.
}

void CMT2310A_SetDataLength(u8 len)
{
    g_radio.frame_cfg.PAYLOAD_LENGTH = len;
    vRadioSetPayloadLength(&g_radio.frame_cfg);
}

void CMT2310A_SetReg_Bits(u8 page,u8 addr,u8 val,u8 bits)
{
    u8 readval = 0;
    if(bits > 7) bits = 7;
    vRadioRegPageSel(page);
    readval = bSpiReadByte(addr);
    if(val == 0) readval &= ~(0x01 << bits);
    else readval |= (0x01 << bits);
    bSpiWriteByte(addr,readval);
    vRadioRegPageSel(0);
}

u8 CMT2310A_Get_RSSI(void)
{
    return CMT2310A_ReadReg(0,0x22);//rssi
}

void CMT2310A_Freq_Scanning(void)
{
    if(TIMER18ms == 0)
    {
        if(Flag_FREQ_Scan)  return;
        CMT2310A_Change_Channel();
        CG2214M6_USE_R;
        bRadioGoRx();

        if(Radio_Date_Type==1)
            TIMER18ms = 25;
        else if(Radio_Date_Type > 1)
            TIMER18ms = 18;
        FLAG_APP_TX = 0;
        Flag_TxEn = 0;
        RSSI_Read_Counter = 0;
        RAM_RSSI_SUM = 0;
    }
    else Flag_FREQ_Scan = 0;
}

void CMT2310A_Change_Channel(void)
{
    if ((FLAG_ID_Erase_Login == 1)||(FLAG_ID_Login == 1)||(FLAG_ID_SCX1801_Login==1))
    {
        PROFILE_CH_FREQ_32bit_200002EC = 426075000;
        Radio_Date_Type = 1;
        Channels = 1;
        CMT2310A_Frequency_Set(PROFILE_CH_FREQ_32bit_200002EC,Radio_Date_Type);
    }
    else
    {
        switch(Channels)
        {
            case 1:
                    Radio_Date_Type = 1;
                    PROFILE_CH_FREQ_32bit_200002EC = 426075000;
                    CMT2310A_Frequency_Set(PROFILE_CH_FREQ_32bit_200002EC,Radio_Date_Type);
                    if(ID_SCX1801_DATA == 0) Channels = 1;
                    else Channels = 2;
                    break;

            case 2:
                    Radio_Date_Type = 2;
                    PROFILE_CH_FREQ_32bit_200002EC = PROFILE_CH1_FREQ_32bit_429HighSpeed;
                    CMT2310A_Frequency_Set(PROFILE_CH_FREQ_32bit_200002EC,Radio_Date_Type);
                    Channels = 3;
                    break;

            case 3:
                    Radio_Date_Type = 2;
                    PROFILE_CH_FREQ_32bit_200002EC = PROFILE_CH2_FREQ_32bit_429HighSpeed;
                    CMT2310A_Frequency_Set(PROFILE_CH_FREQ_32bit_200002EC,Radio_Date_Type);
                    Channels = 4;
                    break;

             case 4:
                    Radio_Date_Type = 1;
                    PROFILE_CH_FREQ_32bit_200002EC = 426075000;
                    CMT2310A_Frequency_Set(PROFILE_CH_FREQ_32bit_200002EC,Radio_Date_Type);
                    Channels = 1;
                    break;

             default:
                break;
        }
    }
}

void CMT2310A_Frequency_Set(u32 freq,u8 radio_type)
{
    ClearWDT();
    CMT2310A_GPIO3_INT1_OFF();
    CMT2310A_GPIO2_INT2_OFF();
    bRadioGoStandby();
    //CMT2310A_Freq_Select(freq);
    CMT2310A_FreqHopping_Select(freq);
    if(radio_type == 1)
    {
        CMT2310A_SetDataLength(12);
        CMT2310A_DataRate_Select(RATE_1_2K);
    }
    else if(radio_type == 2)
    {
        CMT2310A_SetDataLength(28);
        CMT2310A_DataRate_Select(RATE_4_8K);
    }
    CMT2310A_GPIO3_INT1_ON();
    CMT2310A_GPIO2_INT2_ON();
}

void ID_Decode_IDCheck(void);
void RX_ANALYSIS(void)
{
    u8 i;
    for (i = 0; i < 7; i++)
    {
        SPI_Receive_DataForC[i] = (u32)SPI_RECEIVE_BUFF[i * 4 + 3] |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 2] << 8 |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 1] << 16 |
                                  (u32)SPI_RECEIVE_BUFF[i * 4 + 0] << 24;
    }

    FLAG_Receiver_IDCheck = 1;
}

void APP_TX_PACKET(void)
{
    if(Flag_FREQ_Scan == 0 && Flag_TxEn == 1 && Time_APP_blank_TX == 0 && FLAG_ID_Erase_Login == 0 && FLAG_ID_Login == 0)
        //&& (PROFILE_CH_FREQ_32bit_200002EC == 429350000 || PROFILE_CH_FREQ_32bit_200002EC == 429550000))
    {
        Flag_TxEn = 0;
        Uart_Struct_DATA_Packet_Contro.Fno_Type.UN.fno = 0;
        Last_Uart_Struct_DATA_Packet_Contro = Uart_Struct_DATA_Packet_Contro;
        Last_Uart_Struct_DATA_Packet_Contro.Fno_Type.UN.type=1;
        if(RAM_RSSI_AVG >= 100)
        {
            rssi = 255 - RAM_RSSI_AVG;
        }
        else rssi = RAM_RSSI_AVG;

        Last_Uart_Struct_DATA_Packet_Contro.data[1].uc[0]= rssi;
        //DATA_Packet_ID = 12345678;
        //TX_DataLoad(DATA_Packet_ID,Struct_DATA_Packet_Contro_fno, &CONST_TXPACKET_DATA_20000AF0[0]);
        TX_DataLoad_HighSpeed(DATA_Packet_ID,Last_Uart_Struct_DATA_Packet_Contro,&CONST_TXPACKET_DATA_20000AF0[0]);
        CMT2310A_SetTx();
        FLAG_APP_TX = 1;
        APP_TX_freq = 0;
        Time_Tx_Out = 1000;
        RAM_RSSI_SUM = 0;
        RAM_RSSI_AVG = 0;
        RSSI_Read_Counter = 0;
    }
    if(FLAG_APP_TX)
    {
        if(APP_TX_freq == 0)
        {
            Receiver_LED_TX = 1;
            TIMER18ms = 300;
            CMT2300A_TxData(CONST_TXPACKET_DATA_20000AF0,28);
            APP_TX_freq++;
        }
        else if(APP_TX_freq < 3 && Flag_TxDone && Time_APP_blank_TX == 0)
        {
            Flag_TxDone = 0;
            CMT2300A_TxData(CONST_TXPACKET_DATA_20000AF0,28);
            APP_TX_freq++;
        }
        else if(APP_TX_freq >= 3 && Flag_TxDone && Time_APP_blank_TX == 0)
        {
            APP_TX_freq = 0;
            Flag_TxDone = 0;
            FLAG_APP_TX = 0;
            Receiver_LED_TX = 0;
            CMT2310A_SetRx();
        }
    }
    if(Time_Tx_Out == 0 && FLAG_APP_TX == 1)
    {
        APP_TX_freq = 0;
        Flag_TxDone = 0;
        FLAG_APP_TX = 0;
        Flag_TxEn = 0;
        Receiver_LED_TX = 0;
        Time_APP_blank_TX = 0;
        CMT2310A_SetRx();
    }
}

//u8 regbuf[8] = {0};
void CMT2310A_Freq_Select(u32 freq)
{
    switch(freq)
    {
        case 426075000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0x4C;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0xCC;
            g_cmt2310a_page1[52] = 0xD8;
            break;

        case 426100000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0x66;
            g_cmt2310a_page1[18] = 0x66;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0x66;
            g_cmt2310a_page1[51] = 0xE6;
            g_cmt2310a_page1[52] = 0xD8;
            break;

        case 426200000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0xCC;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0x4C;
            g_cmt2310a_page1[52] = 0xD9;
            break;

        case 426750000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0x00;
            g_cmt2310a_page1[18] = 0x00;
            g_cmt2310a_page1[19] = 0x0B;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0x00;
            g_cmt2310a_page1[51] = 0x80;
            g_cmt2310a_page1[52] = 0xDB;
            break;

        case 429175000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x33;
            g_cmt2310a_page1[18] = 0xB3;
            g_cmt2310a_page1[19] = 0x04;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x33;
            g_cmt2310a_page1[51] = 0x33;
            g_cmt2310a_page1[52] = 0xD5;
            break;

        case 429200000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0xCC;
            g_cmt2310a_page1[19] = 0x04;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0x4C;
            g_cmt2310a_page1[52] = 0xD5;
            break;

        case 429350000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x66;
            g_cmt2310a_page1[18] = 0x66;
            g_cmt2310a_page1[19] = 0x05;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x66;
            g_cmt2310a_page1[51] = 0xE6;
            g_cmt2310a_page1[52] = 0xD5;
            break;

        case 429550000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x33;
            g_cmt2310a_page1[18] = 0x33;
            g_cmt2310a_page1[19] = 0x06;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x33;
            g_cmt2310a_page1[51] = 0xB3;
            g_cmt2310a_page1[52] = 0xD6;
            break;
    }
    vRadioRegPageSel(1);
    bRadioSetReg(16,g_cmt2310a_page1[16],0xFF);
    bRadioSetReg(17,g_cmt2310a_page1[17],0xFF);
    bRadioSetReg(18,g_cmt2310a_page1[18],0xFF);
    bRadioSetReg(19,g_cmt2310a_page1[19],0xFF);
    bRadioSetReg(49,g_cmt2310a_page1[49],0xFF);
    bRadioSetReg(50,g_cmt2310a_page1[50],0xFF);
    bRadioSetReg(51,g_cmt2310a_page1[51],0xFF);
    bRadioSetReg(52,g_cmt2310a_page1[52],0xFF);
    vRadioRegPageSel(0);
}

void CMT2310A_DataRate_Select(DataRate_ENUM rate)
{
    switch(rate)
    {
        case RATE_1_2K:
            g_cmt2310a_page1[21] = 0x75;
            g_cmt2310a_page1[22] = 0x02;
            g_cmt2310a_page1[26] = 0x06;
            g_cmt2310a_page1[60] = 0x75;
            g_cmt2310a_page1[61] = 0x02;
            g_cmt2310a_page1[64] = 0x05;
            g_cmt2310a_page1[69] = 0x2B;
            g_cmt2310a_page1[70] = 0x68;
            g_cmt2310a_page1[73] = 0x70;
            g_cmt2310a_page1[75] = 0xE0;
            g_cmt2310a_page1[78] = 0x17;
            g_cmt2310a_page1[81] = 0x04;
            g_cmt2310a_page1[82] = 0x39;
            g_cmt2310a_page1[84] = 0x48;
            g_cmt2310a_page1[85] = 0x04;
            break;

        case RATE_2_4K:
            g_cmt2310a_page1[21] = 0xEA;
            g_cmt2310a_page1[22] = 0x04;
            g_cmt2310a_page1[26] = 0x05;
            g_cmt2310a_page1[60] = 0xEA;
            g_cmt2310a_page1[61] = 0x04;
            g_cmt2310a_page1[64] = 0x01;
            g_cmt2310a_page1[69] = 0x15;
            g_cmt2310a_page1[70] = 0x34;
            g_cmt2310a_page1[73] = 0x70;
            g_cmt2310a_page1[75] = 0xD0;
            g_cmt2310a_page1[78] = 0x1B;
            g_cmt2310a_page1[81] = 0x08;
            g_cmt2310a_page1[82] = 0x09;
            g_cmt2310a_page1[84] = 0x48;
            g_cmt2310a_page1[85] = 0x08;
            break;

        case RATE_4_8K:
            g_cmt2310a_page1[21] = 0xD5;
            g_cmt2310a_page1[22] = 0x09;
            g_cmt2310a_page1[26] = 0x05;
            g_cmt2310a_page1[60] = 0xD5;
            g_cmt2310a_page1[61] = 0x09;
            g_cmt2310a_page1[64] = 0x01;
            g_cmt2310a_page1[69] = 0x0B;
            g_cmt2310a_page1[70] = 0x1A;
            g_cmt2310a_page1[73] = 0x71;
            g_cmt2310a_page1[75] = 0xD0;
            g_cmt2310a_page1[78] = 0x1B;
            g_cmt2310a_page1[81] = 0x0C;
            g_cmt2310a_page1[82] = 0x09;
            g_cmt2310a_page1[84] = 0x24;
            g_cmt2310a_page1[85] = 0x08;
            break;

        case RATE_9_6K:
            g_cmt2310a_page1[21] = 0xA9;
            g_cmt2310a_page1[22] = 0x13;
            g_cmt2310a_page1[26] = 0x04;
            g_cmt2310a_page1[60] = 0xA9;
            g_cmt2310a_page1[61] = 0x13;
            g_cmt2310a_page1[64] = 0x01;
            g_cmt2310a_page1[69] = 0x05;
            g_cmt2310a_page1[70] = 0x0D;
            g_cmt2310a_page1[73] = 0x72;
            g_cmt2310a_page1[75] = 0xD0;
            g_cmt2310a_page1[78] = 0x1B;
            g_cmt2310a_page1[81] = 0x0C;
            g_cmt2310a_page1[82] = 0x09;
            g_cmt2310a_page1[84] = 0x12;
            g_cmt2310a_page1[85] = 0x08;
            break;
    }
    vRadioRegPageSel(1);
    bRadioSetReg(21,g_cmt2310a_page1[21],0xFF);
    bRadioSetReg(22,g_cmt2310a_page1[22],0xFF);
    bRadioSetReg(26,g_cmt2310a_page1[26],0xFF);
    bRadioSetReg(60,g_cmt2310a_page1[60],0xFF);
    bRadioSetReg(61,g_cmt2310a_page1[61],0xFF);
    bRadioSetReg(64,g_cmt2310a_page1[64],0xFF);
    bRadioSetReg(69,g_cmt2310a_page1[69],0xFF);
    bRadioSetReg(70,g_cmt2310a_page1[70],0xFF);
    bRadioSetReg(73,g_cmt2310a_page1[73],0xFF);
    bRadioSetReg(75,g_cmt2310a_page1[75],0xFF);
    bRadioSetReg(78,g_cmt2310a_page1[78],0xFF);
    bRadioSetReg(81,g_cmt2310a_page1[81],0xFF);
    bRadioSetReg(82,g_cmt2310a_page1[82],0xFF);
    bRadioSetReg(84,g_cmt2310a_page1[84],0xFF);
    bRadioSetReg(85,g_cmt2310a_page1[85],0xFF);
    vRadioRegPageSel(0);
}

void CMT2310A_Set_Freq(u32 freq,DEVSET_ENUM dev) //dev -> The value depends on RFPDK how much the export is set to
{
    switch(freq)
    {
        case 426075000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0x4C;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0xCC;
            g_cmt2310a_page1[52] = 0xD8;
            g_cmt2310a_page1[73] = 0xB3; //426MHz, Dev 2.4K and Dev 2.0K alike.
        break;

        case 426100000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0x66;
            g_cmt2310a_page1[18] = 0x66;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0x66;
            g_cmt2310a_page1[51] = 0xE6;
            g_cmt2310a_page1[52] = 0xD8;
            g_cmt2310a_page1[73] = 0xB3;
        break;

        case 426200000:
            g_cmt2310a_page1[16] = 0x6A;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0xCC;
            g_cmt2310a_page1[19] = 0x08;
            g_cmt2310a_page1[49] = 0x6A;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0x4C;
            g_cmt2310a_page1[52] = 0xD9;
            g_cmt2310a_page1[73] = 0xB3;
        break;

        case 429175000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x33;
            g_cmt2310a_page1[18] = 0xB3;
            g_cmt2310a_page1[19] = 0x04;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x33;
            g_cmt2310a_page1[51] = 0x33;
            g_cmt2310a_page1[52] = 0xD5;
        break;

        case 429200000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0xCC;
            g_cmt2310a_page1[18] = 0xCC;
            g_cmt2310a_page1[19] = 0x04;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0xCC;
            g_cmt2310a_page1[51] = 0x4C;
            g_cmt2310a_page1[52] = 0xD5;
        break;

        case 429350000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x66;
            g_cmt2310a_page1[18] = 0x66;
            g_cmt2310a_page1[19] = 0x05;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x66;
            g_cmt2310a_page1[51] = 0xE6;
            g_cmt2310a_page1[52] = 0xD5;
        break;

        case 429550000:
            g_cmt2310a_page1[16] = 0x6B;
            g_cmt2310a_page1[17] = 0x33;
            g_cmt2310a_page1[18] = 0x33;
            g_cmt2310a_page1[19] = 0x06;
            g_cmt2310a_page1[49] = 0x6B;
            g_cmt2310a_page1[50] = 0x33;
            g_cmt2310a_page1[51] = 0xB3;
            g_cmt2310a_page1[52] = 0xD6;
        break;

        default:
            break;
    }
    if(freq > 429000000)
    {
        if(dev == DEV_2_0K)
            g_cmt2310a_page1[73] = 0xB3; //429MHz, Dev 2.4K -> 0x33,Dev 2.0K -> 0xB3.
        else if(dev == DEV_2_4K)
            g_cmt2310a_page1[73] = 0x33;
    }
    vRadioRegPageSel(1);
    bRadioSetReg(16,g_cmt2310a_page1[16],0xFF);
    bRadioSetReg(17,g_cmt2310a_page1[17],0xFF);
    bRadioSetReg(18,g_cmt2310a_page1[18],0xFF);
    bRadioSetReg(19,g_cmt2310a_page1[19],0xFF);
    bRadioSetReg(49,g_cmt2310a_page1[49],0xFF);
    bRadioSetReg(50,g_cmt2310a_page1[50],0xFF);
    bRadioSetReg(51,g_cmt2310a_page1[51],0xFF);
    bRadioSetReg(52,g_cmt2310a_page1[52],0xFF);
    bRadioSetReg(73,g_cmt2310a_page1[73],0xFF);
    vRadioRegPageSel(0);
}

void CMT2310A_Data_Mode(u8 mode)
{
    switch(mode)
    {
        case 0: //Direct 直通模式
            g_cmt2310a_page0[0] = 0x10;
            g_cmt2310a_page1[23] = 0x60;
        break;

        case 1: //Packet 包模式
            g_cmt2310a_page0[0] = 0x12;
            g_cmt2310a_page1[23] = 0xE0;
        break;
    }
}

void TX_DataLoad(u32 IDCache, u8 CtrCmd, u8 *Packet)
{
    u8 i;
    u16 CRCTemp = 0;
    CRCTemp = (IDCache & 0xffff) + (((IDCache >> 16) & 0xff) + ((u16)CtrCmd << 8));
    for (i = 0; i < 24; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((IDCache & ((u32)1 << i)) ? 2 : 1);
    }
    for (i = 24; i < 32; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd & ((u8)1 << (i - 24))) ? 2 : 1);
    }
    for (i = 32; i < 48; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CRCTemp & ((u16)1 << (i - 32))) ? 2 : 1);
    }
    ClearWDT();
}

void TX_DataLoad_HighSpeed(u32 IDCache, Wireless_Body CtrCmd, u8 *Packet)
{
    u8 i;
    u16 CRCTemp = 0;

    CRCTemp = (IDCache & 0xffff) + (((IDCache >> 16) & 0xff) + ((u16)CtrCmd.Fno_Type.byte << 8));
	for(i=0;i<4;i++)
		CRCTemp+=CtrCmd.data[i].ui;

    for (i = 0; i < 24; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((IDCache & ((u32)1 << i)) ? 2 : 1);
    }
    for (i = 24; i < 32; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd.Fno_Type.byte & ((u8)1 << (i - 24))) ? 2 : 1);
    }
	ClearWDT();
    for (i = 32; i < 48; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd.data[0].ui & ((u16)1 << (i - 32))) ? 2 : 1);
    }
    for (i = 48; i < 64; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd.data[1].ui & ((u16)1 << (i - 48))) ? 2 : 1);
    }
	ClearWDT();
    for (i = 64; i < 80; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd.data[2].ui & ((u16)1 << (i - 64))) ? 2 : 1);
    }
    for (i = 80; i < 96; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CtrCmd.data[3].ui & ((u16)1 << (i - 80))) ? 2 : 1);
    }
    ClearWDT();
    for (i = 96; i < 112; i++)
    {
        *(Packet + (i / 4)) <<= 2;
        *(Packet + (i / 4)) |= ((CRCTemp & ((u16)1 << (i - 96))) ? 2 : 1);
    }
}
