#ifndef EPD_WIFI_H_
#define EPD_WIFI_H_

#include "Arduino.h"
#include <WiFi.h>

#include <WebServer.h>

#define WIFI_BUFFER_SIZE 2048

// 定义结构体类型
typedef struct {
    int data_len;
    char Buff__bufArr[WIFI_BUFFER_SIZE];
} wifi_data_t;

class EPDWifi {
public:
    void EPD_12in48B_loadA(String image_data);
    void EPD_12in48B_loadB(String image_data);

    void EPD_9IN69B_loadA(String image_data);
    void EPD_9IN69B_loadB(String image_data);

    void Web_EPD_Init(String arg_info);
    void Web_EPD_LoadA(String arg_info);
    void Web_EPD_LoadB(String arg_info);
    void Web_EPD_Show();

    void EPD_loadCompress(char* pBuf);
    void EPD_loadCompressA(char* pBuf);
    void EPD_loadCompressA2(char* pBuf);
    void EPD_loadCompressG(char* pBuf);
    int Buff__getByte(char* pBuf, int index);

    bool Srvr__loop();

    bool Srvr__init();
    void Srvr__postProc();
    void msgProc(char* pBuf);
private:

}; // class EPDWifi

extern EPDWifi myEPDWifi;

/* Server and IP address ------------------------------------------------------*/
// extern WiFiServer server; // Wifi server exemplar using port 80
extern WebServer server; // Web server exemplar using port 80
extern IPAddress myIP;        // IP address in your local wifi net


#define EPD_12IN48B_WIDTH 1304
#define EPD_12IN48B_HEIGHT 984

extern void EPD_12in48b_initSPI(void);
extern void EPD_12in48B_cmd1S2(void);
extern void EPD_12in48B_cmd1M2(void);
extern void EPD_12in48B_cmd1M1(void);
extern void EPD_12in48B_cmd1S1(void);
extern void EPD_12in48B_data1S2(UBYTE data);
extern void EPD_12in48B_data1M2(UBYTE data);
extern void EPD_12in48B_data1M1(UBYTE data);
extern void EPD_12in48B_data1S1(UBYTE data);
extern void EPD_12in48B_cmd2S2(void);
extern void EPD_12in48B_cmd2M2(void);
extern void EPD_12in48B_cmd2M1(void);
extern void EPD_12in48B_cmd2S1(void);
extern void EPD_12in48B_data2S2(UBYTE data);
extern void EPD_12in48B_data2M2(UBYTE data);
extern void EPD_12in48B_data2M1(UBYTE data);
extern void EPD_12in48B_data2S1(UBYTE data);

extern void EPD_9IN69B_InitSPI(void);
extern void EPD_9IN69B_Prepare();
extern void EPD_9IN69B_SendCommandM(UBYTE Reg);
extern void EPD_9IN69B_SendDataM(UBYTE Data);
extern void EPD_9IN69B_SendCommandS(UBYTE Reg);
extern void EPD_9IN69B_SendDataS(UBYTE Data);
#endif