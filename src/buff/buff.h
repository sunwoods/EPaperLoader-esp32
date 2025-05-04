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

#include <ArduinoJson.h>

/* Size, current position index and byte array of the buffer -----------------*/
// #define Buff__SIZE 800*480  // may need increase
#define Buff__SIZE 1920*1080 // increase for ES108FC

#define JSON_BUFF_SIZE 256

extern JsonDocument document;

extern int   Buff__bufInd;
extern char* Buff__bufArr;

void Buff__init();

/* Reads a word from the buffer at specified position ------------------------*/
int Buff__getByte(int index);
/* Reads a byte from the buffer at specified position ------------------------*/
int Buff__getWord(int index);

/* Reads a byte from the buffer at specified position ------------------------*/
int Buff__getN3(int index);

/* Checks if the buffer's data ends with specified string --------------------*/
int Buff__signature(int index, char*str);
