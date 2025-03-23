/*****************************************************************************
* | File      	:   EPD_3IN52.h
* | Author      :   Waveshare team
* | Function    :   3.52inch e-paper
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2022-11-02
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

//GC 0.9S
static const UBYTE EPD_3IN52_lut_R20_GC[] =
{
  0x01,0x0f,0x0f,0x0f,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00
};							  
static const UBYTE EPD_3IN52_lut_R21_GC[] =
{
  0x01,0x4f,0x8f,0x0f,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00
};						 
static const UBYTE EPD_3IN52_lut_R22_GC[] =
{
  0x01,0x0f,0x8f,0x0f,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const UBYTE EPD_3IN52_lut_R23_GC[] =
{
  0x01,0x4f,0x8f,0x4f,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const UBYTE EPD_3IN52_lut_R24_GC[] =
{
  0x01,0x0f,0x8f,0x4f,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

UBYTE EPD_3IN52_Flag;

/******************************************************************************
function :	Read Busy
parameter:
******************************************************************************/
void EPD_3IN52_ReadBusy(void)
{
    Serial.print("e-Paper busy\r\n");
    UBYTE busy;
    do {
        busy = digitalRead(PIN_SPI_BUSY);
    } while(!busy);
    delay(200);
    Serial.print("e-Paper busy release\r\n");
}

/**
 * @brief 
 * 
 */
void EPD_3IN52_refresh(void)
{
    EPD_SendCommand(0x17);
    EPD_SendData(0xA5);
    EPD_3IN52_ReadBusy();
    delay(200);
}

// LUT download
void EPD_3IN52_lut_GC(void)
{
    UBYTE count;
    EPD_SendCommand(0x20);        // vcom
    for(count = 0; count < 56 ; count++)
    {
        EPD_SendData(EPD_3IN52_lut_R20_GC[count]);
    }
        
    EPD_SendCommand(0x21);        // red not use
    for(count = 0; count < 42 ; count++)
    {
        EPD_SendData(EPD_3IN52_lut_R21_GC[count]);
    }
        
    EPD_SendCommand(0x24);        // bb b
    for(count = 0; count < 42 ; count++)
    {
        EPD_SendData(EPD_3IN52_lut_R24_GC[count]);
    }
    
    if(EPD_3IN52_Flag == 0)
    {
        EPD_SendCommand(0x22);    // bw r
        for(count = 0; count < 56 ; count++)
        {
            EPD_SendData(EPD_3IN52_lut_R22_GC[count]);
        }
            
        EPD_SendCommand(0x23);    // wb w
        for(count = 0; count < 42 ; count++)
        {
            EPD_SendData(EPD_3IN52_lut_R23_GC[count]);
        }
            
        EPD_3IN52_Flag = 1;
    }
        
    else
    {
        EPD_SendCommand(0x22);    // bw r
        for(count = 0; count < 56 ; count++)
        {
            EPD_SendData(EPD_3IN52_lut_R23_GC[count]);
        }
            
        EPD_SendCommand(0x23);    // wb w
        for(count = 0; count < 42 ; count++)
        {
            EPD_SendData(EPD_3IN52_lut_R22_GC[count]);
        }
            
       EPD_3IN52_Flag = 0;
    }
}


void EPD_3IN52_Clear(void)
{
    EPD_SendCommand(0x13);
    for(int i=0; i<10800; i++)
        EPD_SendData(0xFF);
    EPD_3IN52_lut_GC();
    EPD_3IN52_refresh();

    EPD_SendCommand(0x50);
    EPD_SendData(0x17);

    delay(500);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
int EPD_3IN52_Init(void)
{
    EPD_3IN52_Flag = 0;
    EPD_Reset();

    EPD_SendCommand(0x00);		// panel setting   PSR
    EPD_SendData(0xFF);			// RES1 RES0 REG KW/R     UD    SHL   SHD_N  RST_N	
    EPD_SendData(0x01);			// x x x VCMZ TS_AUTO TIGE NORG VC_LUTZ

    EPD_SendCommand(0x01);		// POWER SETTING   PWR
    EPD_SendData(0x03);			//  x x x x x x VDS_EN VDG_EN	
    EPD_SendData(0x10);			//  x x x VCOM_SLWE VGH[3:0]   VGH=20V, VGL=-20V	
    EPD_SendData(0x3F);			//  x x VSH[5:0]	VSH = 15V
    EPD_SendData(0x3F);			//  x x VSL[5:0]	VSL=-15V
    EPD_SendData(0x03);			//  OPTEN VDHR[6:0]  VHDR=6.4V
                                        // T_VDS_OFF[1:0] 00=1 frame; 01=2 frame; 10=3 frame; 11=4 frame
    EPD_SendCommand(0x06);		// booster soft start   BTST 
    EPD_SendData(0x37);			//  BT_PHA[7:0]  	
    EPD_SendData(0x3D);			//  BT_PHB[7:0]	
    EPD_SendData(0x3D);			//  x x BT_PHC[5:0]	

    EPD_SendCommand(0x60);		// TCON setting			TCON 
    EPD_SendData(0x22);			// S2G[3:0] G2S[3:0]   non-overlap = 12		

    EPD_SendCommand(0x82);		// VCOM_DC setting		VDCS 
    EPD_SendData(0x07);			// x  VDCS[6:0]	VCOM_DC value= -1.9v    00~3f,0x12=-1.9v

    EPD_SendCommand(0x30);			 
    EPD_SendData(0x09);		

    EPD_SendCommand(0xe3);		// power saving			PWS 
    EPD_SendData(0x88);			// VCOM_W[3:0] SD_W[3:0]

    EPD_SendCommand(0x61);		// resoultion setting 
    EPD_SendData(0xf0);			//  HRES[7:3] 0 0 0	
    EPD_SendData(0x01);			//  x x x x x x x VRES[8]	
    EPD_SendData(0x68);			//  VRES[7:0]

    EPD_SendCommand(0x50);			
    EPD_SendData(0xB7);	

    EPD_3IN52_Clear();

    EPD_SendCommand(0x13);//DATA_START_TRANSMISSION_1  
    delay(2);
    return 0;
}

void EPD_3IN52_Show(void)
{
    EPD_3IN52_lut_GC();
    EPD_3IN52_refresh();
    delay(2);
    Serial.print("EPD_3IN52_Show END\r\n");
    EPD_SendCommand(0X07);  	//deep sleep
    EPD_SendData(0xA5);
}
