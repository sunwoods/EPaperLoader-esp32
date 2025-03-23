
 #ifndef WIFI_BT_H_
 #define WIFI_BT_H_

#include "Arduino.h"
#include "buff.h"

class BUFFBt {
public:
  /* Reads a word from the buffer at specified position ------------------------*/
  static int Buff__getByte(int index);

  /* Reads a byte from the buffer at specified position ------------------------*/
  static int Buff__getWord(int index);

  /* Reads a byte from the buffer at specified position ------------------------*/
  static int Buff__getN3(int index);

  /* Checks if the buffer's data ends with specified string --------------------*/
  static int Buff__signature(int index, char*str);
}; // class BUFFBt

extern BUFFBt myBUFFBt;
#endif