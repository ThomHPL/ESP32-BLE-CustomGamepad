/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that can be defined with an HID descriptor
 */

 #include <Arduino.h>
 #include <BleCustomGamepad.h>
 
 BleCustomGamepad bleGamepad;
 
 uint8_t defaultHIDReportDescriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        // REPORT_ID (Default: 3)

    0x09, 0x01, // Usage ( Pointer )
    0xA1, 0x00, // Collection ( Physical )
      0x05, 0x09, // Usage Page ( Buttons )
      0x19, 0x01, // Usage Minimum ( Button 1)
      0x29, 0x02, // Usage Maximum ( Button 4)
      0x15, 0x00, // Logical Minimum (0)
      0x25, 0x01, // Logical Maximum (1)
      0x95, 0x03, // Report Count (3)
      0x75, 0x01, // Report Size (1)
      0x81, 0x02, // Input (Data , Absolute , Variable )
      // ---------- Padding ----------
      0x95, 0x01, // Report Count (1)
      0x75, 0x05, // Report Size (5)
      0x81, 0x03, // Input ( Absolute , Constant , Variable )
      // -------- X, Y, Wheel --------
      0x05, 0x01, // Usage page ( desktop )
      0x09, 0x30, // Usage (x)
      0x09, 0x31, // Usage (y)
      0x09, 0x38, // Usage (wheel)
      0x15, 0x81, // Logical min ( -127)
      0x25, 0x7F, // Logical max (127)
      0x75, 0x08, // Report size (8)
      0x95, 0x03, // Report count (3)
      0x81, 0x06, // Input (Data , Relative , Variable )
    // -----------------------------
    0xC0, // End Collection  

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
  
 
 typedef struct __attribute__ ((packed)) {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel;
    uint8_t battery;
 } HIDMouseReport;
 
 HIDMouseReport hidReport = {
   0, // buttons
   0, // x
   0, // y
   0, // wheel
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
         hidReport.x = 10;
         hidReport.y = 10;       
         bleGamepad.sendReport();
         delay(1000);

         Serial.println("Release");
         hidReport.buttons = 0b00000000;
         hidReport.x = -10;
         hidReport.y = -10;
         bleGamepad.sendReport();
         delay(1000);
     }
 }
 