#ifndef EPD_BT_H_
#define EPD_BT_H_

#include "Arduino.h"
#include <WiFi.h>
#include <BLEServer.h>

#define BT_BUFFER_SIZE 512

// define bluetooth async data structure
typedef struct {
    int data_len;
    char Buff__bufArr[BT_BUFFER_SIZE];
} bt_data_t;

class EPDBt {
public:

    void EPD_loadA();
    void EPD_loadAFilp();
    void EPD_loadG();
    
    void EPD_loadCompress(bool flip);
    void EPD_loadCompressA(bool flip);
    void EPD_loadCompressG(bool flip);

    void EPD_loadUncompress(bool flip);
    void EPD_loadUncompressA(bool flip);
    void EPD_loadUncompressG(bool flip);

    void Srvr__rcvProc();
    bool Srvr__init();
    void msgProc();

private:

}; // class EPDBt

extern EPDBt myEPDBt;


#endif