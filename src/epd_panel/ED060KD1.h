
#include "FastEPD.h"

#define POWER_PIN GPIO_NUM_46

#define ED060KD1_WIDTH 1448
#define ED060KD1_HEIGHT 1072

FASTEPD epaper; // a static instance of the FastEPD class

bool panel_init_done = false;

int pixel_index = 0;

void Power_on() {
    digitalWrite(POWER_PIN, HIGH);
}

void Power_off() {
    digitalWrite(POWER_PIN, LOW);
}

int EPD_ED060KD1_init() {
    Serial.println("EPD_ED060KD1_init");
    int rc;

    pixel_index = 0;

    if (panel_init_done != true) {
        rc = epaper.initPanel(BB_PANEL_EPDIY_V7);
        if (rc != 0) {
            Serial.printf("init failed. rc=%d\n", rc);
            return rc;
        }
    
        rc = epaper.setPanelSize(ED060KD1_WIDTH, ED060KD1_HEIGHT);
        if (rc != 0) {
            Serial.printf("set panel size failed. rc=%d\n", rc);
            return rc;
        }

        panel_init_done = true;
    }

    rc = epaper.setMode(BB_MODE_4BPP); 
    if (rc != 0) {
        Serial.printf("set mode failed. rc=%d\n", rc);
        return rc;
    }

    Serial.println("clear white");
    // The default drawing mode is 1-bit per pixel
    epaper.clearWhite(true); // fill the current image buffer and eink panel with white

    Serial.println("init done.");

    return 0;
}

void EPD_ED060KD1_Load() {
    Serial.printf("EPD_ED060KD1_Load, %d\n", pixel_index);
    int pos = 0;

    uint8_t *s, *d, *pBuffer = epaper.currentBuffer();

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte
        int value = Buff__getByte(pos);
        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        for (int i = 0; i < 8; i++) {
            if (i % 2 == 0) {
                if (value & (128 >> i)) {
                    // pBuffer[pixel_index] &= 0x0f;
                } else {
                    pBuffer[pixel_index] &= 0x0f;
                }
            } else {
                if (value & (128 >> i)) {
                    // pBuffer[pixel_index] &= 0xf0;
                } else {
                    pBuffer[pixel_index] &= 0xf0;
                }
                pixel_index++;
            }
        }

        // Increment the current byte index on 2 characters
        pos += 2;
    }
}

static void EPD_ED060KD1_Show() {
    Serial.println("EPD_ED060KD1_Show");
    Serial.printf("pixel_index=%d\n", pixel_index);
    // Serial.println("full update");
    epaper.fullUpdate();
}

// // Graphics modes
// enum {
//     BB_MODE_NONE = 0,
//     BB_MODE_1BPP, // 1 bit per pixel
//     BB_MODE_4BPP, // 4 bits per pixel
// };
void EPD_ED060KD1_InitMode(int mode) {
    Serial.printf("EPD_ED060KD1_InitMode: mode=%d\n", mode);

    // power on
    pinMode(POWER_PIN, OUTPUT);
    Power_on();

}