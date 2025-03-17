/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 *
 * At the moment we are using the default settings, but they can be canged using a BleGamepadConfig instance as parameter for the begin function.
 *
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_16
 * (16 buttons by default. Library can be configured to use up to 128)
 *
 * Possible DPAD/HAT switch position values are:
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes sets all axes at once. There are a few:
 * (x axis, y axis, z axis, rx axis, ry axis, rz axis, slider 1, slider 2)
 *
 * Alternatively, bleGamepad.setHIDAxes sets all axes at once. in the order of:
 * (x axis, y axis, z axis, rZ axis, rX axis, rY axis, slider 1, slider 2)  <- order HID report is actually given in
 *
 * Library can also be configured to support up to 5 simulation controls
 * (rudder, throttle, accelerator, brake, steering), but they are not enabled by default.
 *
 * Library can also be configured to support different function buttons
 * (start, select, menu, home, back, volume increase, volume decrease, volume mute)
 * start and select are enabled by default
 */

 #include <Arduino.h>
 #include <BleCustomGamepad.h>
 
 BleCustomGamepad bleGamepad;
 
 uint8_t defaultHIDReportDescriptor[] = {
     0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
     0x09, 0x05,                    // USAGE (Game Pad)
     0xa1, 0x01,                    // COLLECTION (Application)
     0xa1, 0x00,                    //   COLLECTION (Physical)
     // ReportID - 8 bits
     0x85, 0x03,                    //     REPORT_ID (3)
     // X & Y - 2x8 = 16 bits
     0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
     0x09, 0x30,                    //     USAGE (X)
     0x09, 0x31,                    //     USAGE (Y)
     0x15, 0x80,                    //     LOGICAL_MINIMUM (-128)
     0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
     0x75, 0x08,                    //     REPORT_SIZE (8)
     0x95, 0x02,                    //     REPORT_COUNT (2)
     0x81, 0x02,                    //     INPUT (Data,Var,Abs)
     // Buttons - 8 bits
     0x05, 0x09,                    //     USAGE_PAGE (Button)
     0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
     0x29, 0x08,                    //     USAGE_MAXIMUM (Button 8)
     0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
     0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
     0x75, 0x08,                    //     REPORT_SIZE (8)
     0x95, 0x01,                    //     REPORT_COUNT (1)
     0x81, 0x02,                    //     INPUT (Data,Var,Abs)
     0xc0,                          //     END_COLLECTION
     0xc0                           // END_COLLECTION
     
   };
 
   typedef struct __attribute__ ((packed)) {
     int8_t x;
     int8_t y;
     uint8_t buttons;
     uint8_t battery;
  } HIDGamepadReport;
  
  HIDGamepadReport hidReport = {
    0, // x
    0, // y
    0, // buttons
    100 // battery
  };
  
  void setup()
  {
      Serial.begin(115200);
      Serial.println("Starting BLE work!");
      bleGamepad = BleCustomGamepad("ESP32 BLE Custom Device", "Espressif", 100, false);
      bleGamepad.setReportDescriptor(defaultHIDReportDescriptor, sizeof(defaultHIDReportDescriptor));
      bleGamepad.setReport(&hidReport, sizeof(HIDMouseReport));
      bleGamepad.begin();
  }
  
  void loop()
  {
      if (bleGamepad.isConnected())
      {
          Serial.println("Press");
          hidReport.buttons = 0b00000001;  
          hidReport.x = 60;
          hidReport.y = 60;       
          bleGamepad.sendReport();
          delay(1000);
 
          Serial.println("Release");
          hidReport.buttons = 0b00000000;
          hidReport.x = -60;
          hidReport.y = -60;
          bleGamepad.sendReport();
          delay(1000);
      }
  }
  