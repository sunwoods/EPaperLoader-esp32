#ifndef __BLE_CONFIG_H
#define __BLE_CONFIG_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// define service UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// notify
#define CHARACTERISTIC_UUID_NOTIFY "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// read
#define CHARACTERISTIC_UUID_READ "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
// write_msg
#define CHARACTERISTIC_UUID_WRITE "6E400004-B5A3-F393-E0A9-E50E24DCCA9E"
// write_data
#define CHARACTERISTIC_UUID_WRITE_DATA "6E400005-B5A3-F393-E0A9-E50E24DCCA9E"

void my_ble_init();


extern esp_bd_addr_t connectedAddress;  // destination device addresss

#endif