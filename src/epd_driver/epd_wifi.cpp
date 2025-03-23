
#include <ArduinoJson.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "epd_conf.h"
#include "epd.h"
#include "buff/buff_wifi.h"

#include "epd_wifi.h"

EPDWifi myEPDWifi;

// cost time record
int start_time = -1;
int end_time = -1;
int last_time = 0;

int loaded_size = 0;
int bw_loaded_index = 0;
int r_loaded_index = 0;
bool cmd_s2 = false;

bool EPDWifi::Srvr__loop()
{
    server.handleClient();
    return true;
}

/* Reads a word from the buffer at specified position ------------------------*/
int EPDWifi::Buff__getByte(char* pBuf, int index)
{
    // The first and second characters of the byte stored in the buffer
    // are supposed to be in range ['a'; 'p'], otherwise it isn't a image data's byte
    if ((pBuf[index    ] < 'a') || (pBuf[index    ] > 'p')) return -1;
    if ((pBuf[index + 1] < 'a') || (pBuf[index + 1] > 'p')) return -1;

    // The character 'a' means 0, the character 'p' means 15 consequently,
    // The 1st character describes 4 low bits if the byte and the 2nd one - 4 high bits 
    return ((int)pBuf[index] - 'a') + (((int)pBuf[index + 1] - 'a') << 4);
}

// A类型，每个value代表8个像素
void EPDWifi::EPD_loadCompressA(char* pBuf) {
    int pos = 0;
    int buf_size = strlen(pBuf);
    Serial.printf("buf_size=%d, next_stage=%d\n", buf_size, next_stage);
    // Enumerate all of image data bytes
    while (pos < buf_size)
    {
        // Get current byte
        int value = Buff__getByte(pBuf, pos);
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's memory
        if (next_stage && (EPD_dispMass[EPD_dispIndex].chBk != EPD_dispMass[EPD_dispIndex].chRd)) {
            EPD_SendData(~(byte)value);
        } else {
            EPD_SendData((byte)value);
        }

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

// A类型，每个value代表8个像素
void EPDWifi:: EPD_loadCompressA2(char* pBuf) {
    int pos = 0;
    int x = 0;
    int y = 0;
    int pixel_index = 0;
    int buf_size = strlen(pBuf);
    Serial.println("EPD_loadCompressA2");
    Serial.printf("buf_size=%d, loaded_size=%d, next_stage=%d\n", buf_size, loaded_size, next_stage);

    return;
    // Enumerate all of image data bytes
    while (pos < buf_size)
    {
        // 计算像素点对应的坐标(x,y)，判断落在哪个区域
        pixel_index = (pos + loaded_size) * 8 / 2;
        x = pixel_index % EPD_12IN48B_WIDTH;
        y = pixel_index / EPD_12IN48B_WIDTH;

        // Get current byte
        int value = Buff__getByte(pBuf, pos);
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;


        // 非等分...    
        if (x < 648 && y < EPD_12IN48B_HEIGHT/2) {      
            // Write the byte into e-Paper's memory
            if (cmd_s2 != true) {
                EPD_12in48B_cmd1S2();
                cmd_s2 = true;
            }
            if (next_stage && (EPD_dispMass[EPD_dispIndex].chBk != EPD_dispMass[EPD_dispIndex].chRd)) {
                EPD_12in48B_data1S2(~(byte)value);
            } else {
                EPD_12in48B_data1S2((byte)value);
            }
        }

        // Increment the current byte index on 2 characters
        pos += 2;
    }

    loaded_size += buf_size;
}

// G类型，每个vanlue代表2个像素
void EPDWifi::EPD_loadCompressG(char* pBuf) {
    int pos = 0;
    int buf_size = strlen(pBuf);
    // Enumerate all of image data
    while (pos < buf_size)
    {
        // Get current byte
        int value = Buff__getByte(pBuf, pos);
        // int value = pBuf[pos];
		
        // Switch the positions of the two 4-bits pixels
        // Black:0b000;White:0b001;Green:0b010;Blue:0b011;Red:0b100;Yellow:0b101;Orange:0b110;
        int A = (value     ) & 0x07;
        int B = (value >> 4) & 0x07;
		
        // Write the data into e-Paper's memory
        EPD_SendData((byte)(A << 4) + B);

        pos += 2;
    }
}

void EPDWifi::EPD_loadCompress(char* pBuf) {
    if (EPD_dispIndex == 43) { // 7.3f
        EPD_loadCompressG(pBuf);
    } else if (EPD_dispIndex == 44) { // 12.48b
        EPD_loadCompressA2(pBuf);
    } else {
        EPD_loadCompressA(pBuf);
    }
}

void EPDWifi::msgProc(char* pBuf) {
    if (0 == strncmp(pBuf, "NEXT", sizeof("NEXT"))) {
        
        end_time = millis();
        Serial.printf("----%dms----\n", end_time - last_time - start_time);
        last_time = end_time - start_time;

        Serial.println("NEXT");

        next_stage = true;

        // Instruction code for for writting data into
        // e-Paper's memory
        int code = EPD_dispMass[EPD_dispIndex].next;

        // If the instruction code isn't '-1', then...
        if (code != -1) {
            // Print log message: instruction code
            Serial.printf(" %d", code);

            // Do the selection of the next data channel
            if (EPD_dispIndex == 44) {
                Serial.println("12.48 next");
                // EPD_SendCommand(code);
            } else {
                EPD_SendCommand(code);
            }
            delay(2);
        }

        // Setup the function for loading choosen channel's data
        EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

        loaded_size = 0;
    } else if (0 == strncmp(pBuf, "SHOW", sizeof("SHOW"))) {
        
        end_time = millis();
        Serial.printf("----%dms----\n", end_time - last_time - start_time);
        last_time = end_time - start_time;

        Serial.print("<<<SHOW_START");
        EPD_dispMass[EPD_dispIndex].show();
        Serial.println("SHOW");
        next_stage = false;
        
        end_time = millis();
        Serial.printf("----%dms----\n", end_time - last_time - start_time);
        last_time = end_time - start_time;

    } else {
        Serial.println("LOAD");
        EPD_loadCompress(pBuf);
    }
    return;
}

void EPDWifi::Web_EPD_Init(String arg_info) {
    start_time = millis();
    last_time = 0;

    Serial.print("\r\nEPD\r\n");
    Serial.println(arg_info);
    // config load mode
    loadMode = LOAD_MODE_WIFI;

    deserializeJson(document, arg_info);

    EPD_dispIndex = document["epdInd"];
    // temporarily compatible with web
    if (EPD_dispIndex == 0) {
        EPD_dispIndex = 44;
    }

    // 12.48 use specific pins initialization
    if (EPD_dispIndex == 44) {
        EPD_12in48b_initSPI();
    } else if (EPD_dispIndex == 46) {
        EPD_9IN69B_InitSPI();
    } else {            
        EPD_SPISetCfg(document["din"], document["sck"], document["cs"], document["dc"], document["rst"], document["busy"]);
        EPD_initSPI();
        Serial.printf("width=%d, height=%d\n", document["width"].as<int>(), document["height"].as<int>());
    }

    // Print log message: initialization of e-Paper (e-Paper's type)
    Serial.printf("EPD_dispIndex %d", EPD_dispIndex);
    Serial.printf("EPD %s", EPD_dispMass[EPD_dispIndex].title);

    // Initialization
    EPD_dispInit();

    loaded_size = 0;
    bw_loaded_index = 0;
    r_loaded_index = 0;
    cmd_s2 = false;
    
    end_time = millis();
    Serial.printf("----%dms----\n", end_time - last_time - start_time);
    last_time = end_time - start_time;
}

void EPDWifi::EPD_12in48B_loadA(String image_data) {
    UDOUBLE index = 0;
    while (index < image_data.length()) {
        // Get current byte
        int value = ((((int)image_data[index] - 'a') << 4) + (((int)image_data[index + 1] - 'a') & 0x0f));
        // total resolution 1304 * 984
        // devided into 4 parts： (648*492 + 656*492 + 656*492 + 648*492) 
        // according index within range, use different cmd
        if(bw_loaded_index == 0){ // 162 * 492 / 2
            Serial.println("12.48inch e-Paper B LoadA S2");
            EPD_12in48B_cmd1S2();
        }else if(bw_loaded_index == 39852){ // 164 * 492 / 2，  = 492 * (648/8）
            Serial.println("12.48inch e-Paper B LoadA M2");
            EPD_12in48B_cmd1M2();
        }else if(bw_loaded_index == 80196){ // 162 * 492 / 2，  = 492 * (656/8)
            Serial.println("12.48inch e-Paper B LoadA M1");
            EPD_12in48B_cmd1M1();
        }else if(bw_loaded_index == 120048){ // 164 * 492 / 2
            Serial.println("12.48inch e-Paper B LoadA S1");
            EPD_12in48B_cmd1S1();
        }

        if(bw_loaded_index < 39852){
            EPD_12in48B_data1S2((byte)value);
        }else if(bw_loaded_index < 80196 && bw_loaded_index >= 39852){
            EPD_12in48B_data1M2((byte)value);
        }else if(bw_loaded_index < 120048 && bw_loaded_index >= 80196){
            EPD_12in48B_data1M1((byte)value);
        }else if(bw_loaded_index >= 120048){
            EPD_12in48B_data1S1((byte)value);
        }

        // Increment the current byte index on 2 characters
        index += 2;
        bw_loaded_index++;
    }
}

void EPDWifi::EPD_9IN69B_loadA(String image_data) {
    UDOUBLE index = 0;
    while (index < image_data.length()) {
        // Get current byte
        int value = ((((int)image_data[index] - 'a') << 4) + (((int)image_data[index + 1] - 'a') & 0x0f));
        value = ~value;
        // total resolution 960 * 672
        // devided into 2 parts： 480 * 672 + 480 * 672
        // total data num: 960*672 * 2 / 8 = 161280
        // each parts has data num：161280 / 2 = 80640
        // pixels seperated line: 80640 / 2 = 40320
        // according index within range, use different cmd
        if(bw_loaded_index == 0){
            Serial.println("9.69inch e-Paper B LoadA Master");
            EPD_9IN69B_Prepare();
            EPD_9IN69B_SendCommandM(0x12);
            EPD_9IN69B_SendDataM(0x3b);
            EPD_9IN69B_SendDataM(0x00);
            EPD_9IN69B_SendDataM(0x14);
            //First frame for Master
            EPD_9IN69B_SendCommandM(0x10);
        }else if(bw_loaded_index == 40320){
            Serial.println("12.48inch e-Paper B LoadA Slave");
            EPD_9IN69B_SendCommandS(0x12);
            EPD_9IN69B_SendDataS(0x3b);
            EPD_9IN69B_SendDataS(0x00);
            EPD_9IN69B_SendDataS(0x14);
            //First frame for Slave
            EPD_9IN69B_SendCommandS(0x10);
        }

        if(bw_loaded_index < 40320){
            EPD_9IN69B_SendDataM((byte)value);
        } else {
            EPD_9IN69B_SendDataS((byte)value);
        }

        // Increment the current byte index on 2 characters
        index += 2;
        bw_loaded_index++;
    }
}

void EPDWifi::Web_EPD_LoadA(String arg_info) {
    Serial.print("L>");

    String image_data = arg_info;
    Buff__bufInd = image_data.length();
    memset(Buff__bufArr, 0, Buff__bufInd);
    memcpy(Buff__bufArr, image_data.c_str(), Buff__bufInd);
    Buff__bufArr[Buff__bufInd] = '\0';

    loaded_size += Buff__bufInd;
    Serial.printf("Buff__bufInd=%d, loaded_size=%d\n", Buff__bufInd, loaded_size);

    Serial.println("LOADA");

    // 12.48 & 9.69 use specific pins initialization
    if (EPD_dispIndex == 44) {
        return EPD_12in48B_loadA(image_data);
    } else if (EPD_dispIndex == 46) {
        return EPD_9IN69B_loadA(image_data);
    }

    EPD_dispLoad();
}

void EPDWifi::EPD_12in48B_loadB(String image_data) {
    UDOUBLE index = 0;
    while (index < image_data.length()) {
        // Get current byte
        int value = ((((int)image_data[index] - 'a') << 4) + (((int)image_data[index + 1] - 'a') & 0x0f));
        
        if(r_loaded_index == 0){ // 162 * 492 / 2
            Serial.println("Web_EPD_LoadB 0");
            EPD_12in48B_cmd2S2();
        }else if(r_loaded_index == 39852){ // 164 * 492 / 2
            Serial.println("Web_EPD_LoadB 2");
            EPD_12in48B_cmd2M2();
        }else if(r_loaded_index == 80196){ // 162 * 492 / 2
            Serial.println("Web_EPD_LoadB 6");
            EPD_12in48B_cmd2M1();
        }else if(r_loaded_index == 120048){ // 164 * 492 / 2
            Serial.println("Web_EPD_LoadB 8");
            EPD_12in48B_cmd2S1();
        }
    
        if(r_loaded_index < 39852){
            EPD_12in48B_data2S2((byte)value);
        }else if(r_loaded_index < 80196 && r_loaded_index >= 39852){
            EPD_12in48B_data2M2((byte)value);
        }else if(r_loaded_index < 120048 && r_loaded_index >= 80196){
            EPD_12in48B_data2M1((byte)value);
        }else if(r_loaded_index >= 120048){
            EPD_12in48B_data2S1((byte)value);
        }
    
        // Increment the current byte index on 2 characters
        index += 2;
        r_loaded_index++;
    }
}

void EPDWifi::EPD_9IN69B_loadB(String image_data) {
    UDOUBLE index = 0;
    while (index < image_data.length()) {
        // Get current byte
        int value = ((((int)image_data[index] - 'a') << 4) + (((int)image_data[index + 1] - 'a') & 0x0f));
        value = ~value;
        // total resolution 960 * 672
        // devided into 2 parts： 480 * 672 + 480 * 672
        // total data num: 960*672 * 2 / 8 = 161280
        // each parts has data num：161280 / 2 = 80640
        // pixels seperated line: 80640 / 2 = 40320
        // according index within range, use different cmd
        if(r_loaded_index == 0){
            Serial.println("9.69inch e-Paper B LoadB Master");
            // RAM_RW for Master
            EPD_9IN69B_Prepare();
            EPD_9IN69B_SendCommandM(0x12);
            EPD_9IN69B_SendDataM(0x3b);
            EPD_9IN69B_SendDataM(0x00);
            EPD_9IN69B_SendDataM(0x14);
            //Second frame for Master
            EPD_9IN69B_SendCommandM(0x11);
        }else if(r_loaded_index == 40320){
            Serial.println("12.48inch e-Paper B LoadB Slave");
            // RAM_RW for Slave
            EPD_9IN69B_SendCommandS(0x12);
            EPD_9IN69B_SendDataS(0x3b);
            EPD_9IN69B_SendDataS(0x00);
            EPD_9IN69B_SendDataS(0x14);
            //Second frame for Slave
            EPD_9IN69B_SendCommandS(0x11);
        }

        if(r_loaded_index < 40320){
            EPD_9IN69B_SendDataM((byte)value);
        } else {
            EPD_9IN69B_SendDataS((byte)value);
        }

        // Increment the current byte index on 2 characters
        index += 2;
        r_loaded_index++;
    }
}

void EPDWifi::Web_EPD_LoadB(String arg_info) {    
    Serial.print("L>");

    String image_data = arg_info;
    Buff__bufInd = image_data.length();

    memset(Buff__bufArr, 0, Buff__bufInd);
    memcpy(Buff__bufArr, image_data.c_str(), Buff__bufInd);
    Buff__bufArr[Buff__bufInd] = '\0';
    loaded_size += Buff__bufInd;
    Serial.printf("Buff__bufInd=%d, loaded_size=%d\n", Buff__bufInd, loaded_size);

    Serial.println("LOADB");

    // 12.48 & 9.69 use specific pins initialization
    if (EPD_dispIndex == 44) {
        return EPD_12in48B_loadB(image_data);
    } else if (EPD_dispIndex == 46) {
        return EPD_9IN69B_loadB(image_data);
    }


    // change to next
    if (next_stage != true) {
        next_stage = true;

        // next
        int code = EPD_dispMass[EPD_dispIndex].next;

        // If the instruction code isn't '-1', then...
        if (code != -1) {
            // Print log message: instruction code
            Serial.printf(" %d", code);

            // Do the selection of the next data channel
            EPD_SendCommand(code);
            delay(2);
        }

        // Setup the function for loading choosen channel's data
        EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

        loaded_size = 0;
    }

    EPD_dispLoad();    
}

void EPDWifi::Web_EPD_Show() {
    end_time = millis();
    Serial.printf("----%dms----\n", end_time - last_time - start_time);
    last_time = end_time - start_time;

    Serial.print("<<<SHOW_START");
    EPD_dispMass[EPD_dispIndex].show();
    Serial.println("SHOW");
    next_stage = false;
    
    end_time = millis();
    Serial.printf("----%dms----\n", end_time - last_time - start_time);
    last_time = end_time - start_time;
}

void EPDWifi::Srvr__postProc() {
    String uri = server.uri();
    String method = (server.method() == HTTP_GET) ? "GET" : "POST";
    int arg_num = server.args();
    int i = 0;
    String arg_info = "";

    // parse first param
    arg_info += server.arg(0);
    Serial.println(uri);

    if (uri == "/EPD") {
        Web_EPD_Init(arg_info);
    }

    if (uri == "/LOADA") {
        Web_EPD_LoadA(arg_info);
    }

    if (uri == "/LOADB") {
        Web_EPD_LoadB(arg_info);
    }

    if (uri == "/SHOW") {
        Web_EPD_Show();
    }

    // Sent to the 'client' the header describing the type of data.
    // In this case 'Content-Type' is 'text/html'
    server.send(200, "text/html", "Ok!\r\n");

    delay(1);
}
