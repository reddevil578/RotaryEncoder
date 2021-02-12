#define ENC_UP 1
#define ENC_NONE 0
#define ENC_DOWN -1

class RotaryEncoder {
  public:

  // Digital I/O pins connected to the encoder
  unsigned int pinCk: 4;
  unsigned int pinDt: 4;
  unsigned int pinSw: 4;

  // State flags: true when waiting for asserted state to fall back to idle
  bool encQuiescing = false;
  bool swQuiescing = false;

  // Delay between sampling control at idle (0 - 15 ms). Note: lower resolution
  // sampling (higher delay) can cause more frequent misreads!
  unsigned int sampleDelay: 4;

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


RotaryEncoder::RotaryEncoder(int ckPin, int dtPin, int swPin, int sampleTime) :
    pinCk(ckPin), pinDt(dtPin), pinSw(swPin), sampleDelay(sampleTime) {
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

RotaryEncoder r(2, 3, 4, 1);

void setup() {
  Serial.begin(115200);
}

void loop() {  
  unsigned long t_now = micros();
  RotaryEncoder::Value v = r.Poll(t_now);

  switch (v.increment) {
    case ENC_UP: Serial.println("UP!"); break;
    case ENC_DOWN: Serial.println("DOWN!"); break;
  }

  if (v.switchState)
    Serial.println("Button press");
}
