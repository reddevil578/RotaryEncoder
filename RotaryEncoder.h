/* vim:set ts=2 sts=2 sw=2 et: */

#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H


class RotaryEncoder {
  public:

  const int ENC_UP 1
  const int ENC_NONE 0
  const int ENC_DOWN -1

  // Digital I/O pins connected to the encoder
  unsigned int pinCk: 4;
  unsigned int pinDt: 4;
  unsigned int pinSw: 4;

  // Delay between sampling control at idle (0 - 15 ms). Note: lower resolution
  // sampling (higher delay) can cause more frequent misreads!
  unsigned int sampleDelay: 4;

  // State flags: true when waiting for asserted state to fall back to idle
  bool encQuiescing: 1;
  bool swQuiescing: 1;

  // Last sample time
  unsigned long timeLastUpdate;

  // A multivalued return type
  struct Value {
    int8_t increment;
    bool switchState;
  };

  // Create a new RotaryEncoder interface connected to the given pins
  RotaryEncoder(int ckPin, int dtPin, int swPin = ENC_NONE, int sampleTime = 15);

  // Call with current time in us to check current encoder state
  Value Poll(unsigned long now = micros());
};

#endif /* ROTARYENCODER_H */
