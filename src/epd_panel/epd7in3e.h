/**
  ******************************************************************************
  * @file    edp7in3e.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    29-Dec-2020
  * @brief   This file describes initialisation of 7.3f e-Papers
  *
  ******************************************************************************
  */

  /**********************************
Color Index
**********************************/
#define EPD_7IN3E_BLACK   0x0   /// 000
#define EPD_7IN3E_WHITE   0x1   /// 001
#define EPD_7IN3E_YELLOW  0x2   /// 010
#define EPD_7IN3E_RED     0x3   /// 011
#define EPD_7IN3E_ORANGE  0x4   /// 100
#define EPD_7IN3E_BLUE    0x5   /// 101
#define EPD_7IN3E_GREEN   0x6   /// 110

#include <esp_task_wdt.h>
static void EPD_7IN3E_BusyHigh(void)// If BUSYN=0 then waiting
{
    while(!(digitalRead(PIN_SPI_BUSY)));
}

static void EPD_7IN3E_BusyLow(void)// If BUSYN=1 then waiting
{
    while(digitalRead(PIN_SPI_BUSY));
}


static void EPD_7IN3E_Show(void)
{
    Serial.println("EPD_7IN3E_Show Begin");

    Serial.println("Turn on display.");
    // turn on display
    EPD_SendCommand(0x04);//0x04
    EPD_7IN3E_BusyHigh();

    //Second setting 
    EPD_SendCommand(0x06);
    EPD_SendData(0x6F);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x22);

    EPD_SendCommand(0x12);//0x12
    EPD_SendData(0x00);
    EPD_7IN3E_BusyHigh();

    // sleep
    EPD_SendCommand(0x02);//0x02
    EPD_SendData(0x00);
    EPD_7IN3E_BusyHigh();
    
	delay(200);
    Serial.print("EPD_7IN3E_Show END\r\n");

    // go to sleep
    delay(100);     
    EPD_SendCommand(0x07);//deep sleep
    EPD_SendData(0xA5);
    delay(100);
}

int EPD_7IN3E_init() 
{
    Serial.print("EPD_7IN3E_init Begin\r\n");
    EPD_Reset();
    EPD_7IN3E_BusyHigh();

    EPD_SendCommand(0xAA);    // CMDH
    EPD_SendData(0x49);
    EPD_SendData(0x55);
    EPD_SendData(0x20);
    EPD_SendData(0x08);
    EPD_SendData(0x09);
    EPD_SendData(0x18);

    // PWRR
    EPD_SendCommand(0x01);
    EPD_SendData(0x3F);

    // PSR
    EPD_SendCommand(0x00);
    EPD_SendData(0x5F);
    EPD_SendData(0x69);

    // POFS
    EPD_SendCommand(0x03);
    EPD_SendData(0x00);
    EPD_SendData(0x54);
    EPD_SendData(0x00);
    EPD_SendData(0x44); 

    // BTST1
    EPD_SendCommand(0x05);
    EPD_SendData(0x40);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x2C);

    // BTST2
    EPD_SendCommand(0x06);
    EPD_SendData(0x6F);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x22);

    // BTST3
    EPD_SendCommand(0x08);
    EPD_SendData(0x6F);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x22);

    // PLL
    EPD_SendCommand(0x30);
    // EPD_SendData(0x03);
    EPD_SendData(0x08);

    // CDI
    EPD_SendCommand(0x50);
    EPD_SendData(0x3F);

    // TCON
    EPD_SendCommand(0x60);
    EPD_SendData(0x02);
    EPD_SendData(0x00);

    // TRES
    EPD_SendCommand(0x61);
    EPD_SendData(0x03);
    EPD_SendData(0x20);
    EPD_SendData(0x01); 
    EPD_SendData(0xE0);

    // T_VDCS
    EPD_SendCommand(0x84);
    EPD_SendData(0x01);

    // PWS
    EPD_SendCommand(0xE3);
    EPD_SendData(0x2F);


    EPD_SendCommand(0x10); //begin write data to e-Paper

    Serial.print("EPD_7IN3E_init End\r\n");
	
    return 0;
}
