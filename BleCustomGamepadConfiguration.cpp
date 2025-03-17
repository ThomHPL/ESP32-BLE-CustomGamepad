#include "BleCustomGamepadConfiguration.h"

BleCustomGamepadConfiguration::BleCustomGamepadConfiguration() : _controllerType(CONTROLLER_TYPE_GAMEPAD),
                                                     _autoReport(true),
                                                     _hidReportId(3),
                                                     _buttonCount(16),
                                                     _hatSwitchCount(1),
                                                     _whichSpecialButtons{false, false, false, false, false, false, false, false},
                                                     _whichAxes{true, true, true, true, true, true, true, true},
                                                     _whichSimulationControls{false, false, false, false, false},
                                                     _includeGyroscope(false),
                                                     _includeAccelerometer(false),
                                                     _vid(0xe502),
                                                     _pid(0xbbab),
                                                     _guidVersion(0x0110),
                                                     _axesMin(0x0000),
                                                     _axesMax(0x7FFF),
                                                     _simulationMin(0x0000),
                                                     _simulationMax(0x7FFF),
                                                     _motionMin(0x0000),
                                                     _motionMax(0x7FFF),
                                                     _modelNumber("1.0.0"),
                                                     _softwareRevision("1.0.0"),
                                                     _serialNumber("0123456789"),
                                                     _firmwareRevision("0.7.4"),
                                                     _hardwareRevision("1.0.0"),
                                                     _enableOutputReport(false),
                                                     _enableNordicUARTService(false),
                                                     _outputReportLength(64),
													                           _transmitPowerLevel(9)
{
}

uint8_t BleCustomGamepadConfiguration::getTotalSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESPECIALBUTTONS; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleCustomGamepadConfiguration::getDesktopSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < 3; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleCustomGamepadConfiguration::getConsumerSpecialButtonCount()
{
    int count = 0;
    for (int i = 3; i < 8; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleCustomGamepadConfiguration::getAxisCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLEAXES; i++)
    {
        count += (int)_whichAxes[i];
    }

    return count;
}

uint8_t BleCustomGamepadConfiguration::getSimulationCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESIMULATIONCONTROLS; i++)
    {
        count += (int)_whichSimulationControls[i];
    }

    return count;
}

uint16_t BleCustomGamepadConfiguration::getVid(){ return _vid; }
uint16_t BleCustomGamepadConfiguration::getPid(){ return _pid; }
uint16_t BleCustomGamepadConfiguration::getGuidVersion(){ return _guidVersion; }
int16_t BleCustomGamepadConfiguration::getAxesMin(){ return _axesMin; }
int16_t BleCustomGamepadConfiguration::getAxesMax(){ return _axesMax; }
int16_t BleCustomGamepadConfiguration::getSimulationMin(){ return _simulationMin; }
int16_t BleCustomGamepadConfiguration::getSimulationMax(){ return _simulationMax; }
int16_t BleCustomGamepadConfiguration::getMotionMin(){ return _motionMin; }
int16_t BleCustomGamepadConfiguration::getMotionMax(){ return _motionMax; }
uint8_t BleCustomGamepadConfiguration::getControllerType() { return _controllerType; }
uint8_t BleCustomGamepadConfiguration::getHidReportId() { return _hidReportId; }
uint16_t BleCustomGamepadConfiguration::getButtonCount() { return _buttonCount; }
uint8_t BleCustomGamepadConfiguration::getHatSwitchCount() { return _hatSwitchCount; }
bool BleCustomGamepadConfiguration::getAutoReport() { return _autoReport; }
bool BleCustomGamepadConfiguration::getIncludeStart() { return _whichSpecialButtons[START_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeSelect() { return _whichSpecialButtons[SELECT_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeMenu() { return _whichSpecialButtons[MENU_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeHome() { return _whichSpecialButtons[HOME_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeBack() { return _whichSpecialButtons[BACK_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeVolumeInc() { return _whichSpecialButtons[VOLUME_INC_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeVolumeDec() { return _whichSpecialButtons[VOLUME_DEC_BUTTON]; }
bool BleCustomGamepadConfiguration::getIncludeVolumeMute() { return _whichSpecialButtons[VOLUME_MUTE_BUTTON]; }
const bool *BleCustomGamepadConfiguration::getWhichSpecialButtons() const { return _whichSpecialButtons; }
bool BleCustomGamepadConfiguration::getIncludeXAxis() { return _whichAxes[X_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeYAxis() { return _whichAxes[Y_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeZAxis() { return _whichAxes[Z_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeRxAxis() { return _whichAxes[RX_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeRyAxis() { return _whichAxes[RY_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeRzAxis() { return _whichAxes[RZ_AXIS]; }
bool BleCustomGamepadConfiguration::getIncludeSlider1() { return _whichAxes[SLIDER1]; }
bool BleCustomGamepadConfiguration::getIncludeSlider2() { return _whichAxes[SLIDER2]; }
const bool *BleCustomGamepadConfiguration::getWhichAxes() const { return _whichAxes; }
bool BleCustomGamepadConfiguration::getIncludeRudder() { return _whichSimulationControls[RUDDER]; }
bool BleCustomGamepadConfiguration::getIncludeThrottle() { return _whichSimulationControls[THROTTLE]; }
bool BleCustomGamepadConfiguration::getIncludeAccelerator() { return _whichSimulationControls[ACCELERATOR]; }
bool BleCustomGamepadConfiguration::getIncludeBrake() { return _whichSimulationControls[BRAKE]; }
bool BleCustomGamepadConfiguration::getIncludeSteering() { return _whichSimulationControls[STEERING]; }
const bool *BleCustomGamepadConfiguration::getWhichSimulationControls() const { return _whichSimulationControls; }
bool BleCustomGamepadConfiguration::getIncludeGyroscope() { return _includeGyroscope; }
bool BleCustomGamepadConfiguration::getIncludeAccelerometer() { return _includeAccelerometer; }
char *BleCustomGamepadConfiguration::getModelNumber(){ return _modelNumber; }
char *BleCustomGamepadConfiguration::getSoftwareRevision(){ return _softwareRevision; }
char *BleCustomGamepadConfiguration::getSerialNumber(){ return _serialNumber; }
char *BleCustomGamepadConfiguration::getFirmwareRevision(){ return _firmwareRevision; }
char *BleCustomGamepadConfiguration::getHardwareRevision(){ return _hardwareRevision; }
bool BleCustomGamepadConfiguration::getEnableOutputReport(){ return _enableOutputReport; }
bool BleCustomGamepadConfiguration::getEnableNordicUARTService(){ return _enableNordicUARTService; }
uint16_t BleCustomGamepadConfiguration::getOutputReportLength(){ return _outputReportLength; }
int8_t BleCustomGamepadConfiguration::getTXPowerLevel(){ return _transmitPowerLevel; }	// Returns the power level that was set as the server started

void BleCustomGamepadConfiguration::setWhichSpecialButtons(bool start, bool select, bool menu, bool home, bool back, bool volumeInc, bool volumeDec, bool volumeMute)
{
    _whichSpecialButtons[START_BUTTON] = start;
    _whichSpecialButtons[SELECT_BUTTON] = select;
    _whichSpecialButtons[MENU_BUTTON] = menu;
    _whichSpecialButtons[HOME_BUTTON] = home;
    _whichSpecialButtons[BACK_BUTTON] = back;
    _whichSpecialButtons[VOLUME_INC_BUTTON] = volumeInc;
    _whichSpecialButtons[VOLUME_DEC_BUTTON] = volumeDec;
    _whichSpecialButtons[VOLUME_MUTE_BUTTON] = volumeMute;
}

void BleCustomGamepadConfiguration::setWhichAxes(bool xAxis, bool yAxis, bool zAxis, bool rxAxis, bool ryAxis, bool rzAxis, bool slider1, bool slider2)
{
    _whichAxes[X_AXIS] = xAxis;
    _whichAxes[Y_AXIS] = yAxis;
    _whichAxes[Z_AXIS] = zAxis;
    _whichAxes[RZ_AXIS] = rzAxis;
    _whichAxes[RX_AXIS] = rxAxis;
    _whichAxes[RY_AXIS] = ryAxis;
    _whichAxes[SLIDER1] = slider1;
    _whichAxes[SLIDER2] = slider2;
}

void BleCustomGamepadConfiguration::setWhichSimulationControls(bool rudder, bool throttle, bool accelerator, bool brake, bool steering)
{
    _whichSimulationControls[RUDDER] = rudder;
    _whichSimulationControls[THROTTLE] = throttle;
    _whichSimulationControls[ACCELERATOR] = accelerator;
    _whichSimulationControls[BRAKE] = brake;
    _whichSimulationControls[STEERING] = steering;
}

void BleCustomGamepadConfiguration::setControllerType(uint8_t value) { _controllerType = value; }
void BleCustomGamepadConfiguration::setHidReportId(uint8_t value) { _hidReportId = value; }
void BleCustomGamepadConfiguration::setButtonCount(uint16_t value) { _buttonCount = value; }
void BleCustomGamepadConfiguration::setHatSwitchCount(uint8_t value) { _hatSwitchCount = value; }
void BleCustomGamepadConfiguration::setAutoReport(bool value) { _autoReport = value; }
void BleCustomGamepadConfiguration::setIncludeStart(bool value) { _whichSpecialButtons[START_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeSelect(bool value) { _whichSpecialButtons[SELECT_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeMenu(bool value) { _whichSpecialButtons[MENU_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeHome(bool value) { _whichSpecialButtons[HOME_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeBack(bool value) { _whichSpecialButtons[BACK_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeVolumeInc(bool value) { _whichSpecialButtons[VOLUME_INC_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeVolumeDec(bool value) { _whichSpecialButtons[VOLUME_DEC_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeVolumeMute(bool value) { _whichSpecialButtons[VOLUME_MUTE_BUTTON] = value; }
void BleCustomGamepadConfiguration::setIncludeXAxis(bool value) { _whichAxes[X_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeYAxis(bool value) { _whichAxes[Y_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeZAxis(bool value) { _whichAxes[Z_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeRzAxis(bool value) { _whichAxes[RZ_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeRxAxis(bool value) { _whichAxes[RX_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeRyAxis(bool value) { _whichAxes[RY_AXIS] = value; }
void BleCustomGamepadConfiguration::setIncludeSlider1(bool value) { _whichAxes[SLIDER1] = value; }
void BleCustomGamepadConfiguration::setIncludeSlider2(bool value) { _whichAxes[SLIDER2] = value; }
void BleCustomGamepadConfiguration::setIncludeRudder(bool value) { _whichSimulationControls[RUDDER] = value; }
void BleCustomGamepadConfiguration::setIncludeThrottle(bool value) { _whichSimulationControls[THROTTLE] = value; }
void BleCustomGamepadConfiguration::setIncludeAccelerator(bool value) { _whichSimulationControls[ACCELERATOR] = value; }
void BleCustomGamepadConfiguration::setIncludeBrake(bool value) { _whichSimulationControls[BRAKE] = value; }
void BleCustomGamepadConfiguration::setIncludeSteering(bool value) { _whichSimulationControls[STEERING] = value; }
void BleCustomGamepadConfiguration::setIncludeGyroscope(bool value) { _includeGyroscope = value; }
void BleCustomGamepadConfiguration::setIncludeAccelerometer(bool value) { _includeAccelerometer = value; }
void BleCustomGamepadConfiguration::setVid(uint16_t value) { _vid = value; }
void BleCustomGamepadConfiguration::setPid(uint16_t value) { _pid = value; }
void BleCustomGamepadConfiguration::setGuidVersion(uint16_t value) { _guidVersion = value; }
void BleCustomGamepadConfiguration::setAxesMin(int16_t value) { _axesMin = value; }
void BleCustomGamepadConfiguration::setAxesMax(int16_t value) { _axesMax = value; }
void BleCustomGamepadConfiguration::setSimulationMin(int16_t value) { _simulationMin = value; }
void BleCustomGamepadConfiguration::setSimulationMax(int16_t value) { _simulationMax = value; }
void BleCustomGamepadConfiguration::setMotionMin(int16_t value) { _motionMin = value; }
void BleCustomGamepadConfiguration::setMotionMax(int16_t value) { _motionMax = value; }
void BleCustomGamepadConfiguration::setModelNumber(char *value) { _modelNumber = value; }
void BleCustomGamepadConfiguration::setSoftwareRevision(char *value) { _softwareRevision = value; }
void BleCustomGamepadConfiguration::setSerialNumber(char *value) { _serialNumber = value; }
void BleCustomGamepadConfiguration::setFirmwareRevision(char *value) { _firmwareRevision = value; }
void BleCustomGamepadConfiguration::setHardwareRevision(char *value) { _hardwareRevision = value; }
void BleCustomGamepadConfiguration::setEnableOutputReport(bool value) { _enableOutputReport = value; }
void BleCustomGamepadConfiguration::setEnableNordicUARTService(bool value) { _enableNordicUARTService = value; }
void BleCustomGamepadConfiguration::setOutputReportLength(uint16_t value) { _outputReportLength = value; }
void BleCustomGamepadConfiguration::setTXPowerLevel(int8_t value) { _transmitPowerLevel = value; }
