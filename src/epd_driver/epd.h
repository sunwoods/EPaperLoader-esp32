/**
  ******************************************************************************
  * @file    epd.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    23-January-2018
  * @brief   This file provides e-Paper driver functions
  *           void EPD_SendCommand(byte command);
  *           void EPD_SendData(byte data);
  *           void EPD_WaitUntilIdle();
  *           void EPD_Send_1(byte c, byte v1);
  *           void EPD_Send_2(byte c, byte v1, byte v2);
  *           void EPD_Send_3(byte c, byte v1, byte v2, byte v3);
  *           void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4);
  *           void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5);
  *           void EPD_Reset();
  *           void EPD_dispInit();
  *           
  *          varualbes:
  *           EPD_dispLoad;                - pointer on current loading function
  *           EPD_dispIndex;               - index of current e-Paper
  *           EPD_dispInfo EPD_dispMass[]; - array of e-Paper properties
  *           
  ******************************************************************************
  */
#ifndef EPD_H_
#define EPD_H_
//  #include "epd_wifi.h"
#include <Arduino.h>

void EPD_SPISetCfg(
  int din,
  int sck,
  int cs,
  int dc,
  int rst,
  int busy
);
void EPD_initSPI();

/* The procedure of sending a byte to e-Paper by SPI -------------------------*/
void EpdSpiTransferCallback(byte data);

/* Sending a byte as a command -----------------------------------------------*/
void EPD_SendCommand(byte command);

/* Sending a byte as a data --------------------------------------------------*/
void EPD_SendData(byte data);

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle();

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle_high();
void EPD_WaitUntilIdle_high(int timeoutInMil);
/* Send a one-argument command -----------------------------------------------*/
void EPD_Send_1(byte c, byte v1);

/* Send a two-arguments command ----------------------------------------------*/
void EPD_Send_2(byte c, byte v1, byte v2);

/* Send a three-arguments command --------------------------------------------*/
void EPD_Send_3(byte c, byte v1, byte v2, byte v3);

/* Send a four-arguments command ---------------------------------------------*/
void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4);

/* Send a five-arguments command ---------------------------------------------*/
void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5);

/* Writting lut-data into the e-Paper ----------------------------------------*/
void EPD_lut(byte c, byte l, byte*p);

/* Writting lut-data of the black-white channel ------------------------------*/
void EPD_SetLutBw(byte*c20, byte*c21, byte*c22, byte*c23, byte*c24);

/* Writting lut-data of the red channel --------------------------------------*/
void EPD_SetLutRed(byte*c25, byte*c26, byte*c27);

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
void EPD_Reset();

/* Image data loading function for a-type e-Paper ----------------------------*/ 
void EPD_loadA();
void EPD_loadAFilp();

/* Image data loading function for b-type e-Paper ----------------------------*/
void EPD_loadB();

/* Image data loading function for 2.13 e-Paper ------------------------------*/
void EPD_loadC();

/* Image data loading function for 7.5 e-Paper -------------------------------*/
void EPD_loadD();
/* Image data loading function for 7.5b e-Paper ------------------------------*/
void EPD_loadE();

/* Image data loading function for 5.83b e-Paper -----------------------------*/
void EPD_loadF();
/* Image data loading function for 5.65f e-Paper -----------------------------*/
void EPD_loadG();

/* Show image and turn to deep sleep mode (a-type, 4.2 and 2.7 e-Paper) ------*/
void EPD_showA();

/* Show image and turn to deep sleep mode (b-type, e-Paper) ------------------*/
void EPD_showB();

/* Show image and turn to deep sleep mode (7.5 and 7.5b e-Paper) -------------*/
void EPD_showC();

/* Show image and turn to deep sleep mode (2.13 e-Paper) ---------------------*/
void EPD_showD();

/* Initialization of an e-Paper ----------------------------------------------*/
void EPD_dispInit();

bool Srvr__loop();
bool Srvr__init();

void Srvr__rcvProc();
void Srvr__postProc();

void reboot_device();

#endif /* EPD_H_ */