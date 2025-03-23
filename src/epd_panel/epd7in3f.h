/**
  ******************************************************************************
  * @file    edp7in3f.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    29-Dec-2020
  * @brief   This file describes initialisation of 7.3f e-Papers
  *
  ******************************************************************************
  */

/*****************************************************************************
                      EPD_4IN01F
******************************************************************************/
#include <esp_task_wdt.h>
static void EPD_7IN3F_BusyHigh(void)// If BUSYN=0 then waiting
{
    while(!(digitalRead(PIN_SPI_BUSY)));
}

static void EPD_7IN3F_BusyLow(void)// If BUSYN=1 then waiting
{
    while(digitalRead(PIN_SPI_BUSY));
}

static void EPD_7IN3F_Show(void)
{
    Serial.println("EPD_7IN3F_Show Begin");

    Serial.println("Turn on display.");
    // turn on display
    EPD_SendCommand(0x04);//0x04
    EPD_7IN3F_BusyHigh();

    EPD_SendCommand(0x12);//0x12
    EPD_SendData(0x00);
    EPD_7IN3F_BusyHigh();

    EPD_SendCommand(0x02);//0x02
    EPD_SendData(0x00);
    EPD_7IN3F_BusyHigh();
    
	delay(200);
    Serial.print("EPD_7IN3F_Show END\r\n");

    // go to sleep
    delay(100);     
    EPD_SendCommand(0x07);//sleep
    EPD_SendData(0xA5);
    delay(100);
}

int EPD_7IN3F_init() 
{
    Serial.print("EPD_7IN3F_init Begin\r\n");
    EPD_Reset();
    EPD_7IN3F_BusyHigh();

    EPD_SendCommand(0xAA);    // CMDH
    EPD_SendData(0x49);
    EPD_SendData(0x55);
    EPD_SendData(0x20);
    EPD_SendData(0x08);
    EPD_SendData(0x09);
    EPD_SendData(0x18);

    EPD_SendCommand(0x01);
    EPD_SendData(0x3F);
    EPD_SendData(0x00);
    EPD_SendData(0x32);
    EPD_SendData(0x2A);
    EPD_SendData(0x0E);
    EPD_SendData(0x2A);

    EPD_SendCommand(0x00);
    EPD_SendData(0x5F);
    EPD_SendData(0x69);

    EPD_SendCommand(0x03);
    EPD_SendData(0x00);
    EPD_SendData(0x54);
    EPD_SendData(0x00);
    EPD_SendData(0x44); 

    EPD_SendCommand(0x05);
    EPD_SendData(0x40);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x2C);

    EPD_SendCommand(0x06);
    EPD_SendData(0x6F);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x22);

    EPD_SendCommand(0x08);
    EPD_SendData(0x6F);
    EPD_SendData(0x1F);
    EPD_SendData(0x1F);
    EPD_SendData(0x22);

    EPD_SendCommand(0x13);    // IPC
    EPD_SendData(0x00);
    EPD_SendData(0x04);

    EPD_SendCommand(0x30);
    EPD_SendData(0x3C);

    EPD_SendCommand(0x41);     // TSE
    EPD_SendData(0x00);

    EPD_SendCommand(0x50);
    EPD_SendData(0x3F);

    EPD_SendCommand(0x60);
    EPD_SendData(0x02);
    EPD_SendData(0x00);

    EPD_SendCommand(0x61);
    EPD_SendData(0x03);
    EPD_SendData(0x20);
    EPD_SendData(0x01); 
    EPD_SendData(0xE0);

    EPD_SendCommand(0x82);
    EPD_SendData(0x1E); 

    EPD_SendCommand(0x84);
    EPD_SendData(0x00);

    EPD_SendCommand(0x86);    // AGID
    EPD_SendData(0x00);

    EPD_SendCommand(0xE3);
    EPD_SendData(0x2F);

    EPD_SendCommand(0xE0);   // CCSET
    EPD_SendData(0x00); 

    EPD_SendCommand(0xE6);   // TSSET
    EPD_SendData(0x00);

    // config resolution
    EPD_SendCommand(0x61);
    EPD_SendData(0x03);
    EPD_SendData(0x20);
    EPD_SendData(0x01); 
    EPD_SendData(0xE0);
    EPD_SendCommand(0x10); //begin write data to e-Paper

    Serial.print("EPD_7IN3F_init End\r\n");
	
    return 0;
}
