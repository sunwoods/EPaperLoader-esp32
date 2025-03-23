#include <Arduino.h>

#include "buff_wifi.h"
#include "buff_bt.h"
#include "epd_driver/epd_conf.h"


JsonDocument document;

int     Buff__bufInd;
char* Buff__bufArr = NULL;

// init buffer
void Buff__init() {
    Buff__bufArr = (char*)malloc(Buff__SIZE);
    if (Buff__bufArr == NULL) {
        Serial.println("malloc for Buff__bufArr failed. exit!");
        exit(-2);
    }
    memset(Buff__bufArr, 0, Buff__SIZE);
}

/* Reads a word from the buffer at specified position ------------------------*/
int Buff__getByte(int index)
{
    if (loadMode == LOAD_MODE_BT) {
        return myBUFFBt.Buff__getByte(index);
    } else if (loadMode == LOAD_MODE_WIFI) {
        return myBUFFWifi.Buff__getByte(index);
    }
}

/* Reads a byte from the buffer at specified position ------------------------*/
int Buff__getWord(int index)
{
    if (loadMode == LOAD_MODE_BT) {
        return myBUFFBt.Buff__getWord(index);
    } else if (loadMode == LOAD_MODE_WIFI) {
        return myBUFFWifi.Buff__getWord(index);
    }
}

/* Reads a byte from the buffer at specified position ------------------------*/
int Buff__getN3(int index)
{
    if (loadMode == LOAD_MODE_BT) {
        return myBUFFBt.Buff__getN3(index);
    } else if (loadMode == LOAD_MODE_WIFI) {
        return myBUFFWifi.Buff__getN3(index);
    }
}

/* Checks if the buffer's data ends with specified string --------------------*/
int Buff__signature(int index, char*str)
{
    if (loadMode == LOAD_MODE_BT) {
        return myBUFFBt.Buff__signature(index, str);
    } else if (loadMode == LOAD_MODE_WIFI) {
        return myBUFFWifi.Buff__signature(index, str);
    }
}