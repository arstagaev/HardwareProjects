/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#define LED 2
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define SERVICE_UUID2        "4fafc202-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID2 "beb54202-36e1-4688-b7f5-ea07361b26a8"
BLECharacteristic *pCharacteristic2;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool rqsNotify;
unsigned long prvMillis;

#define INTERVAL_READ 1000
int valNotify;
#define MAX_VALNOTIFY 255
uint8_t value = 0;

// Set your new MAC Address
uint8_t newMACAddress[] = {0x44, 0x44, 0x44, 0x44, 0x44, 0x0A};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.println(value[i]);

        Serial.println();
        Serial.println("*********");
      }
  
      if (value == "01") {
        digitalWrite(LED,HIGH);
        delay(100);
        digitalWrite (LED, LOW);  // turn off the LED
        delay(100); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        digitalWrite (LED,HIGH);  // turn off the LED
        delay(100);
        digitalWrite (LED, LOW);  // turn off the L
        delay(100);
        digitalWrite (LED,HIGH);  // turn off the LED
    } else {
          digitalWrite(LED,LOW);     
  }
}
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();

      rqsNotify = false;
      prvMillis = millis();
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;

      rqsNotify = false;
      Serial.println("Device disconnected");
    }
};
void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");
  
  esp_base_mac_addr_set(&newMACAddress[0]);
  
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  /////////////////

  // создаем BLE-сервис:
  BLEService *pService2 = pServer->createService(SERVICE_UUID2);
  // создаем BLE-характеристику:
 pCharacteristic2 = pService2->createCharacteristic(
 CHARACTERISTIC_UUID2,
 BLECharacteristic::PROPERTY_READ |
 BLECharacteristic::PROPERTY_WRITE |
 BLECharacteristic::PROPERTY_NOTIFY// |
 //BLECharacteristic::PROPERTY_INDICATE
 );
  // создаем BLE-дескриптор:
pCharacteristic2->addDescriptor(new BLE2902());

  // запускаем сервис:
pService2->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  // notify changed value
    if (deviceConnected) {
       digitalWrite (LED, HIGH);  // turn off the LED
        delay(1000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        digitalWrite (LED,LOW);  // turn off the LED

        pCharacteristic2->setValue(&value, 1);
        pCharacteristic2->notify();
        //pCharacteristic->indicate();
        value++;
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        digitalWrite (LED, LOW);  // turn off the LED
    }
  delay(1000);
}