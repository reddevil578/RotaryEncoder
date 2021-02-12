#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(int ckPin, int dtPin, int swPin, int sampleTime) :
    pinCk(ckPin),
    pinDt(dtPin),
    pinSw(swPin),
    encQuiescing(false),
    swQuiescing(false),
    sampleDelay(sampleTime) {
  pinMode(pinCk, INPUT);
  pinMode(pinDt, INPUT);

  if (pinSw)
    pinMode(pinSw, INPUT);
};

RotaryEncoder::Value RotaryEncoder::Poll(unsigned long now) {
  int ckVal;
  int dtVal;
  bool swVal = false;

  // If we're waiting for the button push to end, test that.
  if (swQuiescing && digitalRead(pinSw))
      // It ended. Allow polling for the next push to continue.
      swQuiescing = false;

  // If we're waiting for the encoder contact to leave the trigger mark, test that.
  if (encQuiescing) {
    if (digitalRead(pinCk) && digitalRead(pinDt))
      // It has left the mark. Allow polling the enocder to resume.
      encQuiescing = false;

    return {increment: ENC_NONE, switchState: false};
  }

  // Implement the sampling delay
  if (now - timeLastUpdate <= ((unsigned long)sampleDelay * 1000))
    return {increment: ENC_NONE, switchState: false};

  if (!swQuiescing)
    // Poll the button if we're not still processing the previous push
    if (swVal = pinSw ? !digitalRead(pinSw) : false)
      swQuiescing = true;
  
  Value value = {increment: ENC_NONE, switchState: swVal};

  // Poll the encoder pins
  ckVal = digitalRead(pinCk);
  dtVal = digitalRead(pinDt);

  if (ckVal == LOW) {
    // We've detected a movement. Determine which direction and let quiesce.
    value.increment = (dtVal == LOW) ? ENC_DOWN: ENC_UP;
    encQuiescing = true;
  }
  else
    value.increment = ENC_NONE;

  timeLastUpdate = now;
  
  return value;
}
