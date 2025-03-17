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

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    bleGamepad = new BleCustomGamepad("ESP32 BLE Custom Gamepad", "Espressif", 100, false);
    bleGamepad.begin();
    // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons
}

void loop()
{
    if (bleGamepad.isConnected())
    {
        Serial.println("Press buttons 5, 16 and start. Move all enabled axes to max. Set DPAD (hat 1) to down right.");
        bleGamepad.press(BUTTON_5);
        bleGamepad.press(BUTTON_16);
        bleGamepad.pressStart();
        bleGamepad.setAxes(32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767);       //(X, Y, Z, RX, RY, RZ)
        //bleGamepad.setHIDAxes(32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767);  //(X, Y, Z, RZ, RX, RY)
        bleGamepad.setHat1(HAT_DOWN_RIGHT);
        // All axes, sliders, hats etc can also be set independently. See the IndividualAxes.ino example
        delay(500);

        Serial.println("Release button 5 and start. Move all axes to min. Set DPAD (hat 1) to centred.");
        bleGamepad.release(BUTTON_5);
        bleGamepad.releaseStart();
        bleGamepad.setHat1(HAT_CENTERED);
        bleGamepad.setAxes(0, 0, 0, 0, 0, 0, 0, 0);           //(X, Y, Z, RX, RY, RZ)
        //bleGamepad.setHIDAxes(0, 0, 0, 0, 0, 0, 0, 0);      //(X, Y, Z, RZ, RX, RY)
        delay(500);
    }
}
