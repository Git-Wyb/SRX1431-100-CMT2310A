
/****************************************************************************
;---------------------------------------
;  CMT2310A Configuration File
;  Generated by CMOSTEK RFPDK_V1.59
;  2024.10.31 13:52
;---------------------------------------
; Mode                       = Advanced
; Part Number                = CMT2310A
; Frequency                  = 426.075 MHz
; DC-DC                      = Off
; Demodulation               = GFSK
; Xtal Cap Load              = 0
; Data Rate                  = 1.200 kbps
; Deviation                  = 2.200 kHz
; Tx Xtal Tol.               = 2 ppm
; Rx Xtal Tol.               = 2 ppm
; TRx Matching Network Type  = 20 dBm
; Tx Power                   = 0 dBm
; Gaussian BT                = 0.5
; PA Ramp                    = On-Autosel Rampstep
; PA Ramp Step               = NA
; RF Performance             = NA
; Output Drive Capability    = 4mA
; Rx Duty-Cycle              = Off
; Tx Duty-Cycle              = Off
; Sleep Timer                = Off
; Sleep Time                 = NA
; Rx Timer                   = Off
; Rx Time T1                 = NA
; Rx Time T2                 = NA
; Rx Exit State              = RFS
; Tx Exit State              = TFS
; TX Duty Cycle Persist      = Off
; Packet Done Exit           = On
; TX Duty-Cycle Times        = 0
; SLP Mode                   = Mode 0
; RSSI Valid Source          = PJD & RSSI Compare
; PJD Window                 = 4 Jumps
; RSSI Compare TH            = -127 dBm
; CDR Type                   = Tracing
; AFC                        = enable
; FSK2 Data Map              = 0:F-low 1:F-high
; FSK4 Data Map              = NA
; FSK4 RX Data Map           = NA
; CDR Type                   = Tracing
; CDR Range Sel FSK          = +/-9.4%
; Channel BW                 = autosel
; Baseband BW FSK            = 15
; Data Mode                  = Packet
; Packet Mode                = Normal
; PiggyBacking               = Disable
; Manchester                 = Disable
; Manchester Type            = NA
; Whitening                  = Disable
; Whiten Type                = NA
; Whiten Seed Type           = NA
; Whiten Seed                = NA
; FEC                        = Disable
; FEC Type                   = NA
; FEC Padding Code           = NA
; crc err clear fifo         = Clear
; Tx Packet Gap              = 16 symbol(s)
; Tx Packet Number           = 1 packet(s)
; Tx Prefix Type             = 0
; Packet Type                = Fixed Length
; Address-Length Position    = NA
; Length Size                = 2-byte
; Payload Bit Order          = Start from msb
; Address Field              = Disable
; Preamble Rx Size           = 16
; Preamble Tx Size           = 16
; Preamble Value             = 85
; Preamble Unit              = 8-bit
; Sync Size                  = 3-byte
; Sync Format                = 802.15.4
; Sync Value                 = 5592341
; Sync Manchester            = Disable
; Sync Value Selection       = Sync Value
; Sync FEC Value             = 3003605
; Sync Tolerance             = None
; Address Detect Mode        = None
; Address Split Mode         = NA
; Address Size               = NA
; Address Err Mask           = NA
; Address Free               = NA
; Dest Addr Value            = NA
; Src Addr Value             = NA
; Dest Addr Bit Mask         = NA
; Src Addr Bit Mask          = NA
; Sequence Num               = None
; Sequence Num Match         = off
; Sequence Num Mode          = NA
; Sequence Num Value         = NA
; FCS2                       = None
; FCS2 Value                 = NA
; Payload Length             = 28
; CRC Options                = None
; CRC Swap                   = NA
; CRC Seed                   = NA
; CRC Bit Invert             = NA
; CRC Range                  = NA
; CRC Polynomial             = NA
; CRC Bit Order              = NA
; CRC Refin                  = NA
; CRC_Refout                 = NA
; Frequency Hopping Mode     = Mode 0
; Freq Hopping Space         = 1 kHz
; Hopping Channels           = 0
; CSMA Mode                  = Disable
; CSMA RSSI Detection        = NA
; Hopping Persist            = Disable
; Hopping Intermediate State = TRFS
; CSMA Sleep Timer Random    = NA
; CSMA Rx Time               = NA
; CSMA Sleep Time M          = NA
; CSMA Sleep Time R          = NA
; CSMA Persist               = NA
; CSMA Detect Times          = NA
; Tx Auto Hopping            = Disable
; Rx Auto Hopping            = Disable
; Auto Acknowledge           = off
; Auto Resend                = off
; Maximum Resend Times       = 255
; RSSI Detect Mode           = preamble_ok
; LFOSC LFXO Sel             = LFOSC(32kHz)
; LF Clock Out               = on
; Dout Mute                  = disable
; Dout Mute Sel              = NA
; dout adjust mode           = increase
; Dout Adjust Percentage     = 0
; LBD Threshold              = 2.0 v
; Antenna Diversity          = off
; Antenna Switch Mode        = NA
; Collision Detect           = off
; Collision Step             = NA
; RSSI Offset dB             = NA
; RSSI Offset Sel            = autosel
; i_fir_bb_bw_for_cal_freq   = 12

;---------------------------------------
;  The following are the Register contents
;---------------------------------------

*****************************************************************************/

#ifndef __CMT2310A_PARAMS_H
#define __CMT2310A_PARAMS_H

#define CMT2310A_PAGE0_SIZE         (0x77-0x28+1)
#define CMT2310A_PAGE1_SIZE         (0xef-0x80+1)
#define CMT2310A_PAGE2_SIZE         (0x3f-0x00+1)

/* [CMT page0] */
const unsigned char g_cmt2310a_page0[CMT2310A_PAGE0_SIZE] = {
    0x82,
    0x10,
    0x00,
    0x55,
    0x84,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x15,
    0x55,
    0x55,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xD5,
    0xD4,
    0x2D,
    0x1B,
    0x00,
    0x20,
    0x00,
    0x00,
    0x00,
    0x00,
    0x2D,
    0x00,
    0x00,
    0x00,
    0x2D,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x45,
    0x1F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x88,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xE4,
    0x30,
    0x50,
    0x80,
    0x01,
    0x00,
    0xD0,
    0xE0,
    0xE2,
    0x84,
    0x31,
    0x08,
    0xD0,
    0xE0,
    0x6B,
    0x00,
    0x41,
    0x00,
    0xFF,
    0x00,
    0x02,
    0x00,
    0x00,
    0x03,
    0x04,
};

/* [CMT page1] */
unsigned char g_cmt2310a_page1[CMT2310A_PAGE1_SIZE] = {
    0x10,
    0x06,
    0x00,
    0xFF,
    0x00,
    0x0D,
    0x00,
    0x28,
    0x50,
    0x87,
    0x31,
    0x5B,
    0x08,
    0x00,
    0xFF,
    0x00,
    0x6A,
    0xCC,
    0x4C,
    0x08,
    0xB4,
    0x75,
    0x02,
    0xE0,
    0x20,
    0x08,
    0x06,
    0x01,
    0x03,
    0x00,
    0x2A,
    0x1F,
    0x00,
    0x32,
    0x00,
    0x00,
    0x01,
    0xE4,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xCA,
    0x6A,
    0xCC,
    0xCC,
    0xD8,
    0x6B,
    0x00,
    0x80,
    0x0F,
    0x64,
    0x06,
    0x02,
    0x75,
    0x02,
    0x00,
    0x36,
    0x05,
    0x20,
    0xC8,
    0x63,
    0xA4,
    0x2B,
    0x68,
    0x58,
    0x40,
    0x70,
    0x74,
    0xE0,
    0x0F,
    0x01,
    0x17,
    0xE6,
    0x54,
    0x04,
    0x39,
    0xC6,
    0x48,
    0x04,
    0x01,
    0xB4,
    0x01,
    0x0F,
    0x00,
    0x4C,
    0x00,
    0x00,
    0xF6,
    0x00,
    0x00,
    0x00,
    0x14,
    0x81,
    0x01,
    0x00,
    0x47,
    0x12,
    0x25,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

//#define CMT2310A_AUTO_HOP_ENABLE
#ifdef  CMT2310A_AUTO_HOP_ENABLE
#define freq_space_val           0x01
#define freq_times_val           0x00
#define freq_switch_state_val    0x01
#define freq_hop_persist_val     0x00

/* [CMT page2] */
const unsigned char g_cmt2310a_page2[CMT2310A_PAGE2_SIZE] = {
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
};
#endif

#endif
