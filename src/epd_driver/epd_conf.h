#ifndef EPD_CONF_H_
#define EPD_CONF_H_

#include <Arduino.h>

/* SPI pin definition --------------------------------------------------------*/
#define DEFAULT_PIN_SPI_SCK  13
#define DEFAULT_PIN_SPI_DIN  14
#define DEFAULT_PIN_SPI_CS   15
#define DEFAULT_PIN_SPI_BUSY 25//19
#define DEFAULT_PIN_SPI_RST  26//21
#define DEFAULT_PIN_SPI_DC   27//22

/* Pin level definition ------------------------------------------------------*/
#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define LOAD_MODE_BT 1
#define LOAD_MODE_WIFI 2

/* The set of pointers on 'init', 'load' and 'show' functions, title and code */
struct EPD_dispInfo
{
    int(*init)(); // Initialization
    void(*chBk)();// Black channel loading
    int next;     // Change channel code
    void(*chRd)();// Red channel loading
    void(*show)();// Show and sleep
    char*title;   // Title of an e-Paper
};

extern int loadMode;

extern bool EPD_invert;           // If true, then image data bits must be inverted
extern int  EPD_dispIndex;        // The index of the e-Paper's type
extern int  EPD_dispX, EPD_dispY; // Current pixel's coordinates (for 2.13 only)
extern void(*EPD_dispLoad)();     // Pointer on a image data writting function

extern EPD_dispInfo EPD_dispMass[];

extern bool is_compress;
extern uint8_t *rcv_image_data;
extern uint32_t rcv_image_data_size;
extern uint8_t *image_data;
extern uint32_t image_data_size;
extern bool load_stage;
extern bool next_stage;

extern int PIN_SPI_SCK;
extern int PIN_SPI_DIN;
extern int PIN_SPI_CS;
extern int PIN_SPI_BUSY;
extern int PIN_SPI_RST;
extern int PIN_SPI_DC;

// /* Lut mono ------------------------------------------------------------------*/
extern byte lut_full_mono[];
extern byte lut_partial_mono[];

extern byte LUT_DATA[];
extern byte LUT_DATA_part[];

extern byte lut_vcom0[];
extern byte lut_w    [];
extern byte lut_b    [];
extern byte lut_g1   [];
extern byte lut_g2   [];
extern byte lut_vcom1[];
extern byte lut_red0 [];
extern byte lut_red1 [];

void decompress_image_data();

#endif