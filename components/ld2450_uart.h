#include "esphome.h"

#define CHECK_BIT(var, pos) (((var) >> (pos)) & 1)
#define STATE_SIZE 8
#define TARGETS 3

static const char *TAG = "ld2450";
class LD2450 : public PollingComponent, public UARTDevice
{
public:
  LD2450(UARTComponent *parent) : UARTDevice(parent) {}

  BinarySensor *lastCommandSuccess = new BinarySensor();
  Sensor *target1Resolution = new Sensor();
  Sensor *target1Speed = new Sensor();
  Sensor *target1X = new Sensor();
  Sensor *target1Y = new Sensor();
  Sensor *target2Resolution = new Sensor();
  Sensor *target2Speed = new Sensor();
  Sensor *target2X = new Sensor();
  Sensor *target2Y = new Sensor();
  Sensor *target3Resolution = new Sensor();
  Sensor *target3Speed = new Sensor();
  Sensor *target3X = new Sensor();
  Sensor *target3Y = new Sensor();
  Sensor *targets = new Sensor();

  BinarySensor *configMode = new BinarySensor();
  BinarySensor *bluetoothState = new BinarySensor();
  TextSensor *macAddress = new TextSensor();
  TextSensor *fwVersion = new TextSensor();
  TextSensor *trackingMode = new TextSensor();

  Sensor *zoneType = new Sensor();
  Sensor *zone1X1 = new Sensor();
  Sensor *zone1Y1 = new Sensor();
  Sensor *zone1X2 = new Sensor();
  Sensor *zone1Y2 = new Sensor();
  Sensor *zone2X1 = new Sensor();
  Sensor *zone2Y1 = new Sensor();
  Sensor *zone2X2 = new Sensor();
  Sensor *zone2Y2 = new Sensor();
  Sensor *zone3X1 = new Sensor();
  Sensor *zone3Y1 = new Sensor();
  Sensor *zone3X2 = new Sensor();
  Sensor *zone3Y2 = new Sensor();

  uint32_t lastPeriodicMillis = millis();

  uint16_t twoByteToUint(char firstByte, char secondByte) {
    return (uint16_t)(secondByte << 8) + firstByte;
  }

  int16_t twoByteToInt(char firstByte, char secondByte) {
    return (int16_t)(secondByte << 8) | firstByte;
  }

  void sendCommand(char *commandStr, char *commandValue, int commandValueLen) {
    lastCommandSuccess->publish_state(false);
    // frame start bytes
    write_byte(0xFD);
    write_byte(0xFC);
    write_byte(0xFB);
    write_byte(0xFA);
    // length bytes
    int len = 2;
    if (commandValue != nullptr)
      len += commandValueLen;
    write_byte(lowByte(len));
    write_byte(highByte(len));
    // command string bytes
    write_byte(commandStr[0]);
    write_byte(commandStr[1]);
    // command value bytes
    if (commandValue != nullptr) {
      for (int i = 0; i < commandValueLen; i++) {
        write_byte(commandValue[i]);
      }
    }
    // frame end bytes
    write_byte(0x04);
    write_byte(0x03);
    write_byte(0x02);
    write_byte(0x01);
    delay(10);
  }

  void reportTargetInfo(int target, char *raw) {
    int16_t newX, newY, newSpeed;
    uint16_t newResolution;

    ESP_LOGV(TAG, "Will reporting taget %d", target);

    switch (target) {
      case 0:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        if (target1X->get_state() != newX)
          target1X->publish_state(newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        if (target1Y->get_state() != newY)
          target1Y->publish_state(newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        if (target1Speed->get_state() != newSpeed)
          target1Speed->publish_state(newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        if (target1Resolution->get_state() != newResolution)
          target1Resolution->publish_state(newResolution);
        break;
      case 1:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        if (target2X->get_state() != newX)
          target2X->publish_state(newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        if (target2Y->get_state() != newY)
          target2Y->publish_state(newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        if (target2Speed->get_state() != newSpeed)
          target2Speed->publish_state(newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        if (target2Resolution->get_state() != newResolution)
          target2Resolution->publish_state(newResolution);
        break;
      case 2:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        if (target3X->get_state() != newX)
          target3X->publish_state(newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        if (target3Y->get_state() != newY)
          target3Y->publish_state(newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        if (target3Speed->get_state() != newSpeed)
          target3Speed->publish_state(newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        if (target3Resolution->get_state() != newResolution)
          target3Resolution->publish_state(newResolution);
        break;
    }
  }

  void handlePeriodicData(char *buffer, int len) {
    if (len < 29)
      return;  // 4 frame start bytes + 2 length bytes + 1 data end byte + 1 crc byte + 4 frame end bytes
    if (buffer[0] != 0xAA || buffer[1] != 0xFF || buffer[2] != 0x03 || buffer[3] != 0x00)
      return;  // check 4 frame start bytes
    if (buffer[len - 2] != 0x55 || buffer[len - 1] != 0xCC)
      return;  //  data end=0x55, 0xcc
    char stateBytes[STATE_SIZE];

    /*
      Reduce data update rate to prevent home assistant database size glow fast
    */
    uint32_t currentMillis = millis();
    if (currentMillis - lastPeriodicMillis < 1000)
      return;
    lastPeriodicMillis = currentMillis;
    for (int i = 0; i < TARGETS; i++) {
      memcpy(stateBytes, &buffer[4 + i * STATE_SIZE], STATE_SIZE);
      reportTargetInfo(i, stateBytes);
    }
    uint8_t newTargets = 0;
    if (target1Resolution->get_state() > 0){
      newTargets+=1;
    }
    if (target2Resolution->get_state() > 0){
      newTargets+=1;
    }
    if (target3Resolution->get_state() > 0){
      newTargets+=1;
    }
    if (targets->get_state() != newTargets)
      targets->publish_state(newTargets);
  }

  void handleACKData(char *buffer, int len) {
    if (len < 10)
      return;
    if (buffer[0] != 0xFD || buffer[1] != 0xFC || buffer[2] != 0xFB || buffer[3] != 0xFA)
      return; // check 4 frame start bytes
    if (buffer[7] != 0x01)
      return;

    if (twoByteToUint(buffer[8], buffer[9]) != 0x00) {
      lastCommandSuccess->publish_state(false);
      return;
    }
    lastCommandSuccess->publish_state(true);
    switch (buffer[6])
    {
      case 0x80: // Query Single Tracking response
      {
        trackingMode->publish_state("Single");
      }
      break;

      case 0x90: // Query Multiple Trackingresponse
      {
        trackingMode->publish_state("Multiple");
      }
      break;

      case 0xA0: // Query FW Version response
      {
        char version[18];
        snprintf(version, sizeof(version), "%01X.%02X.%02X%02X%02X%02X",
                buffer[13], buffer[12], buffer[17], buffer[16], buffer[15], buffer[14]);
        fwVersion->publish_state(version);
      }
      break;

      case 0xA5: // Query Mac Address response
      {
        char mac[18];
        snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
        macAddress->publish_state(mac);
      }
      break;

      case 0xC1: // Query Zone Setting response
      {
        int16_t newX, newY;
        zoneType->publish_state(buffer[10]);

        newX = twoByteToInt(buffer[12], buffer[13] & 0x7F);
        zone1X1->publish_state(newX / 10);
        newY = twoByteToInt(buffer[14], buffer[15] & 0x7F);
        zone1Y1->publish_state(newY / 10);
        newX = twoByteToInt(buffer[16], buffer[17] & 0x7F);
        zone1X2->publish_state(newX / 10);
        newY = twoByteToInt(buffer[18], buffer[19] & 0x7F);
        zone1Y2->publish_state(newY / 10);

        newX = twoByteToInt(buffer[20], buffer[21] & 0x7F);
        zone2X1->publish_state(newX / 10);
        newY = twoByteToInt(buffer[22], buffer[23] & 0x7F);
        zone2Y1->publish_state(newY / 10);
        newX = twoByteToInt(buffer[24], buffer[25] & 0x7F);
        zone2X2->publish_state(newX / 10);
        newY = twoByteToInt(buffer[26], buffer[27] & 0x7F);
        zone2Y2->publish_state(newY / 10);

        newX = twoByteToInt(buffer[28], buffer[29] & 0x7F);
        zone3X1->publish_state(newX / 10);
        newY = twoByteToInt(buffer[30], buffer[31] & 0x7F);
        zone3Y1->publish_state(newY / 10);
        newX = twoByteToInt(buffer[32], buffer[33] & 0x7F);
        zone3X2->publish_state(newX / 10);
        newY = twoByteToInt(buffer[34], buffer[35] & 0x7F);
        zone3Y2->publish_state(newY / 10);
      }
      break;

    default:
      break;
    }
  }

  void readline(int readch, char *buffer, int len) {
    static int pos = 0;
    if (readch >= 0) {
      if (pos < len - 1) {
        buffer[pos++] = readch;
        buffer[pos] = 0;
      } else {
        pos = 0;
      }
      if (pos >= 4) {
        if (buffer[pos - 2] == 0x55 && buffer[pos - 1] == 0xCC) {
          handlePeriodicData(buffer, pos);
          pos = 0;  // Reset position index ready for next time
        } else if (buffer[pos - 4] == 0x04 && buffer[pos - 3] == 0x03 && buffer[pos - 2] == 0x02 && buffer[pos - 1] == 0x01) {
          handleACKData(buffer, pos);
          pos = 0;  // Reset position index ready for next time
        }
      }
    }
    return;
  }

  void setup()  override {
    set_update_interval(15000);
  }

  void loop() override {
    const int max_line_length = 160;
    static char buffer[max_line_length];
    while (available()) {
      readline(read(), buffer, max_line_length);
    }
  }

  void setConfigMode(bool enable) {
    char cmd[2] = {enable ? (char)0xFF : (char)0xFE, 0x00};
    char value[2] = {0x01, 0x00};
    sendCommand(cmd, enable ? value : nullptr, enable ? 2 : 0);
    configMode->publish_state(enable);
  }

  void setSingle() {
    char cmd[2] = {0x80, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
  }

  void setMultiple() {
    char cmd[2] = {0x90, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
  }

  void getFwVersion() {
    char cmd[2] = {0xA0, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
  }

  void setBaudrate(int index) {
    char cmd[2] = {0xA1, 0x00};
    char value[2] = {(char)index, 0x00};
    setConfigMode(true);
    sendCommand(cmd, value, 2);
    setConfigMode(false);
  }

  void factoryReset() {
    char cmd[2] = {0xA2, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
  }

  void reboot() {
    char cmd[2] = {0xA3, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
    // not need to exit config mode because the ra2413mt will reboot automatically
  }

  void setBluetooth(bool enable) {
    char cmd[2] = {0xA4, 0x00};
    char value[2] = {enable? (char)0x01: (char)0x00, 0x00};
    setConfigMode(true);
    sendCommand(cmd, value, 2);
    setConfigMode(false);
    bluetoothState->publish_state(enable);
  }

  void getMacAddress() {
    char cmd[2] = {0xA5, 0x00};
    char value[2] = {0x01, 0x00};
    setConfigMode(true);
    sendCommand(cmd, value, 2);
    setConfigMode(false);
  }

  void getZone() {
    char cmd[2] = {0xC1, 0x00};
    setConfigMode(true);
    sendCommand(cmd, nullptr, 0);
    setConfigMode(false);
  }

  void setZone(int type, int z1x1, int z1y1, int z1x2, int z1y2, int z2x1, int z2y1, int z2x2, int z2y2, int z3x1, int z3y1, int z3x2, int z3y2) {
    char cmd[2] = {0xC2, 0x00};
    char value[26] = {lowByte(type), highByte(type), \
        lowByte(z1x1), highByte(z1x1), lowByte(z1y1), highByte(z1y1), lowByte(z1x2), highByte(z1x2), lowByte(z1y2), highByte(z1y2), \
        lowByte(z2x1), highByte(z2x1), lowByte(z2y1), highByte(z2y1), lowByte(z2x2), highByte(z2x2), lowByte(z2y2), highByte(z2y2), \
        lowByte(z3x1), highByte(z3x1), lowByte(z3y1), highByte(z3y1), lowByte(z3x2), highByte(z3x2), lowByte(z3y2), highByte(z3y2)};
    setConfigMode(true);
    sendCommand(cmd, value, 26);
    setConfigMode(false);
  }

  void getInfo() {
    setConfigMode(true);
    getMacAddress();
    getFwVersion();
    getZone();
    setConfigMode(false);
  }

  void update() {
  }
};
