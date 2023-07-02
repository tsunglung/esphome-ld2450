#include "esphome.h"

#define CHECK_BIT(var, pos) (((var) >> (pos)) & 1)
#define STATE_SIZE 8
#define TARGETS 3

static const char *TAG = "ld2450";
class LD2450 : public PollingComponent, public UARTDevice
{
public:
  LD2450(UARTComponent *parent) : UARTDevice(parent) {}

  // BinarySensor *lastCommandSuccess = new BinarySensor();
  Sensor *lastCommandSuccess = new Sensor();
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

  uint32_t lastPeriodicMillis = millis();

  uint16_t twoByteToUint(char firstByte, char secondByte) {
    return (uint16_t)(secondByte << 8) + firstByte;
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

  void update(){
  }
};
