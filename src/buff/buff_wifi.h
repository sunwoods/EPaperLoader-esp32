/**
  ******************************************************************************
  * @file    buff.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    23-January-2018
  * @brief   ESP8266 WiFi server.
  *          This file provides firmware functions:
  *           + Sending web page of the tool to a client's browser
  *           + Uploading images from client part by part
  *
  ******************************************************************************
  */ 

 #ifndef WIFI_BUFF_H_
 #define WIFI_BUFF_H_
 
#include "Arduino.h"
#include "buff.h"

class BUFFWifi {
public:
  /* Reads a word from the buffer at specified position ------------------------*/
  static int Buff__getByte(int index);

  /* Reads a byte from the buffer at specified position ------------------------*/
  static int Buff__getWord(int index);

  /* Reads a byte from the buffer at specified position ------------------------*/
  static int Buff__getN3(int index);

  /* Checks if the buffer's data ends with specified string --------------------*/
  static int Buff__signature(int index, char*str);
}; // class BUFFWifi

extern BUFFWifi myBUFFWifi;

#endif