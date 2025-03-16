#ifndef ESP32_BLE_GAMEPAD_H
#define ESP32_BLE_GAMEPAD_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include "BleConnectionStatus.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"
#include "BleGamepadConfiguration.h"
#include "BleOutputReceiver.h"
#include "BleNUS.h"

uint8_t hidReportDescriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x00,        // Usage (Undefined, for VR tracker you might customize this)
  0xA1, 0x01,        // Collection (Application)
  
      // Buttons
      0x05, 0x09,    //   Usage Page (Button)
      0x19, 0x01,    //   Usage Minimum (Button 1)
      0x29, 0x08,    //   Usage Maximum (Button 8)
      0x15, 0x00,    //   Logical Minimum (0)
      0x25, 0x01,    //   Logical Maximum (1)
      0x95, 0x08,    //   Report Count (8)
      0x75, 0x01,    //   Report Size (1)
      0x81, 0x02,    //   Input (Data, Variable, Absolute) ; 8 buttons
      0x75, 0x08,    //   Report Size (8)
      0x95, 0x01,    //   Report Count (1)
      0x81, 0x03,    //   Input (Constant, Variable, Absolute) ; Padding
  
      // Position (X, Y, Z)
      0x05, 0x01,    //   Usage Page (Generic Desktop)
      0x09, 0x30,    //   Usage (X)
      0x09, 0x31,    //   Usage (Y)
      0x09, 0x32,    //   Usage (Z)
      0x16, 0x00, 0x00, // Logical Minimum (0)
      0x26, 0xFF, 0x7F, // Logical Maximum (32767)
      0x75, 0x10,    //   Report Size (16)
      0x95, 0x03,    //   Report Count (3)
      0x81, 0x02,    //   Input (Data, Variable, Absolute)
  
      // Orientation (Quaternion W, X, Y, Z)
      0x09, 0x33,    //   Usage (Rx)
      0x09, 0x34,    //   Usage (Ry)
      0x09, 0x35,    //   Usage (Rz)
      0x0A, 0x36, 0x00, // Usage (custom for W)
      0x16, 0x00, 0x00, // Logical Minimum (0)
      0x26, 0xFF, 0x7F, // Logical Maximum (32767)
      0x75, 0x10,    //   Report Size (16)
      0x95, 0x04,    //   Report Count (4)
      0x81, 0x02,    //   Input (Data, Variable, Absolute)
  
      // Battery Level
      0x05, 0x06,    //   Usage Page (Generic Device Controls)
      0x09, 0x20,    //   Usage (Battery Strength)
      0x15, 0x00,    //   Logical Minimum (0)
      0x26, 0x64, 0x00, // Logical Maximum (100)
      0x75, 0x08,    //   Report Size (8)
      0x95, 0x01,    //   Report Count (1)
      0x81, 0x02,    //   Input (Data, Variable, Absolute)
  
  0xC0               // End Collection
  
};

class BleGamepad
{
  private:
    std::string deviceManufacturer;
    std::string deviceName;
    uint8_t tempHidReportDescriptor[150];
    int hidReportDescriptorSize;
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
    BleGamepadConfiguration configuration;
    
    BleGamepad(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100, bool delayAdvertising = false);
    void begin(BleGamepadConfiguration *config = new BleGamepadConfiguration());
    void end(void);
    void setAxes(int16_t x = 0, int16_t y = 0, int16_t z = 0, int16_t rX = 0, int16_t rY = 0, int16_t rZ = 0, int16_t slider1 = 0, int16_t slider2 = 0);
    void setHIDAxes(int16_t x = 0, int16_t y = 0, int16_t z = 0, int16_t rZ = 0, int16_t rX = 0, int16_t rY = 0, int16_t slider1 = 0, int16_t slider2 = 0);
    void press(uint8_t b = BUTTON_1);   // press BUTTON_1 by default
    void release(uint8_t b = BUTTON_1); // release BUTTON_1 by default
    void pressSpecialButton(uint8_t b);
    void releaseSpecialButton(uint8_t b);
    void pressStart();
    void releaseStart();
    void pressSelect();
    void releaseSelect();
    void pressMenu();
    void releaseMenu();
    void pressHome();
    void releaseHome();
    void pressBack();
    void releaseBack();
    void pressVolumeInc();
    void releaseVolumeInc();
    void pressVolumeDec();
    void releaseVolumeDec();
    void pressVolumeMute();
    void releaseVolumeMute();
    void setLeftThumb(int16_t x = 0, int16_t y = 0);
    void setRightThumb(int16_t z = 0, int16_t rZ = 0);
    void setRightThumbAndroid(int16_t z = 0, int16_t rX = 0);
    void setLeftTrigger(int16_t rX = 0);
    void setRightTrigger(int16_t rY = 0);
    void setTriggers(int16_t rX = 0, int16_t rY = 0);
    void setHats(signed char hat1 = 0, signed char hat2 = 0, signed char hat3 = 0, signed char hat4 = 0);
    void setHat(signed char hat = 0);
    void setHat1(signed char hat1 = 0);
    void setHat2(signed char hat2 = 0);
    void setHat3(signed char hat3 = 0);
    void setHat4(signed char hat4 = 0);
    void setX(int16_t x = 0);
    void setY(int16_t y = 0);
    void setZ(int16_t z = 0);
    void setRZ(int16_t rZ = 0);
    void setRX(int16_t rX = 0);
    void setRY(int16_t rY = 0);
    void setSliders(int16_t slider1 = 0, int16_t slider2 = 0);
    void setSlider(int16_t slider = 0);
    void setSlider1(int16_t slider1 = 0);
    void setSlider2(int16_t slider2 = 0);
    void setRudder(int16_t rudder = 0);
    void setThrottle(int16_t throttle = 0);
    void setAccelerator(int16_t accelerator = 0);
    void setBrake(int16_t brake = 0);
    void setSteering(int16_t steering = 0);
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