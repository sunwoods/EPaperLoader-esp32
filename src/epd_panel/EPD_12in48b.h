/*****************************************************************************
* | File      	:	EPD_12in48b.c
* | Author      :   Waveshare team
* | Function    :   Electronic paper driver
* | Info	 :
*----------------
* |	This version:   V1.0
* | Date	 :   2018-11-29
* | Info	 :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files(the "Software"), to deal
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
#ifndef _EPD_12IN48B_H
#define _EPD_12IN48B_H

// use 14 pins, specially defined here
#define EPD_S1_BUSY_PIN  3
#define EPD_MOSI_PIN 46
#define EPD_S1_CS_PIN 9
#define EPD_M1S1_DC_PIN  10
#define EPD_M1S1_RST_PIN 11
#define EPD_M1_BUSY_PIN  12
#define EPD_M1_CS_PIN 13

#define EPD_S2_BUSY_PIN  14
#define EPD_SCK_PIN  21
#define EPD_M2_CS_PIN 47
#define EPD_M2S2_DC_PIN  48
#define EPD_M2S2_RST_PIN 45
#define EPD_M2_BUSY_PIN  7
#define EPD_S2_CS_PIN 6

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)

static void EPD_12in48b_Reset(void);
static void EPD_M1_SendCommand(UBYTE Reg);
static void EPD_M1_SendData(UBYTE Data);
static void EPD_S1_SendCommand(UBYTE Reg);
static void EPD_S1_SendData(UBYTE Data);
static void EPD_M2_SendCommand(UBYTE Reg);
static void EPD_M2_SendData(UBYTE Data);
static void EPD_S2_SendCommand(UBYTE Reg);
static void EPD_S2_SendData(UBYTE Data);
static void EPD_M1M2_SendCommand(UBYTE Reg);
static void EPD_M1M2_SendData(UBYTE Data);
static void EPD_M1S1M2S2_SendCommand(UBYTE Reg);
static void EPD_M1S1M2S2_SendData(UBYTE Data);
static void EPD_M1_ReadBusy(void);
static void EPD_M2_ReadBusy(void);
static void EPD_S1_ReadBusy(void);
static void EPD_S2_ReadBusy(void);
static void EPD_M1_ReadTemperature(void);
static void EPD_12in48b_SetLut(void);
void EPD_12in48B_TurnOnDisplay(void);
void EPD_12in48B_Sleep(void);

void EPD_12in48b_initSPI(void)
{
    Serial.println("EPD_12in48b_initSPI");
    // return;
    // SPI
    pinMode(EPD_SCK_PIN, OUTPUT);
    pinMode(EPD_MOSI_PIN, OUTPUT);

    // GPIO
    pinMode(EPD_M1_CS_PIN,  OUTPUT);
    pinMode(EPD_S1_CS_PIN,  OUTPUT);
    pinMode(EPD_M2_CS_PIN,  OUTPUT);
    pinMode(EPD_S2_CS_PIN,  OUTPUT);
    pinMode(EPD_M1S1_DC_PIN,  OUTPUT);
    pinMode(EPD_M2S2_DC_PIN,  OUTPUT);
    pinMode(EPD_M1S1_RST_PIN,  OUTPUT);
    pinMode(EPD_M2S2_RST_PIN,  OUTPUT);

    pinMode(EPD_M1_BUSY_PIN,  INPUT);
    pinMode(EPD_S1_BUSY_PIN,  INPUT);
    pinMode(EPD_M2_BUSY_PIN,  INPUT);
    pinMode(EPD_S2_BUSY_PIN,  INPUT);

    // Init
    // CS： High代表非选中，Low代表选中
    DEV_Digital_Write(EPD_M1_CS_PIN,  HIGH);
    DEV_Digital_Write(EPD_S1_CS_PIN,  HIGH);
    DEV_Digital_Write(EPD_M2_CS_PIN,  HIGH);
    DEV_Digital_Write(EPD_S2_CS_PIN,  HIGH);
    DEV_Digital_Write(EPD_SCK_PIN, LOW);
	
	DEV_Digital_Write(EPD_SCK_PIN, 0);
    DEV_Digital_Write(EPD_MOSI_PIN, 0);
    DEV_Digital_Write(EPD_M1_CS_PIN,  0);
    DEV_Digital_Write(EPD_S1_CS_PIN,  0);
    DEV_Digital_Write(EPD_M2_CS_PIN,  0);
    DEV_Digital_Write(EPD_S2_CS_PIN,  0);
    DEV_Digital_Write(EPD_M1S1_DC_PIN,  0);
    DEV_Digital_Write(EPD_M2S2_DC_PIN,  0);
    DEV_Digital_Write(EPD_M1S1_RST_PIN,  0);
    DEV_Digital_Write(EPD_M2S2_RST_PIN,  0);
    DEV_Digital_Write(EPD_M1_BUSY_PIN,  0);
    DEV_Digital_Write(EPD_S1_BUSY_PIN,  0);
    DEV_Digital_Write(EPD_M2_BUSY_PIN,  0);
    DEV_Digital_Write(EPD_S2_BUSY_PIN,  0);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
int EPD_12in48B_Init(void)
{
	Serial.println("EPD_12in48B_Init");

    UBYTE Version = 2; // The latest version is V2
	
    EPD_12in48b_Reset();

    // deselected 4 channels
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);
	
	if(Version == 1) {
		//panel setting
		EPD_M1_SendCommand(0x00);
		EPD_M1_SendData(0x2f);	//KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
		EPD_S1_SendCommand(0x00);
		EPD_S1_SendData(0x2f);
		EPD_M2_SendCommand(0x00);
		EPD_M2_SendData(0x23);
		EPD_S2_SendCommand(0x00);
		EPD_S2_SendData(0x23);

		// POWER SETTING
		EPD_M1_SendCommand(0x01);
		EPD_M1_SendData(0x07);
		EPD_M1_SendData(0x17);	//VGH=20V,VGL=-20V
		EPD_M1_SendData(0x3F);	//VDH=15V
		EPD_M1_SendData(0x3F);  //VDL=-15V
		EPD_M1_SendData(0x0d);
		EPD_M2_SendCommand(0x01);
		EPD_M2_SendData(0x07);
		EPD_M2_SendData(0x17);	//VGH=20V,VGL=-20V
		EPD_M2_SendData(0x3F);	//VDH=15V
		EPD_M2_SendData(0x3F);  //VDL=-15V
		EPD_M2_SendData(0x0d);

		// booster soft start
		EPD_M1_SendCommand(0x06);
		EPD_M1_SendData(0x17);	//A
		EPD_M1_SendData(0x17);	//B
		EPD_M1_SendData(0x39);	//C
		EPD_M1_SendData(0x17);
		EPD_M2_SendCommand(0x06);
		EPD_M2_SendData(0x17);
		EPD_M2_SendData(0x17);
		EPD_M2_SendData(0x39);
		EPD_M2_SendData(0x17);

		//resolution setting
		EPD_M1_SendCommand(0x61);
		EPD_M1_SendData(0x02);
		EPD_M1_SendData(0x88);	//source 648
		EPD_M1_SendData(0x01);	//gate 492
		EPD_M1_SendData(0xEC);
		EPD_S1_SendCommand(0x61);
		EPD_S1_SendData(0x02);
		EPD_S1_SendData(0x90);	//source 656
		EPD_S1_SendData(0x01);	//gate 492
		EPD_S1_SendData(0xEC);
		EPD_M2_SendCommand(0x61);
		EPD_M2_SendData(0x02);
		EPD_M2_SendData(0x90);	//source 656
		EPD_M2_SendData(0x01);	//gate 492
		EPD_M2_SendData(0xEC);
		EPD_S2_SendCommand(0x61);
		EPD_S2_SendData(0x02);
		EPD_S2_SendData(0x88);	//source 648
		EPD_S2_SendData(0x01);	//gate 492
		EPD_S2_SendData(0xEC);

		EPD_M1S1M2S2_SendCommand(0x15);	//DUSPI
		EPD_M1S1M2S2_SendData(0x20);

		EPD_M1S1M2S2_SendCommand(0x30);	// PLL
		EPD_M1S1M2S2_SendData(0x08);

		EPD_M1S1M2S2_SendCommand(0x50);	//Vcom and data interval setting
		EPD_M1S1M2S2_SendData(0x31);
		EPD_M1S1M2S2_SendData(0x07);

		EPD_M1S1M2S2_SendCommand(0x60);//TCON
		EPD_M1S1M2S2_SendData(0x22);

		EPD_M1_SendCommand(0xE0);			//POWER SETTING
		EPD_M1_SendData(0x01);
		EPD_M2_SendCommand(0xE0);			//POWER SETTING
		EPD_M2_SendData(0x01);

		EPD_M1S1M2S2_SendCommand(0xE3);
		EPD_M1S1M2S2_SendData(0x00);

		EPD_M1_SendCommand(0x82);
		EPD_M1_SendData(0x1c);
		EPD_M2_SendCommand(0x82);
		EPD_M2_SendData(0x1c);

		EPD_12in48b_SetLut();
		// EPD_M1_ReadTemperature();
		return 0;
	}
	else if(Version == 2) {	
		// panel setting for Display
        // bit3: UD:Gate Scan Direction    0: Scan down; 1: Scan up(default) G0->G1 -> Gn-1
        // bit5: REG  0: LUT from OTP(default)
        // bit4: KW/R 0: Pixel with B/W/R(default)
		EPD_M1_SendCommand(0x00);
		EPD_M1_SendData(0x0f);	//KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
		EPD_S1_SendCommand(0x00);
		EPD_S1_SendData(0x0f);
        // bit2: SHL: Source shift direction  0: Shift left; 1: Shift right(default)
		EPD_M2_SendCommand(0x00);
		EPD_M2_SendData(0x03);
		EPD_S2_SendCommand(0x00);
		EPD_S2_SendData(0x03);

		// booster soft start
		EPD_M1_SendCommand(0x06);
        // Phase A/B
        // bit[7:6] Soft start period of phase A.  00b: 10ms(default)
        // bit[5:3] Driving strength of phase A.  010b: strength 3(default)
        // bit[2:0] Minimum OFF time setting of GDR in phase A.  111b: 6.58uS(default)
		EPD_M1_SendData(0x17);	//A
		EPD_M1_SendData(0x17);	//B
        // Phase C1
        // bit[5:3] Driving strength of phase C.  000b: strength 3(default) 111b: strength 8(strongest)
		EPD_M1_SendData(0x39);	//C
        // Phase C2
        // bit[7] Booster phase-C2 enable   0:disable 1:enable
		EPD_M1_SendData(0x17);


		EPD_M2_SendCommand(0x06);
		EPD_M2_SendData(0x17);
		EPD_M2_SendData(0x17);
		EPD_M2_SendData(0x39);
		EPD_M2_SendData(0x17);

		//resolution setting
        // total resolution： 1304 * 984  (0x518 * 0x3D8)
        // devided into 4 parts： (648*492 + 656*492 + 656*492 + 648*492) 
		EPD_M1_SendCommand(0x61);
		EPD_M1_SendData(0x02);
		EPD_M1_SendData(0x88);	//source 648
		EPD_M1_SendData(0x01);	//gate 492
		EPD_M1_SendData(0xEC);
		EPD_S1_SendCommand(0x61);
		EPD_S1_SendData(0x02);
		EPD_S1_SendData(0x90);	//source 656
		EPD_S1_SendData(0x01);	//gate 492
		EPD_S1_SendData(0xEC);
		EPD_M2_SendCommand(0x61);
		EPD_M2_SendData(0x02);
		EPD_M2_SendData(0x90);	//source 656
		EPD_M2_SendData(0x01);	//gate 492
		EPD_M2_SendData(0xEC);
		EPD_S2_SendCommand(0x61);
		EPD_S2_SendData(0x02);
		EPD_S2_SendData(0x88);	//source 648
		EPD_S2_SendData(0x01);	//gate 492
		EPD_S2_SendData(0xEC);

        // Dual SPI Mode
        // bit[5]: MM_EN  MM input pin definition enable（stack）.  0: disable; 1: enable
        // bit[4]: DUSPI_EN  Dual SPI mode enable.  0: disable; 1: enable
		EPD_M1S1M2S2_SendCommand(0x15);	//DUSPI
		EPD_M1S1M2S2_SendData(0x20);

		EPD_M1S1M2S2_SendCommand(0x50);	//Vcom and data interval setting
		EPD_M1S1M2S2_SendData(0x11);
		EPD_M1S1M2S2_SendData(0x07);

		EPD_M1S1M2S2_SendCommand(0x60);//TCON
		EPD_M1S1M2S2_SendData(0x22);

		EPD_M1S1M2S2_SendCommand(0xE3);
		EPD_M1S1M2S2_SendData(0x00);

        // read M1 temperature
		EPD_M1_ReadTemperature();

		return 0;
	}
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_12in48B_Clear(void)
{
    UWORD y, x;

    // M1 part 648*492
    EPD_M1_SendCommand(0x10);
    for(y =  492; y < 984; y++) {
        for(x = 0; x < 81; x++) {
            EPD_M1_SendData(0xff);
        }
    }
    EPD_M1_SendCommand(0x13);
    for(y = 492; y < 984; y++) {
        for(x = 0; x < 81; x++) {
            EPD_M1_SendData(0x00);
        }
    }

    // S1 part 656*492
    EPD_S1_SendCommand(0x10);
    for(y = 492; y < 984; y++) {
        for(x = 81; x < 163; x++) {
            EPD_S1_SendData(0xff);
        }
    }
    EPD_S1_SendCommand(0x13);
    for(y = 492; y < 984; y++) {
        for(x = 81; x < 163; x++) {
            EPD_S1_SendData(0x00);
        }
    }

    // M2 part 656*492
    EPD_M2_SendCommand(0x10);
    for(y = 0; y < 492; y++) {
        for(x = 81; x < 163; x++) {
            EPD_M2_SendData(0xff);
        }
    }
    EPD_M2_SendCommand(0x13);
    for(y = 0; y < 492; y++) {
        for(x = 81; x < 163; x++) {
            EPD_M2_SendData(0x00);
        }
    }

    // S2 part 648*492
    EPD_S2_SendCommand(0x10);
    for(y = 0; y < 492; y++) {
        for(x = 0; x < 81; x++) {
            EPD_S2_SendData(0xff);
        }
    }
    EPD_S2_SendCommand(0x13);
    for(y = 0; y < 492; y++) {
        for(x = 0; x < 81; x++) {
            EPD_S2_SendData(0x00);
        }
    }          
	
    // Turn On Display
    EPD_12in48B_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_12in48B_cmd1S2(void)
{
    EPD_S2_SendCommand(0x10);
}
void EPD_12in48B_cmd1M2(void)
{
    EPD_M2_SendCommand(0x10);
}
void EPD_12in48B_cmd1M1(void)
{
    EPD_M1_SendCommand(0x10);
}
void EPD_12in48B_cmd1S1(void)
{
    EPD_S1_SendCommand(0x10);
}
void EPD_12in48B_data1S2(UBYTE data)
{
    EPD_S2_SendData(data);
}
void EPD_12in48B_data1M2(UBYTE data)
{
    EPD_M2_SendData(data);
}
void EPD_12in48B_data1M1(UBYTE data)
{
    EPD_M1_SendData(data);
}
void EPD_12in48B_data1S1(UBYTE data)
{
    EPD_S1_SendData(data);
}

void EPD_12in48B_cmd2S2(void)
{
    EPD_S2_SendCommand(0x13);
}
void EPD_12in48B_cmd2M2(void)
{
    EPD_M2_SendCommand(0x13);
}
void EPD_12in48B_cmd2M1(void)
{
    EPD_M1_SendCommand(0x13);
}
void EPD_12in48B_cmd2S1(void)
{
    EPD_S1_SendCommand(0x13);
}
void EPD_12in48B_data2S2(UBYTE data)
{
    EPD_S2_SendData(~data);
}
void EPD_12in48B_data2M2(UBYTE data)
{
    EPD_M2_SendData(~data);
}
void EPD_12in48B_data2M1(UBYTE data)
{
    EPD_M1_SendData(~data);
}
void EPD_12in48B_data2S1(UBYTE data)
{
    EPD_S1_SendData(~data);
}
/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_12in48B_TurnOnDisplay(void)
{
    EPD_M1M2_SendCommand(0x04); //power on
    delay(300);
    EPD_M1S1M2S2_SendCommand(0x12); //Display Refresh

    EPD_M1_ReadBusy();
    EPD_S1_ReadBusy();
    EPD_M2_ReadBusy();
    EPD_S2_ReadBusy();
}

void EPD_12in48B_Show(void) {
    Serial.println("EPD_12in48B_Show");

    EPD_12in48B_TurnOnDisplay();
    EPD_12in48B_Sleep();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_12in48B_Sleep(void)
{
    EPD_M1S1M2S2_SendCommand(0X02);  	//power off
    delay(300);

    EPD_M1S1M2S2_SendCommand(0X07);  	//deep sleep
    EPD_M1S1M2S2_SendData(0xA5);
    delay(300);
   
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_12in48b_Reset(void)
{
    DEV_Digital_Write(EPD_M1S1_RST_PIN, 1);
    DEV_Digital_Write(EPD_M2S2_RST_PIN, 1);
    delay(200);
    DEV_Digital_Write(EPD_M1S1_RST_PIN, 0);
    DEV_Digital_Write(EPD_M2S2_RST_PIN, 0);
    delay(10);
    DEV_Digital_Write(EPD_M1S1_RST_PIN, 1);
    DEV_Digital_Write(EPD_M2S2_RST_PIN, 1);
    delay(200);
}

/******************************************************************************
function:	Microsecond delay
parameter:
Info:
******************************************************************************/
void DEV_Delay_us(UWORD xus)
{
    UWORD i;
    while(xus) {
        for(i = 0; i < 30; i++);
        xus--;
    }
}


/******************************************************************************
function:
			SPI read and write
******************************************************************************/
static void DEV_SPI_WriteByte(UBYTE value)
{
    char i;
    DEV_Digital_Write(EPD_SCK_PIN, 0);
    for(i = 0; i < 8; i++) {
        DEV_Digital_Write(EPD_SCK_PIN, 0);
        DEV_Delay_us(5);
        if((value << i)  & 0x80) {
            DEV_Digital_Write(EPD_MOSI_PIN, 1);
        } else {
            DEV_Digital_Write(EPD_MOSI_PIN, 0);
        }
        DEV_Delay_us(5);
        DEV_Digital_Write(EPD_SCK_PIN, 1);
        DEV_Delay_us(5);
    }
}

static UBYTE DEV_SPI_ReadByte(char x)
{
    UBYTE i,temp=0;
    
    return temp;
}

/******************************************************************************
function :	send command and data(M1\M2\S1\S2\M1S1\M1S1M2S2)
parameter:
    Reg : Command register
or:
    Data : Write data
******************************************************************************/
static void EPD_M1_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 0);
    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
}
static void EPD_M1_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 1);
    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
}

static void EPD_S1_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 0);
    DEV_Digital_Write(EPD_S1_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
}
static void EPD_S1_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 1);
    DEV_Digital_Write(EPD_S1_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
}

static void EPD_M2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 0);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
}
static void EPD_M2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
}

static void EPD_S2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 0);
    DEV_Digital_Write(EPD_S2_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);
}
static void EPD_S2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 1);
    DEV_Digital_Write(EPD_S2_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);
}

static void EPD_M1M2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 0);
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 0);
    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
}
static void EPD_M1M2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 1);
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 1);
    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
}

static void EPD_M1S1M2S2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 0);	// command write
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 0);  // command write

    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_Digital_Write(EPD_S1_CS_PIN, 0);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_Digital_Write(EPD_S2_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);
}
static void EPD_M1S1M2S2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 1);	// command write
    DEV_Digital_Write(EPD_M2S2_DC_PIN, 1);  // command write

    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_Digital_Write(EPD_S1_CS_PIN, 0);
    DEV_Digital_Write(EPD_M2_CS_PIN, 0);
    DEV_Digital_Write(EPD_S2_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW(M1\M2\S1\S2)
parameter:
******************************************************************************/
static void EPD_M1_ReadBusy(void)
{
    UBYTE busy;
    do {
        EPD_M1_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_M1_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(0);
    Serial.print("M1 Busy free\r\n");
    delay(200);
    
}
static void EPD_M2_ReadBusy(void)
{
    UBYTE busy;
    do {
        EPD_M2_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_M2_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    Serial.print("M2 Busy free\r\n");
    delay(200);
}
static void EPD_S1_ReadBusy(void)
{
    UBYTE busy;
    do {
        EPD_S1_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_S1_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    Serial.print("S1 Busy free\r\n");
    delay(200);
}
static void EPD_S2_ReadBusy(void)
{
    UBYTE busy;
    do {
        EPD_S2_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_S2_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    Serial.print("S2 Busy free\r\n");
    delay(200);
}

/******************************************************************************
function :	ReadTemperature
parameter:
******************************************************************************/
static void EPD_M1_ReadTemperature(void)
{
    EPD_M1_SendCommand(0x40);
    EPD_M1_ReadBusy();
    delay(300);

    // just select M1
    DEV_Digital_Write(EPD_M1_CS_PIN, 0);
    DEV_Digital_Write(EPD_S1_CS_PIN, 1);
    DEV_Digital_Write(EPD_M2_CS_PIN, 1);
    DEV_Digital_Write(EPD_S2_CS_PIN, 1);

    // read data
    DEV_Digital_Write(EPD_M1S1_DC_PIN, 1);
    DEV_Delay_us(5);

    UBYTE temp;
    temp = DEV_SPI_ReadByte(0x00);
    // restore deselected M1
    DEV_Digital_Write(EPD_M1_CS_PIN, 1);
    printf("Read Temperature Reg:%d\r\n", temp);

    // Sync master temperature data -> slave
    // bit[1] TSFIX:  Let the value of slave’s temperature is same as the master’s.
    //         1: Temperature value is defined by TS_SET[7:0] registers.
    // bit[0] CCEN: Output clock enable/disable.
    //         1: Output clock at CL pin to slave chip.
    EPD_M1S1M2S2_SendCommand(0xe0);//Cascade setting
    EPD_M1S1M2S2_SendData(0x03);
    EPD_M1S1M2S2_SendCommand(0xe5);//Force temperature
    EPD_M1S1M2S2_SendData(temp);

}

static unsigned char lut_12in48b_vcom1[] = {
    0x00,	0x10,	0x10,	0x01,	0x08,	0x01,
    0x00,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x00,	0x08,	0x01,	0x08,	0x01,	0x06,
    0x00,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x06,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x01,
    0x00,	0x04,	0x05,	0x08,	0x08,	0x01,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};
static unsigned char lut_12in48b_ww1[] = {
    0x91,	0x10,	0x10,	0x01,	0x08,	0x01,
    0x04,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x84,	0x08,	0x01,	0x08,	0x01,	0x06,
    0x80,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x06,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x01,
    0x08,	0x04,	0x05,	0x08,	0x08,	0x01,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};
static unsigned char lut_12in48b_bw1[] = {
    0xA8,	0x10,	0x10,	0x01,	0x08,	0x01,
    0x84,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x84,	0x08,	0x01,	0x08,	0x01,	0x06,
    0x86,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x8C,	0x05,	0x01,	0x1E,	0x0F,	0x06,
    0x8C,	0x05,	0x01,	0x1E,	0x0F,	0x01,
    0xF0,	0x04,	0x05,	0x08,	0x08,	0x01,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};
static unsigned char lut_12in48b_wb1[] = {
    0x91,	0x10,	0x10,	0x01,	0x08,	0x01,
    0x04,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x84,	0x08,	0x01,	0x08,	0x01,	0x06,
    0x80,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x06,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x01,
    0x08,	0x04,	0x05,	0x08,	0x08,	0x01,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};
static unsigned char lut_12in48b_bb1[] = {
    0x92,	0x10,	0x10,	0x01,	0x08,	0x01,
    0x80,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x84,	0x08,	0x01,	0x08,	0x01,	0x06,
    0x04,	0x06,	0x01,	0x06,	0x01,	0x05,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x06,
    0x00,	0x05,	0x01,	0x1E,	0x0F,	0x01,
    0x01,	0x04,	0x05,	0x08,	0x08,	0x01,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};

/******************************************************************************
function :	ReadTemperature
parameter:
******************************************************************************/
static void EPD_12in48b_SetLut(void)
{
    UWORD count;

    EPD_M1S1M2S2_SendCommand(0x20);							//vcom
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_vcom1[count]);
    }

    EPD_M1S1M2S2_SendCommand(0x21);							//red not use
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_ww1[count]);
    }

    EPD_M1S1M2S2_SendCommand(0x22);							//bw r
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_bw1[count]);   // bw=r
    }

    EPD_M1S1M2S2_SendCommand(0x23);							//wb w
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_wb1[count]);   // wb=w
    }

    EPD_M1S1M2S2_SendCommand(0x24);							//bb b
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_bb1[count]);   // bb=b
    }

    EPD_M1S1M2S2_SendCommand(0x25);							//bb b
    for(count=0; count<60; count++) {
        EPD_M1S1M2S2_SendData(lut_12in48b_ww1[count]);   // bb=b
    }
}

#endif