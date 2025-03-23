#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include <ArduinoJson.h>

#include "config/wifi_config.h"
#include "config/ble_config.h"

#include "buff/buff_wifi.h"
#include "epd_driver/epd.h"

void setup()
{
  Serial.begin(115200);

  Serial.println("setup begin");

  // init epd buffer
  Buff__init();

  // init wifi
  my_wifi_init();

  // init ble
  my_ble_init();

  // init service
  Srvr__init();

  // Initialization is complete
  Serial.println("Ok!");
}


void loop()
{
  // service loop
  Srvr__loop();
}
