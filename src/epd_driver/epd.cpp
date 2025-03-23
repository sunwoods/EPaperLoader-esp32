#include <ArduinoUZlib.h>

#include "buff/buff.h"
#include "epd_conf.h"
#include "epd.h"
#include "epd_wifi.h"
#include "epd_bt.h"

#include "epd_panel/epd1in54.h"
#include "epd_panel/epd2in13.h"
#include "epd_panel/epd2in9.h"
#include "epd_panel/epd2in7.h"
#include "epd_panel/epd2in66.h"
#include "epd_panel/epd3in7.h"
#include "epd_panel/epd3in52.h"
#include "epd_panel/epd4in01f.h"
#include "epd_panel/epd4in2.h"
#include "epd_panel/epd5in65f.h"
#include "epd_panel/epd5in83.h"
#include "epd_panel/epd7in5.h"
#include "epd_panel/epd7in5_HD.h"
#include "epd_panel/epd7in3f.h"
#include "epd_panel/EPD_12in48b.h"
#include "epd_panel/epd9in7g.h"
#include "epd_panel/EPD_9in69b.h"
#include "epd_panel/epd7in3e.h"

/* Lut mono ------------------------------------------------------------------*/
byte lut_full_mono[] =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

byte lut_partial_mono[] =
{
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

byte lut_vcom0[] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00 };
byte lut_w    [] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04 };
byte lut_b    [] = { 15, 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04 };
byte lut_g1   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_g2   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_vcom1[] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red0 [] = { 15, 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red1 [] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


int loadMode = LOAD_MODE_BT;

int PIN_SPI_DIN = -1;
int PIN_SPI_SCK = -1;
int PIN_SPI_CS = -1;
int PIN_SPI_DC = -1;
int PIN_SPI_RST = -1;
int PIN_SPI_BUSY = -1;

bool is_compress = false;

uint8_t *rcv_image_data = NULL;
uint32_t rcv_image_data_size = 0;
uint8_t *image_data = NULL;
uint32_t image_data_size = 0;
bool load_stage = false;
bool next_stage = false;

void decompress_image_data() {
    uint32_t size_out = 0;
    unsigned long start_time = millis();
    unsigned long end_time = 0;
    uint32_t result = ArduinoUZlib::decompress(rcv_image_data, rcv_image_data_size, image_data, image_data_size);

    end_time = millis();
    Serial.printf("cost time=%d\n", end_time - start_time);
    Serial.printf("\n=========out buffer=========\n");
    Serial.printf("Byte: %lu, result=%lu\n", image_data_size, result);
}

void EPD_SPISetCfg(
    int din = DEFAULT_PIN_SPI_DIN,
    int sck = DEFAULT_PIN_SPI_SCK,
    int cs = DEFAULT_PIN_SPI_CS,
    int dc = DEFAULT_PIN_SPI_DC,
    int rst = DEFAULT_PIN_SPI_RST,
    int busy = DEFAULT_PIN_SPI_BUSY) 
{
    Serial.printf("SPI config: din=%d, sck=%d, cs=%d, dc=%d, rst=%d, busy=%d\n", 
            din, sck, cs, dc, rst, busy);
    PIN_SPI_DIN = din;
    PIN_SPI_SCK = sck;
    PIN_SPI_CS = cs;
    PIN_SPI_DC = dc;
    PIN_SPI_RST = rst;
    PIN_SPI_BUSY = busy;
}

void EPD_initSPI()
{
    pinMode(PIN_SPI_BUSY,  INPUT);
    pinMode(PIN_SPI_RST , OUTPUT);
    pinMode(PIN_SPI_DC  , OUTPUT);
    
    pinMode(PIN_SPI_SCK, OUTPUT);
    pinMode(PIN_SPI_DIN, OUTPUT);
    pinMode(PIN_SPI_CS , OUTPUT);

    digitalWrite(PIN_SPI_CS , HIGH);
    digitalWrite(PIN_SPI_SCK, LOW);
}

void EpdSpiTransferCallback(byte data) 
{
    digitalWrite(PIN_SPI_CS, GPIO_PIN_RESET);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(PIN_SPI_DIN, GPIO_PIN_RESET); 
        else                    digitalWrite(PIN_SPI_DIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(PIN_SPI_SCK, GPIO_PIN_SET);     
        digitalWrite(PIN_SPI_SCK, GPIO_PIN_RESET);
    }

    digitalWrite(PIN_SPI_CS, GPIO_PIN_SET);
}

/* Sending a byte as a command -----------------------------------------------*/
void EPD_SendCommand(byte command) 
{
    digitalWrite(PIN_SPI_DC, LOW);
    EpdSpiTransferCallback(command);
}

/* Sending a byte as a data --------------------------------------------------*/
void EPD_SendData(byte data) 
{
    digitalWrite(PIN_SPI_DC, HIGH);
    EpdSpiTransferCallback(data);
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle() 
{
    //0: busy, 1: idle
    while(digitalRead(PIN_SPI_BUSY) == 0) delay(100);    
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle_high() 
{
    //1: busy, 0: idle
    while(digitalRead(PIN_SPI_BUSY) == 1) delay(100);   
}

/* Send a one-argument command -----------------------------------------------*/
void EPD_Send_1(byte c, byte v1)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
}

/* Send a two-arguments command ----------------------------------------------*/
void EPD_Send_2(byte c, byte v1, byte v2)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
}

/* Send a three-arguments command --------------------------------------------*/
void EPD_Send_3(byte c, byte v1, byte v2, byte v3)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
}

/* Send a four-arguments command ---------------------------------------------*/
void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
    EPD_SendData(v4);
}

/* Send a five-arguments command ---------------------------------------------*/
void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
    EPD_SendData(v4);
    EPD_SendData(v5);
}

/* Writting lut-data into the e-Paper ----------------------------------------*/
void EPD_lut(byte c, byte l, byte*p)
{
    // lut-data writting initialization
    EPD_SendCommand(c);

    // lut-data writting doing
    for (int i = 0; i < l; i++, p++) EPD_SendData(*p);
}

/* Writting lut-data of the black-white channel ------------------------------*/
void EPD_SetLutBw(byte*c20, byte*c21, byte*c22, byte*c23, byte*c24) 
{
    EPD_lut(0x20, *c20, c20 + 1);//g vcom 
    EPD_lut(0x21, *c21, c21 + 1);//g ww -- 
    EPD_lut(0x22, *c22, c22 + 1);//g bw r
    EPD_lut(0x23, *c23, c23 + 1);//g wb w
    EPD_lut(0x24, *c24, c24 + 1);//g bb b
}

/* Writting lut-data of the red channel --------------------------------------*/
void EPD_SetLutRed(byte*c25, byte*c26, byte*c27) 
{
    EPD_lut(0x25, *c25, c25 + 1);
    EPD_lut(0x26, *c26, c26 + 1);
    EPD_lut(0x27, *c27, c27 + 1);
}

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
void EPD_Reset() 
{
    digitalWrite(PIN_SPI_RST, HIGH); 
    delay(200);    
    digitalWrite(PIN_SPI_RST, LOW);    
    delay(5);
    digitalWrite(PIN_SPI_RST, HIGH); 
    delay(200);
}


/* Image data loading function for a-type e-Paper ----------------------------*/ 
void EPD_loadA()
{
    if (loadMode == LOAD_MODE_BT) {
        myEPDBt.EPD_loadA();
        return;
    }
    
    int pos = 0;

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte
        int value = Buff__getByte(pos);
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's memory
        EPD_SendData((byte)value);

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

void EPD_loadAFilp()
{
    if (loadMode == LOAD_MODE_BT) {
        myEPDBt.EPD_loadAFilp();
        return;
    }

    int pos = 0;

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte
        int value = Buff__getByte(pos);
        
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's memory
        EPD_SendData(~(byte)value);

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

/* Image data loading function for b-type e-Paper ----------------------------*/
void EPD_loadB()
{
    if (loadMode == LOAD_MODE_BT) {
        // not done
        return;
    }
    
    // Come back to the image data end
    Buff__bufInd -= 8;

    // Get the index of the image data begin
    int pos = Buff__bufInd - Buff__getWord(Buff__bufInd);

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current word from obtained image data
        int valueA = (int)Buff__getByte(pos) + ((int)Buff__getByte(pos + 2) << 8);

        // Clean current word of processed image data 
        int valueB = 0;

        // Enumerate next 8 pixels
        for (int p = 0; p < 8; p++)
        {
            // Current obtained pixel data
            int pixInd = valueA & 3;

            // Remove the obtained pixel data from 'valueA' word
            valueA = valueA >> 2;

            // Processing of 8 2-bit pixels to 8 2-bit pixels: 
            // black(value 0) to bits 00, white(value 1) to bits 11, gray(otherwise) to bits 10
            valueB = (valueB << 2) + (pixInd == 1 ? 3 : (pixInd == 0 ? 0 : 2));
        }

        // Write the word into e-Paper's memory
        EPD_SendData((byte)(valueB >> 8));
        EPD_SendData((byte)valueB);

        // Increment the current byte index on 2 characters
        pos += 4;
    }
}

/* Image data loading function for 2.13 e-Paper ------------------------------*/
void EPD_loadC()
{
    if (loadMode == LOAD_MODE_BT) {
        // not done
        return;
    }

    // Come back to the image data end
    Buff__bufInd -= 8;

    // Get the index of the image data begin
    int pos = Buff__bufInd - Buff__getWord(Buff__bufInd);

    EPD_Send_2(0x44, 0, 15);        //SET_RAM_X_ADDRESS_START_END_POSITION LO(x >> 3), LO((w - 1) >> 3)
    EPD_Send_4(0x45, 0, 0, 249, 0); //SET_RAM_Y_ADDRESS_START_END_POSITION LO(y), HI(y), LO(h - 1), HI(h - 1)

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Before write a line of image data
        // 2.13 e-Paper requires to set the address counter
        // Every line has 15*8-6 pixels + 6 empty bits, totally 15*8 bits
        if (EPD_dispX == 0)
        {
            EPD_Send_1(0x4E, 0           );//SET_RAM_X_ADDRESS_COUNTER: LO(x >> 3)
            EPD_Send_2(0x4F, EPD_dispY, 0);//SET_RAM_Y_ADDRESS_COUNTER: LO(y), HI(y)  
            EPD_SendCommand(0x24);//WRITE_RAM
        }

        // Write the byte into e-Paper's memory
        EPD_SendData((byte)Buff__getByte(pos));  

        // Increment the current byte index on 2 characters     
        pos += 2;

        // EPD_dispX and EPD_dispY increments
        if (++EPD_dispX > 15)
        {
            EPD_dispX = 0;

            // If the client's browser sends more bits, than it needs, then exit the function
            if (++EPD_dispY > 250) return;
        }
    }
}

/* Image data loading function for 7.5 e-Paper -------------------------------*/
void EPD_loadD()
{
    if (loadMode == LOAD_MODE_BT) {
        // not done
        return;
    }

    // Come back to the image data end
    Buff__bufInd -= 8;

    // Get the index of the image data begin
    int pos = Buff__bufInd - Buff__getWord(Buff__bufInd);

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte from obtained image data
        int valueA = Buff__getByte(pos);

        // Processing of 4 1-bit pixels to 4 4-bit pixels:
        // black(value 0) to bits 0000, white(value 1) to bits 0011
        EPD_SendData((byte)((valueA & 0x80) ? 0x30 : 0x00) + ((valueA & 0x40) ? 0x03 : 0x00));
        EPD_SendData((byte)((valueA & 0x20) ? 0x30 : 0x00) + ((valueA & 0x10) ? 0x03 : 0x00));
        EPD_SendData((byte)((valueA & 0x08) ? 0x30 : 0x00) + ((valueA & 0x04) ? 0x03 : 0x00));
        EPD_SendData((byte)((valueA & 0x02) ? 0x30 : 0x00) + ((valueA & 0x01) ? 0x03 : 0x00));  

        // Increment the current byte index on 2 characters 
        pos += 2;
    }
}

/* Image data loading function for 7.5b e-Paper ------------------------------*/
void EPD_loadE()
{
    if (loadMode == LOAD_MODE_BT) {
        // not done
        return;
    }

    // Come back to the image data end
    Buff__bufInd -= 8;

    // Get the index of the image data begin
    int pos = Buff__bufInd - Buff__getWord(Buff__bufInd);

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte from obtained image data
        int value = Buff__getByte(pos);  

        // Processing of 4 1-bit pixels to 4 4-bit pixels:
        // red(value 1) to bits 0011, white(value 3) to bits 0100
        int A = (value     ) & 3;if (A == 3) A = 4;if (A == 1) A = 3;
        int B = (value >> 2) & 3;if (B == 3) B = 4;if (B == 1) B = 3;
        int C = (value >> 4) & 3;if (C == 3) C = 4;if (C == 1) C = 3;
        int D = (value >> 6) & 3;if (D == 3) D = 4;if (D == 1) D = 3;

        // Write the word into e-Paper's memory
        EPD_SendData((A << 4) + B);
        EPD_SendData((C << 4) + D);

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

/* Image data loading function for 5.83b e-Paper -----------------------------*/
void EPD_loadF()
{
    if (loadMode == LOAD_MODE_BT) {
        // not done
        return;
    }

    // Come back to the image data end
    Buff__bufInd -= 8;

    // Get the index of the image data begin
    int pos = Buff__bufInd - Buff__getWord(Buff__bufInd);

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte from obtained image data
        int value = Buff__getByte(pos);  
        int value1 = Buff__getByte(pos+2);  
        // Processing of 4 1-bit pixels to 4 4-bit pixels:
        // white(value 1) to bits 0011, red(value 2) to bits 0100
        int A = (value     ) & 3;if (A == 2) A = 4;
        int B = (value >> 8) & 3;if (B == 2) B = 4;
        int C = (value1 >> 4) & 3;if (C == 2) C = 4;
        int D = (value1 >> 6) & 3;if (D == 2) D = 4;

        // Write the word into e-Paper's memory
        EPD_SendData((A << 4) + B);
        EPD_SendData((C << 4) + D);

        // Increment the current byte index on 2 characters
        pos += 4;
    }
}
/* Image data loading function for 5.65f e-Paper -----------------------------*/
void EPD_loadG()
{
    if (loadMode == LOAD_MODE_BT) {
        myEPDBt.EPD_loadG();
        return;
    }

    int pos = 0;

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte
        int value = Buff__getByte(pos);
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's memory
        EPD_SendData((byte)value);

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

/* Show image and turn to deep sleep mode (a-type, 4.2 and 2.7 e-Paper) ------*/
void EPD_showA() 
{
    // Refresh
    EPD_Send_1(0x22, 0xC4);// DISPLAY_UPDATE_CONTROL_2
    EPD_SendCommand( 0x20);// MASTER_ACTIVATION
    EPD_SendCommand( 0xFF);// TERMINATE_FRAME_READ_WRITE
    EPD_WaitUntilIdle();

    // Sleep
    EPD_SendCommand(0x10);// DEEP_SLEEP_MODE
    EPD_WaitUntilIdle();
}

/* Show image and turn to deep sleep mode (b-type, e-Paper) ------------------*/
void EPD_showB() 
{
    // Refresh
    EPD_SendCommand(0x12);// DISPLAY_REFRESH
    delay(100);
    EPD_WaitUntilIdle();

    // Sleep
    EPD_Send_1(0x50, 0x17);// VCOM_AND_DATA_INTERVAL_SETTING
    EPD_Send_1(0x82, 0x00);// VCM_DC_SETTING_REGISTER, to solve Vcom drop
    EPD_Send_4(0x01, 0x02, 0x00, 0x00, 0x00);// POWER_SETTING
    EPD_WaitUntilIdle();
    EPD_SendCommand(0x02);// POWER_OFF
}

/* Show image and turn to deep sleep mode (7.5 and 7.5b e-Paper) -------------*/
void EPD_showC() 
{
    // Refresh
    EPD_SendCommand(0x12);// DISPLAY_REFRESH
    delay(100);
    EPD_WaitUntilIdle();

    // Sleep
    EPD_SendCommand(0x02);// POWER_OFF
    EPD_WaitUntilIdle();
    EPD_Send_1(0x07, 0xA5);// DEEP_SLEEP
}

/* Show image and turn to deep sleep mode (2.13 e-Paper) ---------------------*/
void EPD_showD()
{
    // VCOM AND DATA INTERVAL SETTING
    // WBmode:VBDF 17, D7 VBDW 97, VBDB 57
    // WBRmode:VBDF F7, VBDW 77, VBDB 37, VBDR B7
    EPD_Send_1(0x50, 0x97);

    EPD_SendCommand(0x20);
    for(int count=0; count<44; count++) EPD_SendData(lut_vcomDC_2in13d[count]);
    
    EPD_SendCommand(0x21);
    for(int count=0; count<42; count++) EPD_SendData(lut_ww_2in13d[count]);

    EPD_SendCommand(0x22);
    for(int count=0; count<42; count++) EPD_SendData(lut_bw_2in13d[count]);
    
    EPD_SendCommand(0x23);
    for(int count=0; count<42; count++) EPD_SendData(lut_wb_2in13d[count]);
    
    EPD_SendCommand(0x24);
    for(int count=0; count<42; count++) EPD_SendData(lut_bb_2in13d[count]);

    delay(10);
    EPD_SendCommand(0x12);//DISPLAY REFRESH
    delay(100);     //!!!The delay here is necessary, 200uS at least!!!
    EPD_WaitUntilIdle(); 

    EPD_Send_1(0x50, 0xf7);
    EPD_SendCommand( 0x02);//POWER_OFF
    EPD_Send_1(0x07, 0xA5);//DEEP_SLEEP
}

/* Initialization of an e-Paper ----------------------------------------------*/
void EPD_dispInit()
{
    // Call initialization function
    EPD_dispMass[EPD_dispIndex].init();

    // Set loading function for black channel
    EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chBk;

    // Set initial coordinates
    EPD_dispX = 0;
    EPD_dispY = 0;

    // The inversion of image data bits isn't needed by default
    EPD_invert = false;
    
}

// for web
bool Srvr__loop() {
    bool wifiRet = myEPDWifi.Srvr__loop();

    return true;
}

// create queue for bt
bool Srvr__init() {
    bool btRet = myEPDBt.Srvr__init();
    return true;
}

// for bluetooth
void Srvr__rcvProc() {
    myEPDBt.Srvr__rcvProc();
}

// for wifi
void Srvr__postProc() {
    myEPDWifi.Srvr__postProc();
}

void reboot_device() {
    Serial.println("Now reboot device...");
    esp_restart();
}

/* e-Paper initialization functions ------------------------------------------*/ 

bool EPD_invert;           // If true, then image data bits must be inverted
int  EPD_dispIndex;        // The index of the e-Paper's type
int  EPD_dispX, EPD_dispY; // Current pixel's coordinates (for 2.13 only)
void(*EPD_dispLoad)();     // Pointer on a image data writting function
/* Array of sets describing the usage of e-Papers ----------------------------*/
EPD_dispInfo EPD_dispMass[] =
{
    { EPD_Init_1in54 , EPD_loadA, -1  , 0,         EPD_showA, "1.54 inch"   },// a 0
    { EPD_Init_1in54b, EPD_loadB, 0x13, EPD_loadA, EPD_showB, "1.54 inch b" },// b 1
    { EPD_Init_1in54c, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "1.54 inch c" },// c 2
    { EPD_Init_2in13 , EPD_loadC, -1  , 0,         EPD_showA, "2.13 inch"   },// d 3
    { EPD_Init_2in13b, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.13 inch b" },// e 4
    { EPD_Init_2in13b, EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.13 inch c" },// f 5
    { EPD_Init_2in13d, EPD_loadA, -1  , 0,         EPD_showD, "2.13 inch d" },// g 6
    { EPD_Init_2in7  , EPD_loadA, -1  , 0,         EPD_showB, "2.7 inch"    },// h 7
    { EPD_Init_2in7b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.7 inch b"  },// i 8
    { EPD_Init_2in9  , EPD_loadA, -1  , 0,         EPD_showA, "2.9 inch"    },// j 9
    { EPD_Init_2in9b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.9 inch b"  },// k 10
    { EPD_Init_2in9b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "2.9 inch c"  },// l 11
    { EPD_Init_2in9d , EPD_loadA, -1  , 0,         EPD_2IN9D_Show, "2.9 inch d" },// M 12
    { EPD_Init_4in2  , EPD_loadA, -1  , 0,         EPD_showB, "4.2 inch"    },// m 14
    { EPD_Init_4in2b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "4.2 inch b"  },// n 15
    { EPD_Init_4in2b , EPD_loadA, 0x13, EPD_loadA, EPD_showB, "4.2 inch c"  },// o 16
    { EPD_Init_5in83 , EPD_loadD, -1  , 0,         EPD_showC, "5.83 inch"   },// p 17
    { EPD_Init_5in83b, EPD_loadE, -1  , 0,         EPD_showC, "5.83 inch b" },// q 16
    { EPD_Init_5in83b, EPD_loadE, -1  , 0,         EPD_showC, "5.83 inch c" },// r 18
    { EPD_Init_7in5 ,  EPD_loadD, -1  , 0,         EPD_showC, "7.5 inch"    },// s 19
    { EPD_Init_7in5 ,  EPD_loadE, -1  , 0,         EPD_showC, "7.5 inch b"  },// t 20
    { EPD_Init_7in5 ,  EPD_loadE, -1  , 0,         EPD_showC, "7.5 inch c"  }, // u 21
    { EPD_7in5_V2_init , EPD_loadAFilp, -1  , 0,   EPD_7IN5_V2_Show, "7.5 inch V2" },// w 22
    { EPD_7in5B_V2_Init , EPD_loadA, 0x13  , EPD_loadAFilp,      EPD_7IN5_V2_Show, "7.5 inch B V2 "},// x 23    
    { EPD_7IN5B_HD_init , EPD_loadA, 0x26  , EPD_loadAFilp,      EPD_7IN5B_HD_Show, "7.5 inch B HD "},// x 24
	{ EPD_5IN65F_init,		EPD_loadG,		-1  ,	0,				EPD_5IN65F_Show,	"5.65 inch F "	},// z 25
	{ EPD_7IN5_HD_init,		EPD_loadA,		-1,		0,				EPD_7IN5_HD_Show,	"7.5 inch HD"	},// 26
	{ EPD_3IN7_1Gray_Init,	EPD_loadA,		-1	,	0,				EPD_3IN7_1Gray_Show,"3.7 inch"		},// 27
	{ EPD_2IN66_Init,		EPD_loadA,		-1	,	0,				EPD_2IN66_Show,		"2.66 inch"		},// 28
	{ EPD_5in83b_V2_init,	EPD_loadA,		0x13,	EPD_loadAFilp,	EPD_showC,			"5.83 inch B V2"},// 29
	{ EPD_Init_2in9b_V3,	EPD_loadA,		0x13,	EPD_loadA,		EPD_showC,			"2.9 inch B V3"	},// 30
	{ EPD_1IN54B_V2_Init,	EPD_loadA,		0x26,	EPD_loadAFilp,	EPD_1IN54B_V2_Show,	"1.54 inch B V2"},// 31
	{ EPD_2IN13B_V3_Init,	EPD_loadA,		0x13,	EPD_loadA,		EPD_2IN13B_V3_Show,	"2.13 inch B V3"},// 32
	{ EPD_Init_2in9_V2, 	EPD_loadA,		-1,		0,				EPD_2IN9_V2_Show,	"2.9 inch V2"	},// 33
	{ EPD_Init_4in2b_V2,	EPD_loadA,		0x13,	EPD_loadA,		EPD_4IN2B_V2_Show,	"4.2 inch B V2"	},// 34
	{ EPD_2IN66B_Init,		EPD_loadA,		0x26,	EPD_loadAFilp,	EPD_2IN66_Show,		"2.66 inch B"	},// 35
	{ EPD_Init_5in83_V2,	EPD_loadAFilp,	-1,		0,				EPD_showC,			"5.83 inch V2"	},// 36
	{ EPD_4IN01F_init,		EPD_loadG,		-1,		0,				EPD_4IN01F_Show,	"4.01 inch F"	},// 37
	{ EPD_Init_2in7b_V2, 	EPD_loadA,		0x26,	EPD_loadAFilp,	EPD_2in7b_V2_Show,	"2.7 inch B V2"	},// 38
	{ EPD_Init_2in13_V3,	EPD_loadC,		-1, 	0, 				EPD_2IN13_V3_Show, 	"2.13 inch V3"	},// 39
	{ EPD_2IN13B_V4_Init,	EPD_loadA,		0x26,	EPD_loadA,		EPD_2IN13B_V4_Show, "2.13 inch B V4"},// 40
    { EPD_3IN52_Init,	    EPD_loadA,		-1,	    0,		        EPD_3IN52_Show,     "3.52 inch"     },// 41
    { EPD_2IN7_V2_Init,		EPD_loadA, 		-1  ,	0,				EPD_2IN7_V2_Show,	"2.7 inch V2"	},// 42
    { EPD_7IN3F_init,		EPD_loadG, 		-1  ,	0,				EPD_7IN3F_Show,	    "7.3 inch F"	},// 43
    { EPD_12in48B_Init,		NULL, 	        0x13,	NULL,	        EPD_12in48B_Show,   "12.48 inch B V1"	},// 44
    { EPD_9IN7G_init,		EPD_loadA, 		-1  ,	0,	            EPD_9IN7G_Show,	    "9.7 inch G"	},// 45
    { EPD_9IN69B_Init,		EPD_loadA, 	    -1  ,	EPD_loadAFilp,	EPD_9IN69B_Show,	"9.69 inch B"	},// 46
    { EPD_7IN3E_init,		EPD_loadG, 		-1  ,	0,				EPD_7IN3E_Show,	    "7.3 inch E"	},// 47
};
