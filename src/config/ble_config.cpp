#include <Arduino.h>

#include <ArduinoJson.h>

#include "buff/buff.h"
#include "epd_driver/epd.h"

#include "config/wifi_config.h"
#include "config/ble_config.h"


BLEServer *pServer = NULL;
BLECharacteristic *pReadCharacteristic;

esp_bd_addr_t connectedAddress;  // destination device address

char json_buff[JSON_BUFF_SIZE];

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) {
      Serial.println("Try connecting..");

      // update connection params
      memcpy(connectedAddress, param->connect.remote_bda, sizeof(esp_bd_addr_t));

      Serial.printf("connId:%d, mtu=%d\n", param->mtu.conn_id, param->mtu.mtu);

      pServer->updateConnParams(connectedAddress,6,6,0,500);
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Disconnected..");
      pServer->getAdvertising()->start();
    }

    // // Maximum Transmission Unit allowed in GATT
    // #define ESP_GATT_MAX_MTU_SIZE       517  /* relate to GATT_MAX_MTU_SIZE in stack/gatt_api.h */
    void onMtuChanged(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) {
      Serial.printf(">>> onMtuChanged: connId=%d, mtu=%d\n", param->mtu.conn_id, param->mtu.mtu);
    }

};

class MsgCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");

        // receive data
        Buff__bufInd = 0;
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){
            Buff__bufArr[Buff__bufInd++] = rxValue[i];
            Serial.print(rxValue[i]);
        }

        Serial.println();
        Serial.println("*********");
        Serial.println();
      }
    }
};


class DataCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      uint8_t* rxValue = pCharacteristic->getData();
      size_t rxLength = pCharacteristic->getLength();

      if (rxLength > 0) {

        // receive data
        Buff__bufInd = 0;
        memset(Buff__bufArr, 0, Buff__SIZE);

        for (int i = 0; i < rxLength; i++){
            Buff__bufArr[Buff__bufInd++] = (byte)rxValue[i];
        }

        Srvr__rcvProc();
      }
    }
};


void my_ble_init() {
    Serial.println("1- Download and install an BLE scanner app in your phone");
    Serial.println("2- Scan for BLE devices in the app");
    Serial.println("3- Connect to MyESP32");
    Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
    Serial.println("5- See the magic =)");

    // init ble device
    BLEDevice::init("MyESP32");

    // create server for ble device
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // create service based on SERVICE_UUID
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pReadCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID_READ,
                                        BLECharacteristic::PROPERTY_READ
                                    );
    pReadCharacteristic->addDescriptor(new BLE2902());


    document["ip_address"] = get_local_ip();
    document["host_name"] = get_host_name();

    // serialize data and prepare to send
    serializeJson(document, json_buff);
    Serial.println(json_buff);

    pReadCharacteristic->setValue(json_buff);


    BLECharacteristic * pWriteCharacteristic = pService->createCharacteristic(
                                                CHARACTERISTIC_UUID_WRITE,
                                                BLECharacteristic::PROPERTY_WRITE
                                            );
    pWriteCharacteristic->setCallbacks(new MsgCallbacks());

    BLECharacteristic * pWriteDataCharacteristic = pService->createCharacteristic(
                                                CHARACTERISTIC_UUID_WRITE_DATA,
                                                BLECharacteristic::PROPERTY_WRITE
                                            );
    pWriteDataCharacteristic->setCallbacks(new DataCallbacks());

    // start service
    pService->start();

    pServer->getAdvertising()->start();

    Serial.println("Waiting a client connection to notify...");
    Serial.println();
}