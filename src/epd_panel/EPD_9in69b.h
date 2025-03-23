/*****************************************************************************
* | File      	:	EPD_9IN69B.h
* | Author      :   Waveshare team
* | Function    :   Electronic paper driver
* | Info        :
*----------------
* |	This version:   V2.1
* | Date        :   2020-12-01
* | Info        :
* 1.Remove:ImageBuff[EPD_HEIGHT * EPD_WIDTH / 8]
* 2.Change:EPD_Display(UBYTE *Image)
*   Need to pass parameters: pointer to cached data
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
#ifndef _EPD_9IN69B_H_
#define _EPD_9IN69B_H_

// use 7 pins, specially defined here
#define EPD_MOSI_PIN 18
#define EPD_SCK_PIN  17
#define EPD_CS_PIN   16
#define EPD_DC_PIN   15
#define EPD_RST_PIN  14
#define EPD_BUSY_PIN 13
#define EPD_CS_S_PIN 12

// Display resolution
#define EPD_9IN69B_WIDTH       960
#define EPD_9IN69B_HEIGHT      672

#define EPD_9IN69B_IMAGE_DATA_SIZE (EPD_9IN69B_WIDTH * EPD_9IN69B_HEIGHT / 8 / 2)

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_9IN69B_Reset(void)
{
    // 200, 20, 200, 200, 5
    delay(200);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    delay(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    delay(200);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    delay(200);
    DEV_Digital_Write(EPD_CS_PIN, 1);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
    delay(5);
}

void GPIO_Mode(UWORD GPIO_Pin, UWORD Mode)
{
    if(Mode == 0) {
        pinMode(GPIO_Pin , INPUT);
	} else {
		pinMode(GPIO_Pin , OUTPUT);
	}
}

/******************************************************************************
function:
			SPI read and write
******************************************************************************/
static void EPD_9IN69B_SPI_WriteByte(UBYTE data)
{
    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(EPD_MOSI_PIN, GPIO_PIN_RESET); 
        else                    digitalWrite(EPD_MOSI_PIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }
}

static UBYTE EPD_9IN69B_SPI_ReadByte()
{
    UBYTE j=0xff;
    GPIO_Mode(EPD_MOSI_PIN, 0);

    for (int i = 0; i < 8; i++)
    {
        j = j << 1;
        if (digitalRead(EPD_MOSI_PIN))  j = j | 0x01;
        else                            j = j & 0xfe;
        
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }

    GPIO_Mode(EPD_MOSI_PIN, 1);
    return j;
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_9IN69B_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_CS_S_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
}

void EPD_9IN69B_SendCommandM(UBYTE Reg)
{
    DEV_Digital_Write(EPD_CS_S_PIN, 1);

    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void EPD_9IN69B_SendCommandS(UBYTE Reg)
{
    DEV_Digital_Write(EPD_CS_PIN, 1);

    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_S_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_9IN69B_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_CS_S_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
}

void EPD_9IN69B_SendDataM(UBYTE Data)
{
    DEV_Digital_Write(EPD_CS_S_PIN, 1);

    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void EPD_9IN69B_SendDataS(UBYTE Data)
{
    DEV_Digital_Write(EPD_CS_PIN, 1);

    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_S_PIN, 0);
    EPD_9IN69B_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
static void EPD_9IN69B_WaitUntilIdle(void)
{
    Serial.print("e-Paper busy\r\n");
    unsigned char busy;
	do	{
		busy = DEV_Digital_Read(EPD_BUSY_PIN);
		busy =!(busy & 0x01); 
        delay(100);       
	}while(busy);
	Serial.print("e-Paper busy release\r\n");
}

static void EPD_9IN69B_DCDC_SoftStart() {
    //DCDC soft-start
    uint8_t  Index51_data[]={0x50,0x01,0x0a,0x01};
    uint8_t  Index09_data[]={0x1f,0x9f,0x7f,0xff};

    EPD_9IN69B_SendCommand(0x51);
    EPD_9IN69B_SendData(Index51_data[0]);
    EPD_9IN69B_SendData(Index51_data[1]);

    for(int value = 1; value <= 4; value++) {
        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[0]);

        Index51_data[1]=value;
        EPD_9IN69B_SendCommand(0x51);
        EPD_9IN69B_SendData(Index51_data[0]);
        EPD_9IN69B_SendData(Index51_data[1]);

        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[1]);
        delay(2);
    }

    for(int value = 1; value <= 10; value++) {
        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[0]);

        Index51_data[3]=value;
        EPD_9IN69B_SendCommand(0x51);
        EPD_9IN69B_SendData(Index51_data[2]);
        EPD_9IN69B_SendData(Index51_data[3]);

        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[1]);
        delay(2);
    }

    for(int value = 3; value <= 10; value++) {
        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[2]);

        Index51_data[3]=value;
        EPD_9IN69B_SendCommand(0x51);
        EPD_9IN69B_SendData(Index51_data[2]);
        EPD_9IN69B_SendData(Index51_data[3]);

        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[3]);
        delay(2);
    }

    for(int value = 9; value >= 2; value--) {
        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[1]);

        Index51_data[2]=value;
        EPD_9IN69B_SendCommand(0x51);
        EPD_9IN69B_SendData(Index51_data[2]);
        EPD_9IN69B_SendData(Index51_data[3]);

        EPD_9IN69B_SendCommand(0x09);
        EPD_9IN69B_SendData(Index09_data[3]);
        delay(2);
    }

    EPD_9IN69B_SendCommand(0x09);
    EPD_9IN69B_SendData(Index09_data[3]);
    delay(10);
}

static void EPD_9IN69B_DCDC_SoftShutdown() {
    EPD_9IN69B_WaitUntilIdle();

    EPD_9IN69B_SendCommand(0x09);
    EPD_9IN69B_SendData(0x7f);
    EPD_9IN69B_SendCommand(0x05);
    EPD_9IN69B_SendData(0x7d);
    EPD_9IN69B_SendCommand(0x09);
    EPD_9IN69B_SendData(0x00);
    delay(200);

    EPD_9IN69B_WaitUntilIdle();

    digitalWrite(EPD_DC_PIN, LOW);
    digitalWrite(EPD_CS_PIN, LOW);
    digitalWrite(EPD_RST_PIN, LOW);
    digitalWrite(EPD_CS_PIN, HIGH);
}


/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_9IN69B_TurnOnDisplay(void)
{
    EPD_9IN69B_DCDC_SoftStart();

    EPD_9IN69B_SendCommand(0x12);			//DISPLAY REFRESH

    EPD_9IN69B_WaitUntilIdle();
    delay(100);

    EPD_9IN69B_SendCommand(0x15);
	EPD_9IN69B_SendData(0x3c);
    delay(5);
}


void EPD_9IN69B_Prepare() {
    // DUW for both
    EPD_9IN69B_SendCommand(0x13);
	EPD_9IN69B_SendData(0x00);
	EPD_9IN69B_SendData(0x3b);
    EPD_9IN69B_SendData(0x00);
    EPD_9IN69B_SendData(0x00);
    EPD_9IN69B_SendData(0x9f);
    EPD_9IN69B_SendData(0x02);

    // DRFW for both
    EPD_9IN69B_SendCommand(0x90);
	EPD_9IN69B_SendData(0x00);
	EPD_9IN69B_SendData(0x3b);
    EPD_9IN69B_SendData(0x00);
    EPD_9IN69B_SendData(0xa9);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
static int EPD_9IN69B_Init(void)
{
    Serial.println("EPD_9IN69B_Init is calling");
    EPD_9IN69B_Reset();

    EPD_9IN69B_SendCommand(0x05);
    EPD_9IN69B_SendData(0x7d);
    delay(200);

    EPD_9IN69B_SendCommand(0x05);
    EPD_9IN69B_SendData(0x00);
    delay(10);

    // MS_SYNC
    EPD_9IN69B_SendCommand(0xd8);
    EPD_9IN69B_SendData(0x80);
    // BVSS
    EPD_9IN69B_SendCommand(0xd6);
    EPD_9IN69B_SendData(0x00);

    EPD_9IN69B_SendCommand(0xa7);
    EPD_9IN69B_SendData(0x10);
    delay(100);
    EPD_9IN69B_SendCommand(0xa7);
	EPD_9IN69B_SendData(0x00);
    delay(100);

    // OSC
    EPD_9IN69B_SendCommand(0x03);
	EPD_9IN69B_SendData(0x00);
    EPD_9IN69B_SendData(0x11);

    // Master
    EPD_9IN69B_SendCommandM(0x44);
    EPD_9IN69B_SendDataM(0x00);
    EPD_9IN69B_SendCommandM(0x45);
    EPD_9IN69B_SendDataM(0x80);
    EPD_9IN69B_SendCommandM(0xa7);
    EPD_9IN69B_SendDataM(0x10);
    delay(100);
    EPD_9IN69B_SendCommandM(0xa7);
    EPD_9IN69B_SendDataM(0x00);
    EPD_9IN69B_SendCommandM(0x44);
    EPD_9IN69B_SendDataM(0x06);
    // temperature
    EPD_9IN69B_SendCommandM(0x45);
    EPD_9IN69B_SendDataM(0x82);

    EPD_9IN69B_SendCommandM(0xa7);
    EPD_9IN69B_SendDataM(0x10);
    delay(100);
    EPD_9IN69B_SendCommandM(0xa7);
    EPD_9IN69B_SendDataM(0x00);
    delay(100);

    // Slave
    EPD_9IN69B_SendCommandS(0x44);
    EPD_9IN69B_SendDataS(0x00);
    EPD_9IN69B_SendCommandS(0x45);
    EPD_9IN69B_SendDataS(0x80);
    EPD_9IN69B_SendCommandS(0xa7);
    EPD_9IN69B_SendDataS(0x10);
    delay(100);
    EPD_9IN69B_SendCommandS(0xa7);
    EPD_9IN69B_SendDataS(0x00);
    delay(100);
    EPD_9IN69B_SendCommandS(0x44);
    EPD_9IN69B_SendDataS(0x06);
    // temperature
    EPD_9IN69B_SendCommandS(0x45);
    EPD_9IN69B_SendDataS(0x82);

    EPD_9IN69B_SendCommandS(0xa7);
    EPD_9IN69B_SendDataS(0x10);
    delay(100);
    EPD_9IN69B_SendCommandS(0xa7);
    EPD_9IN69B_SendDataS(0x00);
    delay(100);

    // both, TCON
    EPD_9IN69B_SendCommand(0x60);
    EPD_9IN69B_SendData(0x25);
    // Master, STV_DIR
    EPD_9IN69B_SendCommandM(0x61);
    EPD_9IN69B_SendDataM(0x01);
    // both, VCOM
    EPD_9IN69B_SendCommand(0x02);
    EPD_9IN69B_SendData(0x00);

    return 0;
}


void EPD_9IN69B_SendImages(uint8_t* data1m, uint8_t* data2m, uint8_t* data1s, uint8_t* data2s)  {
    Serial.println("EPD_9IN69B_SendImages is calling");
    EPD_9IN69B_Prepare();

    // RAM_RW for Master
    EPD_9IN69B_SendCommandM(0x12);
	EPD_9IN69B_SendDataM(0x3b);
	EPD_9IN69B_SendDataM(0x00);
    EPD_9IN69B_SendDataM(0x14);
    //First frame for Master
    EPD_9IN69B_SendCommandM(0x10);
    for (int i = 0; i < EPD_9IN69B_IMAGE_DATA_SIZE; i++) {
        EPD_9IN69B_SendDataM(data1m[i]);
    }

    // RAM_RW for Master
    EPD_9IN69B_SendCommandM(0x12);
    EPD_9IN69B_SendDataM(0x3b);
    EPD_9IN69B_SendDataM(0x00);
    EPD_9IN69B_SendDataM(0x14);
    //Second frame for Master
    EPD_9IN69B_SendCommandM(0x11);
    for (int i = 0; i < EPD_9IN69B_IMAGE_DATA_SIZE; i++) {
        EPD_9IN69B_SendDataM(data2m[i]);
    }

    // RAM_RW for Slave
    EPD_9IN69B_SendCommandS(0x12);
    EPD_9IN69B_SendDataS(0x3b);
    EPD_9IN69B_SendDataS(0x00);
    EPD_9IN69B_SendDataS(0x14);
    //First frame for Slave
    EPD_9IN69B_SendCommandS(0x10);
    for (int i = 0; i < EPD_9IN69B_IMAGE_DATA_SIZE; i++) {
        EPD_9IN69B_SendDataS(data1s[i]);
    }

    // RAM_RW for Slave
    EPD_9IN69B_SendCommandS(0x12);
    EPD_9IN69B_SendDataS(0x3b);
    EPD_9IN69B_SendDataS(0x00);
    EPD_9IN69B_SendDataS(0x14);
    //Second frame for Slave
    EPD_9IN69B_SendCommandS(0x11);
    for (int i = 0; i < EPD_9IN69B_IMAGE_DATA_SIZE; i++) {
        EPD_9IN69B_SendDataS(data2s[i]);
    }
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_9IN69B_Clear(void)
{
    int buffer_size = EPD_9IN69B_IMAGE_DATA_SIZE;
    uint8_t* data_buffer = (uint8_t*)malloc(buffer_size);
    if (NULL == data_buffer) {
        Serial.println("malloc failed.");
        exit(-1);
    }
    memset(data_buffer, 0, buffer_size);

    EPD_9IN69B_SendImages(data_buffer, data_buffer, data_buffer, data_buffer);

    EPD_9IN69B_TurnOnDisplay();
}

void EPD_9IN69B_InitSPI(void) {
    Serial.println("EPD_9IN69B_InitSPI is calling");
    pinMode(EPD_BUSY_PIN,  INPUT);

    pinMode(EPD_RST_PIN , OUTPUT);
    digitalWrite(EPD_RST_PIN , HIGH);

    pinMode(EPD_DC_PIN  , OUTPUT);
    digitalWrite(EPD_DC_PIN , HIGH);
    
    pinMode(EPD_CS_PIN , OUTPUT);
    digitalWrite(EPD_CS_PIN , HIGH);
    pinMode(EPD_CS_S_PIN , OUTPUT);
    digitalWrite(EPD_CS_S_PIN , HIGH);

    pinMode(EPD_SCK_PIN, OUTPUT);
    pinMode(EPD_MOSI_PIN, OUTPUT);

    digitalWrite(EPD_SCK_PIN, LOW);
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
static void EPD_9IN69B_Sleep(void)
{
    EPD_9IN69B_DCDC_SoftShutdown();
}

static void EPD_9IN69B_Show(void) {
    Serial.println("EPD_9IN69B_Show");

    EPD_9IN69B_TurnOnDisplay();
    EPD_9IN69B_Sleep();
}
#endif
