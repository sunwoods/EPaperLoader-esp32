#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "buff/buff.h"
#include "epd_conf.h"
#include "epd.h"
#include "config/ble_config.h"
#include "epd_bt.h"

EPDBt myEPDBt;
QueueHandle_t btQueue;
TaskHandle_t btTask;

bt_data_t btInBuf;
bt_data_t btOutBuf;

/* Image data loading function for a-type e-Paper ----------------------------*/ 
void EPDBt::EPD_loadA()
{
    if (is_compress) {
        EPD_loadCompressA(false);
    } else {
        EPD_loadUncompressA(false);
    }
    return;
}

void EPDBt::EPD_loadAFilp()
{
    if (is_compress) {
        EPD_loadCompressA(true);
     } else {
        EPD_loadUncompressA(true);
     }
     return;
}

/* Image data loading function for 5.65f e-Paper -----------------------------*/
void EPDBt::EPD_loadG()
{
    if (is_compress) {
        EPD_loadCompressG(false);
    } else {
        EPD_loadUncompressG(false);
    }
    return;
}


void EPDBt::msgProc() {

    char *pBuf = btOutBuf.Buff__bufArr;
    int data_len = btOutBuf.data_len;

    int width = 0;
    int height = 0;

    // Initialization
    if (pBuf[0] == 'I' && data_len == 13)
    {
        // config load mode
        loadMode = LOAD_MODE_BT;

        // Getting of e-Paper's type
        // 1 -> index
        EPD_dispIndex = pBuf[1];

        // 2 -> is_compress
        if (pBuf[2] != 0) {
            is_compress = true;
        } else {
            is_compress = false;
        }

        Serial.printf("is_compress: %d\n", is_compress);
        // Print log message: initialization of e-Paper (e-Paper's type)
        Serial.printf("<<<EPD %d, %s\n", EPD_dispIndex, EPD_dispMass[EPD_dispIndex].title);

        // 3~8 -> spi pin
        if (data_len > 3) {
            EPD_SPISetCfg(pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7], pBuf[8]);
            width = (pBuf[9] << 8) + pBuf[10];
            height = (pBuf[11] << 8) + pBuf[12];
            Serial.printf("width=%d, height=%d\n", width, height);
        }
        EPD_initSPI();

        // alloc image buffer
        if (rcv_image_data == NULL) {
            rcv_image_data = (uint8_t*)malloc(width*height);
            image_data = (uint8_t*)malloc(width*height*2);
            if (rcv_image_data == NULL || image_data == NULL) {
                Serial.println("malloc failed.");
                exit(-1);
            }
            Serial.printf("rcv_image_data=%p, image_data=%p\n", rcv_image_data, image_data);
        }
        rcv_image_data_size = 0;
        image_data_size = 0;

        esp_gap_conn_params_t connParams;
        esp_ble_conn_update_params_t updateParams;
        memcpy(updateParams.bda, connectedAddress, sizeof(esp_bd_addr_t));
        updateParams.min_int = 6;
        updateParams.max_int = 6;
        updateParams.latency = 0;
        updateParams.timeout = 500;
        esp_ble_gap_update_conn_params(&updateParams);

        esp_ble_get_current_conn_params(connectedAddress, &connParams);
        Serial.printf("interval=%d, latency=%d, timeout=%d\n", connParams.interval, connParams.latency, connParams.timeout);

        // Initialization
        EPD_dispInit();
    }

    // Loading of pixels' data
    else if (pBuf[0] == 'L' && data_len == 1)
    {
        // Print log message: image loading
        Serial.print("<<<LOAD");
        load_stage = true;
    }

    // Initialize next channel
    else if (pBuf[0] == 'N' && data_len == 1)
    {
        // Print log message: next data channel
        Serial.print("<<<NEXT");

        // load first batch data
        Serial.printf("rcv_image_data_size=%d\n", rcv_image_data_size);
        if (is_compress) {
            decompress_image_data();
        }
        EPD_dispLoad();
        load_stage = false;
        next_stage = true;
        rcv_image_data_size = 0;

        // Instruction code for for writting data into 
        // e-Paper's memory
        int code = EPD_dispMass[EPD_dispIndex].next;

        // e-Paper '2.7' (index 8) needs inverting of image data bits
        EPD_invert = (EPD_dispIndex == 8);

        // If the instruction code isn't '-1', then...
        if (code != -1)
        {
            // Print log message: instruction code
            Serial.printf(" %d", code);

            // Do the selection of the next data channel
            EPD_SendCommand(code);
            delay(2);
        }

        // Setup the function for loading choosen channel's data
        EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;
    }

    // Show loaded picture
    else if (pBuf[0] == 'S' && data_len == 1)
    {
        Serial.print("<<<SHOW_START");

        Serial.printf("rcv_image_data_size=%d\n", rcv_image_data_size);
        if (is_compress) {
            decompress_image_data();
        }
        // EPD_loadCompress();
        if (EPD_dispLoad) {
            EPD_dispLoad();
        }

        // reinit after show
        rcv_image_data_size = 0;
        load_stage = false;
        next_stage = false;
        
        EPD_dispMass[EPD_dispIndex].show();

        //Print log message: show
        Serial.print("<<<SHOW");
    }
    // json
    else if (pBuf[0] == '{' && !load_stage) {
        document.clear();
        deserializeJson(document, pBuf);
        // for example: {"COMMAND":"reboot_device"}
        if (document.containsKey("COMMAND")) {
            String cmd_type = document["COMMAND"].as<String>();
            Serial.println("CMD: " + cmd_type);
            if (cmd_type == "reboot_device") {
                reboot_device();
            } else {
                Serial.println("unknown command - failed!>>>");
            }
        }
    } else {
        if (!load_stage) {
            Serial.print("unknown header - failed!>>>");
        } else {
            Serial.printf(".");
            // Load data into the e-Paper 
            // if there is loading function for current channel (black or red)
            memcpy(&rcv_image_data[rcv_image_data_size], pBuf, data_len);
            rcv_image_data_size += (data_len);
        }
    }

    Buff__bufInd = 0;
}

// process msg
void btRcvTask(void *pvParameters) {
    while (1) {
        xQueueReceive(btQueue, &btOutBuf, portMAX_DELAY);

        myEPDBt.msgProc();

        esp_task_wdt_reset();
    }
    
}

bool EPDBt::Srvr__init() {
    int queLen = 100;

    btQueue = xQueueCreate(queLen, sizeof(bt_data_t));
    
    xTaskCreate(btRcvTask, "BT_RCV", 20480, NULL, tskIDLE_PRIORITY, &btTask);

    return true;
}

// Type A: each value represents 8 pixels
void EPDBt::EPD_loadCompressA(bool flip) {
    int pos = 0;
    // Enumerate all of image data
    for (pos = 0; pos < image_data_size; pos++)
    {
        // Get current byte
        // int value = Buff__getByte(pos);
        int value = image_data[pos];

        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's 
        // if (next_stage && (EPD_dispMass[EPD_dispIndex].chBk != EPD_dispMass[EPD_dispIndex].chRd)) {
        if (flip) {
            EPD_SendData(~(byte)value);
        } else {
            EPD_SendData((byte)value);
        }
    }
}

void EPDBt::EPD_loadUncompressA(bool flip) {
    int pos = 0;
    // Enumerate all of image data
    for (pos = 0; pos < rcv_image_data_size; pos++)
    {
        // Get current byte
        // int value = Buff__getByte(pos);
        int value = rcv_image_data[pos];

        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's 
        if (flip) {
            EPD_SendData(~(byte)value);
        } else {
            EPD_SendData((byte)value);
        }
    }
}

// Type G: each value represents 2 pixels
void EPDBt::EPD_loadCompressG(bool flip) {
    int pos = 0;
    // Enumerate all of image data
    for (pos = 0; pos < image_data_size; pos++)
    {
        // Get current byte
        // int value = Buff__getByte(pos);
        int value = image_data[pos];
		
        // Switch the positions of the two 4-bits pixels
        // Black:0b000;White:0b001;Green:0b010;Blue:0b011;Red:0b100;Yellow:0b101;Orange:0b110;
        int A = (value     ) & 0x07;
        int B = (value >> 4) & 0x07;
		
        // Write the data into e-Paper's memory
        EPD_SendData((byte)(A << 4) + B);
    }
}

void EPDBt::EPD_loadUncompressG(bool flip) {
    int pos = 0;
    // Enumerate all of image data
    for (pos = 0; pos < rcv_image_data_size; pos++)
    {
        // Get current byte
        // int value = Buff__getByte(pos);
        int value = rcv_image_data[pos];
		
        // Switch the positions of the two 4-bits pixels
        // Black:0b000;White:0b001;Green:0b010;Blue:0b011;Red:0b100;Yellow:0b101;Orange:0b110;
        int A = (value     ) & 0x07;
        int B = (value >> 4) & 0x07;
		
        // Write the data into e-Paper's memory
        EPD_SendData((byte)(A << 4) + B);
    }
}

void EPDBt::EPD_loadCompress(bool flip) {
    if (EPD_dispIndex == 43) {
        EPD_loadCompressG(flip);
    } else {
        EPD_loadCompressA(flip);
    }
}

void EPDBt::EPD_loadUncompress(bool flip) {
    if (EPD_dispIndex == 43) {
        EPD_loadUncompressG(flip);
    } else {
        EPD_loadUncompressA(flip);
    }
}

void EPDBt::Srvr__rcvProc() {
    memcpy(btInBuf.Buff__bufArr, Buff__bufArr, Buff__bufInd);
    btInBuf.data_len = Buff__bufInd;

    xQueueSend(btQueue, &btInBuf, portMAX_DELAY);

    return;
}