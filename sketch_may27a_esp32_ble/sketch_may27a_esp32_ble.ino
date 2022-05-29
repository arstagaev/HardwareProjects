/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID_ESPOTA        "d804b643-6ce7-4e81-9f8a-ce0f699085eb"
#define CHARACTERISTIC_UUID_ID           "d804b644-6ce7-4e81-9f8a-ce0f699085eb"

#define SERVICE_UUID_OTA                    "c8659210-af91-4ad3-a995-a58d6fd26145" // UART service UUID
#define CHARACTERISTIC_UUID_FW              "c8659211-af91-4ad3-a995-a58d6fd26145"
#define CHARACTERISTIC_UUID_HW_VERSION      "c8659212-af91-4ad3-a995-a58d6fd26145"
#define SERVICE_UUID_ESPOTA        "d804b643-6ce7-4e81-9f8a-ce0f699085eb"
#define CHARACTERISTIC_UUID_ID           "d804b644-6ce7-4e81-9f8a-ce0f699085eb"

#define SERVICE_UUID_OTA                    "c8659210-af91-4ad3-a995-a58d6fd26145" // UART service UUID
#define CHARACTERISTIC_UUID_FW              "c8659211-af91-4ad3-a995-a58d6fd26145"
#define CHARACTERISTIC_UUID_HW_VERSION      "c8659212-af91-4ad3-a995-a58d6fd26145"
/////
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pOtaCharacteristic=NULL;
 
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  pOtaCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_FW,
                     BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE
                     );
pOtaCharacteristic->addDescriptor(new BLE2902());
pOtaCharacteristic->setCallbacks(new otaCallback(this));pOtaCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_FW,
                     BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE
                     );
pOtaCharacteristic->addDescriptor(new BLE2902());
//pOtaCharacteristic->setCallbacks(new otaCallback(this));

  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
void otaCallback::onWrite(BLECharacteristic *pCharacteristic)
{
  std::string rxData = pCharacteristic->getValue();
  if (!updateFlag) { //If it's the first packet of OTA since bootup, begin OTA
    Serial.println("BeginOTA");
    esp_ota_begin(esp_ota_get_next_update_partition(NULL), OTA_SIZE_UNKNOWN, &otaHandler);
    updateFlag = true;
  }
  if (_p_ble != NULL)
  {
    if (rxData.length() > 0)
    {
      esp_ota_write(otaHandler, rxData.c_str(), rxData.length());
      if (rxData.length() != FULL_PACKET)
      {
        esp_ota_end(otaHandler);
        Serial.println("EndOTA");
        if (ESP_OK == esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL))) {
          esp_restart();
        }
        else {
          Serial.println("Upload Error");
        }
      }
    }
  }

  uint8_t txData[5] = {1, 2, 3, 4, 5};
  //delay(1000);
  pCharacteristic->setValue((uint8_t*)txData, 5);
  pCharacteristic->notify();
}

void loop() {
  // put your main code here, to run repeatedly:
  String value = pCharacteristic->getValue();
  //Serial.print(“The new characteristic value is: “);
  Serial.println(value.c_str());
  if (pCharacteristic->getValue() = "1") {
    
    }
  delay(100);

}
