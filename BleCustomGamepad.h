#ifndef ESP32_BLE_GAMEPAD_H
#define ESP32_BLE_GAMEPAD_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include "BleConnectionStatus.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"
#include "BleCustomGamepadConfiguration.h"
#include "BleOutputReceiver.h"
#include "BleNUS.h"

class BleCustomGamepad
{
  private:
    std::string deviceManufacturer;
    std::string deviceName;
    uint8_t* tempHidReportDescriptor;
    int hidReportDescriptorSize;
    void* hidReport;
    uint8_t hidReportSize;
    uint8_t numOfButtonBytes;
    bool enableOutputReport;
    uint16_t outputReportLength;
    uint8_t _buttons[16]; // 8 bits x 16 bytes = 128 bits --> 128 button max
    uint8_t _specialButtons;
    int16_t _x;
    int16_t _y;
    int16_t _z;
    int16_t _rX;
    int16_t _rY;
    int16_t _rZ;
    int16_t _slider1;
    int16_t _slider2;
    int16_t _rudder;
    int16_t _throttle;
    int16_t _accelerator;
    int16_t _brake;
    int16_t _steering;
    int16_t _hat1;
    int16_t _hat2;
    int16_t _hat3;
    int16_t _hat4;
    int16_t _gX;
    int16_t _gY;
    int16_t _gZ;
    int16_t _aX;
    int16_t _aY;
    int16_t _aZ;
    uint8_t _batteryPowerInformation;
    uint8_t _dischargingState;
    uint8_t _chargingState;
    uint8_t _powerLevel;
    bool nusInitialized;
    
    BleConnectionStatus *connectionStatus;
    BleOutputReceiver *outputReceiver;
    NimBLEServer *pServer;
    BleNUS* nus;
    
    NimBLEHIDDevice *hid;
    NimBLECharacteristic *inputGamepad;
    NimBLECharacteristic *outputGamepad;
    NimBLECharacteristic *pCharacteristic_Power_State;

    uint8_t *outputBackupBuffer;

    void rawAction(uint8_t msg[], char msgSize);
    static void taskServer(void *pvParameter);
    uint8_t specialButtonBitPosition(uint8_t specialButton);

  public:
    BleCustomGamepadConfiguration configuration;
    
    BleCustomGamepad(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100, bool delayAdvertising = false);
    void begin(BleCustomGamepadConfiguration *config = new BleCustomGamepadConfiguration());
    void end(void);
    void setReportDescriptor(uint8_t *reportDescriptor, int reportDescriptorSize);
    void setReport(void* report, int reportSize);
    void setX(int16_t x = 0);
    void setY(int16_t y = 0);
    void setZ(int16_t z = 0);
    void setRZ(int16_t rZ = 0);
    void setRX(int16_t rX = 0);
    void setRY(int16_t rY = 0);
    void setRW(int16_t rW = 0);
    void setSimulationControls(int16_t rudder = 0, int16_t throttle = 0, int16_t accelerator = 0, int16_t brake = 0, int16_t steering = 0);
    void sendReport();
    bool isPressed(uint8_t b = BUTTON_1); // check BUTTON_1 by default
    bool isConnected(void);
    void resetButtons();
    void setBatteryLevel(uint8_t level);
    void setPowerStateAll(uint8_t batteryPowerInformation, uint8_t dischargingState, uint8_t chargingState, uint8_t powerLevel);
    void setBatteryPowerInformation(uint8_t batteryPowerInformation);
    void setDischargingState(uint8_t dischargingState);
    void setChargingState(uint8_t chargingState);
    void setPowerLevel(uint8_t powerLevel);
    void setTXPowerLevel(int8_t level = 9);
    int8_t getTXPowerLevel();
    uint8_t batteryLevel;
    bool delayAdvertising;
    bool isOutputReceived();
    uint8_t* getOutputBuffer();
    bool deleteBond(bool resetBoard = false);
    bool deleteAllBonds(bool resetBoard = false);
    bool enterPairingMode();
    NimBLEAddress getAddress();
    String getStringAddress();
    NimBLEConnInfo getPeerInfo();
    String getDeviceName();
    String getDeviceManufacturer();
    void setGyroscope(int16_t gX = 0, int16_t gY = 0, int16_t gZ = 0);
    void setAccelerometer(int16_t aX = 0, int16_t aY = 0, int16_t aZ = 0);
    void setMotionControls(int16_t gX = 0, int16_t gY = 0, int16_t gZ = 0, int16_t aX = 0, int16_t aY = 0, int16_t aZ = 0);
    void beginNUS();
    void sendDataOverNUS(const uint8_t* data, size_t length);
    void setNUSDataReceivedCallback(void (*callback)(const uint8_t* data, size_t length));
    BleNUS* getNUS();

  protected:
    virtual void onStarted(NimBLEServer *pServer) {};
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_GAMEPAD_H