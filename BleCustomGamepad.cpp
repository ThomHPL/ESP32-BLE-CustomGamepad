#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include "NimBLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include "sdkconfig.h"
#include "BleConnectionStatus.h"
#include "BleCustomGamepad.h"
#include "NimBLELog.h"
#include "BleCustomGamepadConfiguration.h"

#include <stdexcept>

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG "BleCustomGamepad"
#else
#include "esp_log.h"
static const char *LOG_TAG = "BleCustomGamepad";
#endif

#define SERVICE_UUID_DEVICE_INFORMATION         "180A"      // Service - Device information

#define CHARACTERISTIC_UUID_MODEL_NUMBER        "2A24"      // Characteristic - Model Number String - 0x2A24
#define CHARACTERISTIC_UUID_SOFTWARE_REVISION   "2A28"      // Characteristic - Software Revision String - 0x2A28
#define CHARACTERISTIC_UUID_SERIAL_NUMBER       "2A25"      // Characteristic - Serial Number String - 0x2A25
#define CHARACTERISTIC_UUID_FIRMWARE_REVISION   "2A26"      // Characteristic - Firmware Revision String - 0x2A26
#define CHARACTERISTIC_UUID_HARDWARE_REVISION   "2A27"      // Characteristic - Hardware Revision String - 0x2A27
#define CHARACTERISTIC_UUID_BATTERY_POWER_STATE "2A1A"      // Characteristic - Battery Power State - 0x2A1A

#define POWER_STATE_UNKNOWN         0 // 0b00
#define POWER_STATE_NOT_SUPPORTED   1 // 0b01
#define POWER_STATE_NOT_PRESENT     2 // 0b10
#define POWER_STATE_NOT_DISCHARGING 2 // 0b10
#define POWER_STATE_NOT_CHARGING    2 // 0b10
#define POWER_STATE_GOOD            2 // 0b10
#define POWER_STATE_PRESENT         3 // 0b11
#define POWER_STATE_DISCHARGING     3 // 0b11
#define POWER_STATE_CHARGING        3 // 0b11
#define POWER_STATE_CRITICAL        3 // 0b11

BleCustomGamepad::BleCustomGamepad(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel, bool delayAdvertising) : _batteryPowerInformation(0),
  _dischargingState(0),
  _chargingState(0),
  _powerLevel(0),
  hid(0),
  pCharacteristic_Power_State(0),
  configuration(),
  pServer(nullptr), 
  nus(nullptr)
{
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
  this->delayAdvertising = delayAdvertising;
  this->connectionStatus = new BleConnectionStatus();
  
  hidReportDescriptorSize = 0;
  hidReportSize = 0;
  numOfButtonBytes = 0;
  enableOutputReport = false;
  outputReportLength = 64;
  nusInitialized = false;
}

void BleCustomGamepad::setReportDescriptor(uint8_t *reportDescriptor, int reportDescriptorSize)
{
  tempHidReportDescriptor = reportDescriptor;
  hidReportDescriptorSize = reportDescriptorSize;
  
}

void BleCustomGamepad::setReport(void* report, int reportSize)
{
  hidReport = (uint8_t*) report;
  hidReportSize = reportSize;
}

void BleCustomGamepad::begin(BleCustomGamepadConfiguration *config)
{
  configuration = *config; // we make a copy, so the user can't change actual values midway through operation, without calling the begin function again

  // Set task priority from 5 to 1 in order to get ESP32-C3 working
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 1, NULL);
}

void BleCustomGamepad::end(void)
{
}

void BleCustomGamepad::sendReport(void)
{
  if (this->isConnected())
  {

    uint8_t m[hidReportSize];

    memcpy(&m, hidReport, hidReportSize);

  
    this->inputGamepad->setValue(m, sizeof(m));
    this->inputGamepad->notify();
    
    // // Testing
    // Serial.println("HID Report");
    // for (int i = 0; i < sizeof(m); i++)
    // {
    //    Serial.printf("%02x", m[i]);
    //    Serial.println();
    // }
  }
}


bool BleCustomGamepad::isConnected(void)
{
  return this->connectionStatus->connected;
}

void BleCustomGamepad::setBatteryLevel(uint8_t level)
{
  this->batteryLevel = level;
  if (hid != 0)
  {

    this->hid->setBatteryLevel(this->batteryLevel, this->isConnected() ? true : false);

    if (configuration.getAutoReport())
    {
      sendReport();
    }
  }
}

bool BleCustomGamepad::isOutputReceived()
{
  if (enableOutputReport && outputReceiver)
  {
    if (this->outputReceiver->outputFlag)
    {
      this->outputReceiver->outputFlag = false; // Clear Flag
      return true;
    }
  }
  return false;
}

uint8_t* BleCustomGamepad::getOutputBuffer()
{
  if (enableOutputReport && outputReceiver)
  {
    memcpy(outputBackupBuffer, outputReceiver->outputBuffer, outputReportLength); // Creating a backup to avoid buffer being overwritten while processing data
    return outputBackupBuffer;
  }
  return nullptr;
}

bool BleCustomGamepad::deleteAllBonds(bool resetBoard)
{
  bool success = false;

  NimBLEDevice::deleteAllBonds();
  NIMBLE_LOGD(LOG_TAG, "deleteAllBonds - All bonds deleted");
  success = true;
  delay(500);

  if (resetBoard)
  {
    NIMBLE_LOGD(LOG_TAG, "deleteAllBonds - Reboot ESP32");
    ESP.restart();
  }

  return success;	// Returns false if all bonds are not deleted
}

bool BleCustomGamepad::deleteBond(bool resetBoard)
{
  bool success = false;

  NimBLEServer* server = NimBLEDevice::getServer();

  if (server)
  {
    NimBLEConnInfo info = server->getPeerInfo(0);
    NimBLEAddress address = info.getAddress();

    success = NimBLEDevice::deleteBond(address);
    NIMBLE_LOGD(LOG_TAG, "deleteBond - Bond for %s deleted", std::string(address).c_str());

    delay(500);

    if (resetBoard)
    {
      NIMBLE_LOGD(LOG_TAG, "deleteBond - Reboot ESP32");
      ESP.restart();
    }
  }
  return success;	// Returns false if current bond is not deleted
}

bool BleCustomGamepad::enterPairingMode()
{
  NimBLEServer* server = NimBLEDevice::getServer();

  if (server)
  {
    NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Pairing mode entered");

    // Get current connection information and address
    NimBLEConnInfo currentConnInfo = server->getPeerInfo(0);
    NimBLEAddress currentAddress = currentConnInfo.getAddress();
    NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Connected Address: %s", std::string(currentAddress).c_str());

    // Disconnect from current connection
    for (uint16_t connHandle : server->getPeerDevices())
    {
      server->disconnect(connHandle); // Disconnect the client
      NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Disconnected from client");
      delay(500);
    }

    bool connectedToOldDevice = true;

    // While connected to old device, keep allowing to connect new new devices
    NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Advertising for clients...");
    while (connectedToOldDevice)
    {
      delay(10);	// Needs a delay to work - do not remove!

      if (this->isConnected())
      {
        NimBLEConnInfo newConnInfo = server->getPeerInfo(0);
        NimBLEAddress newAddress = newConnInfo.getAddress();

        // Block specific MAC address
        if (newAddress == currentAddress)
        {
          NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Connected to previous client, so disconnect and continue advertising for new client");
          server->disconnect(newConnInfo.getConnHandle());
          delay(500);
        }
        else
        {
          NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Connected to new client");
          NIMBLE_LOGD(LOG_TAG, "enterPairingMode - Exit pairing mode");
          connectedToOldDevice = false;
          return true;
        }
      }
    }
    return false; // Might want to adjust this function to stay in pairing mode for a while, and then return false after a while if no other device pairs with it
  }
  return false;
}

NimBLEAddress BleCustomGamepad::getAddress()
{
  NimBLEServer* server = NimBLEDevice::getServer();

  if (server)
  {
    // Get current connection information and address
    NimBLEConnInfo currentConnInfo = server->getPeerInfo(0);
    NimBLEAddress currentAddress = currentConnInfo.getAddress();
    return currentAddress;
  }
  NimBLEAddress blankAddress("00:00:00:00:00:00", 0);
  return blankAddress;
}

String BleCustomGamepad::getStringAddress()
{
  NimBLEServer* server = NimBLEDevice::getServer();

  if (server)
  {
    // Get current connection information and address
    NimBLEConnInfo currentConnInfo = server->getPeerInfo(0);
    NimBLEAddress currentAddress = currentConnInfo.getAddress();
    return currentAddress.toString().c_str();
  }
  NimBLEAddress blankAddress("00:00:00:00:00:00", 0);
  return blankAddress.toString().c_str();
}

NimBLEConnInfo BleCustomGamepad::getPeerInfo()
{
  NimBLEServer* server = NimBLEDevice::getServer();
  NimBLEConnInfo currentConnInfo = server->getPeerInfo(0);
  return currentConnInfo;
}

String BleCustomGamepad::getDeviceName()
{
  return this->deviceName.c_str();
}

String BleCustomGamepad::getDeviceManufacturer()
{
  return this->deviceManufacturer.c_str();
}

int8_t BleCustomGamepad::getTXPowerLevel()
{
  return NimBLEDevice::getPower();
}

void BleCustomGamepad::setTXPowerLevel(int8_t level)
{
  NimBLEDevice::setPower(level);  // The only valid values are: -12, -9, -6, -3, 0, 3, 6 and 9
  configuration.setTXPowerLevel(level);
}

void BleCustomGamepad::setGyroscope(int16_t gX, int16_t gY, int16_t gZ)
{
  if (gX == -32768)
  {
    gX = -32767;
  }
  
  if (gY == -32768)
  {
    gY = -32767;
  }
  
  if (gY == -32768)
  {
    gY = -32767;
  }
  
  _gX = gX;
  _gY = gY;
  _gZ = gZ; 
  
  if (configuration.getAutoReport())
  {
    sendReport();
  }
}

    
void BleCustomGamepad::setPowerStateAll(uint8_t batteryPowerInformation, uint8_t dischargingState, uint8_t chargingState, uint8_t powerLevel)
{
    uint8_t powerStateBits = 0b00000000;
    
    _batteryPowerInformation = batteryPowerInformation;
    _dischargingState = dischargingState;
    _chargingState = chargingState;
    _powerLevel = powerLevel;

    // HID Battery Power State Bits:
    // Bits 0 and 1: Battery Power Information : 0(0b00) = Unknown, 1(0b01) = Not Supported,  2(0b10) = Not Present,               3(0b11) = Present
    // Bits 2 and 3: Discharging State         : 0(0b00) = Unknown, 1(0b01) = Not Supported,  2(0b10) = Not Discharging,           3(0b11) = Discharging
    // Bits 4 and 5: Charging State            : 0(0b00) = Unknown, 1(0b01) = Not Chargeable, 2(0b10) = Not Charging (Chargeable), 3(0b11) = Charging (Chargeable)
    // Bits 6 and 7: Power Level               : 0(0b00) = Unknown, 1(0b01) = Not Supported,  2(0b10) = Good Level,                3(0b11) = Critically Low Level

    powerStateBits |= (_batteryPowerInformation << 0);  // Populate first 2 bits with data
    powerStateBits |= (_dischargingState        << 2);  // Populate second 2 bits with data
    powerStateBits |= (_chargingState           << 4);  // Populate third 2 bits with data
    powerStateBits |= (_powerLevel              << 6);  // Populate last 2 bits with data

    if (this->pCharacteristic_Power_State) 
    {
      this->pCharacteristic_Power_State->setValue(&powerStateBits, 1);
      this->pCharacteristic_Power_State->notify();
    }
}


void BleCustomGamepad::setBatteryPowerInformation(uint8_t batteryPowerInformation)
{
  _batteryPowerInformation = batteryPowerInformation;
  setPowerStateAll(_batteryPowerInformation, _dischargingState, _chargingState, _powerLevel);
}

void BleCustomGamepad::setDischargingState(uint8_t dischargingState)
{
  _dischargingState = dischargingState;
  setPowerStateAll(_batteryPowerInformation, _dischargingState, _chargingState, _powerLevel);
}

void BleCustomGamepad::setChargingState(uint8_t chargingState)
{
  _chargingState = chargingState;
  setPowerStateAll(_batteryPowerInformation, _dischargingState, _chargingState, _powerLevel);
}

void BleCustomGamepad::setPowerLevel(uint8_t powerLevel)
{
  _powerLevel = powerLevel;
  setPowerStateAll(_batteryPowerInformation, _dischargingState, _chargingState, _powerLevel);
}

void BleCustomGamepad::beginNUS() 
{
    if (!this->nusInitialized) 
    {
        // Extrememly important to make sure that the pointer to server is actually valid
        while(!NimBLEDevice::isInitialized ()){}        // Wait until the server is initialized
        while(NimBLEDevice::getServer() == nullptr){}   // Ensure pointer to server is actually valid
        
        // Now server is nkown to be valid, initialise nus to new BleNUS instance
        nus = new BleNUS(NimBLEDevice::getServer()); // Pass the existing BLE server
        nus->begin();
        nusInitialized = true;
    }
}

BleNUS* BleCustomGamepad::getNUS() 
{
    return nus;  // Return a pointer instead of a reference
}

void BleCustomGamepad::sendDataOverNUS(const uint8_t* data, size_t length) 
{
  if (nus) 
  {
    nus->sendData(data, length);
  }
}

void BleCustomGamepad::setNUSDataReceivedCallback(void (*callback)(const uint8_t* data, size_t length)) 
{
  if (nus) 
  {
    nus->setDataReceivedCallback(callback);
  }
}

void BleCustomGamepad::taskServer(void *pvParameter)
{
  BleCustomGamepad *BleCustomGamepadInstance = (BleCustomGamepad *)pvParameter; // static_cast<BleCustomGamepad *>(pvParameter);

  NimBLEDevice::init(BleCustomGamepadInstance->deviceName);
  NimBLEDevice::setPower(BleCustomGamepadInstance->configuration.getTXPowerLevel()); // Set transmit power for advertising (Range: -12 to +9 dBm)
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(BleCustomGamepadInstance->connectionStatus);
  pServer->advertiseOnDisconnect(true);

  BleCustomGamepadInstance->hid = new NimBLEHIDDevice(pServer);

  BleCustomGamepadInstance->inputGamepad = BleCustomGamepadInstance->hid->getInputReport(BleCustomGamepadInstance->configuration.getHidReportId()); // <-- input REPORTID from report map
  BleCustomGamepadInstance->connectionStatus->inputGamepad = BleCustomGamepadInstance->inputGamepad;

  if (BleCustomGamepadInstance->enableOutputReport) 
  {
    BleCustomGamepadInstance->outputGamepad = BleCustomGamepadInstance->hid->getOutputReport(BleCustomGamepadInstance->configuration.getHidReportId());
    BleCustomGamepadInstance->outputReceiver = new BleOutputReceiver(BleCustomGamepadInstance->outputReportLength);
    BleCustomGamepadInstance->outputBackupBuffer = new uint8_t[BleCustomGamepadInstance->outputReportLength];
    BleCustomGamepadInstance->outputGamepad->setCallbacks(BleCustomGamepadInstance->outputReceiver);
  }

  BleCustomGamepadInstance->hid->setManufacturer(BleCustomGamepadInstance->deviceManufacturer);

  NimBLEService *pService = pServer->getServiceByUUID(SERVICE_UUID_DEVICE_INFORMATION);

  BLECharacteristic* pCharacteristic_Model_Number = pService->createCharacteristic(
        CHARACTERISTIC_UUID_MODEL_NUMBER,
        NIMBLE_PROPERTY::READ
      );
  pCharacteristic_Model_Number->setValue(std::string(BleCustomGamepadInstance->configuration.getModelNumber()));

  BLECharacteristic* pCharacteristic_Software_Revision = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SOFTWARE_REVISION,
        NIMBLE_PROPERTY::READ
      );
  pCharacteristic_Software_Revision->setValue(std::string(BleCustomGamepadInstance->configuration.getSoftwareRevision()));

  BLECharacteristic* pCharacteristic_Serial_Number = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SERIAL_NUMBER,
        NIMBLE_PROPERTY::READ
      );
  pCharacteristic_Serial_Number->setValue(std::string(BleCustomGamepadInstance->configuration.getSerialNumber()));

  BLECharacteristic* pCharacteristic_Firmware_Revision = pService->createCharacteristic(
        CHARACTERISTIC_UUID_FIRMWARE_REVISION,
        NIMBLE_PROPERTY::READ
      );
  pCharacteristic_Firmware_Revision->setValue(std::string(BleCustomGamepadInstance->configuration.getFirmwareRevision()));

  BLECharacteristic* pCharacteristic_Hardware_Revision = pService->createCharacteristic(
        CHARACTERISTIC_UUID_HARDWARE_REVISION,
        NIMBLE_PROPERTY::READ
      );
  pCharacteristic_Hardware_Revision->setValue(std::string(BleCustomGamepadInstance->configuration.getHardwareRevision()));
  
  NimBLECharacteristic* pCharacteristic_Power_State = BleCustomGamepadInstance->hid->getBatteryService()->createCharacteristic(
        CHARACTERISTIC_UUID_BATTERY_POWER_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
      );
  BleCustomGamepadInstance->pCharacteristic_Power_State = pCharacteristic_Power_State; // Assign the created characteristic
  BleCustomGamepadInstance->pCharacteristic_Power_State->setValue(0b00000000); // Now it's safe to call setValue <- Set all to unknown by default

  BleCustomGamepadInstance->hid->setPnp(0x01, BleCustomGamepadInstance->configuration.getVid(), BleCustomGamepadInstance->configuration.getPid(), BleCustomGamepadInstance->configuration.getGuidVersion());
  BleCustomGamepadInstance->hid->setHidInfo(0x00, 0x01);

  // NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);
  NimBLEDevice::setSecurityAuth(true, false, false); // enable bonding, no MITM, no SC


  uint8_t customHidReportDescriptor[BleCustomGamepadInstance->hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, BleCustomGamepadInstance->tempHidReportDescriptor, BleCustomGamepadInstance->hidReportDescriptorSize);

  // // Testing - Ask ChatGPT to convert it into a commented HID descriptor
  // Serial.println("------- HID DESCRIPTOR START -------");
  // for (int i = 0; i < BleCustomGamepadInstance->hidReportDescriptorSize; i++)
  // {
  //    Serial.printf("%02x", customHidReportDescriptor[i]);
  //    Serial.println();
  // }
  // Serial.println("------- HID DESCRIPTOR END -------");
  
  BleCustomGamepadInstance->hid->setReportMap((uint8_t *)customHidReportDescriptor, BleCustomGamepadInstance->hidReportDescriptorSize);
  BleCustomGamepadInstance->hid->startServices();

  BleCustomGamepadInstance->onStarted(pServer);

  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->setName(BleCustomGamepadInstance->deviceName);
  pAdvertising->addServiceUUID(BleCustomGamepadInstance->hid->getHidService()->getUUID());
  
  if(BleCustomGamepadInstance->delayAdvertising)
  {
    NIMBLE_LOGD(LOG_TAG, "Main NimBLE server advertising delayed (until Nordic UART Service added)");
  }
  else
  {
    NIMBLE_LOGD(LOG_TAG, "Main NimBLE server advertising started!");
    pAdvertising->start();
  }
  
  BleCustomGamepadInstance->hid->setBatteryLevel(BleCustomGamepadInstance->batteryLevel);

  vTaskDelay(portMAX_DELAY); // delay(portMAX_DELAY);
}