#include <IRremote.h>

// --- Pin Config ---
uint8_t buzzerPin = 5;
uint8_t green     = 12;
uint8_t red1      = 3;
uint8_t red2      = 7;
uint8_t red3      = 11;
uint8_t red4      = 8;
uint8_t gas       = A0;
const uint8_t RECV_PIN = 9;

// --- Replace with actual values from Serial Monitor ---
#define WARNING_CODE      4261494848UL
#define DANGER_CODE       4244783168UL
#define MORE_DANGER_CODE  4228071488UL
#define OFF_CODE          2991407168UL
// ------------------------------------------------------

unsigned long currentTime = 0;
unsigned long prevBlinkTime = 0;

uint8_t activeMode = 0;      // 0=off, 1=warning, 2=danger, 3=more danger
bool remoteControl = false;  // true when remote is controlling

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  // Setup pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(red3, OUTPUT);
  pinMode(red4, OUTPUT);

  // Force everything OFF at startup
  digitalWrite(buzzerPin, LOW);
  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(red3, LOW);
  digitalWrite(red4, LOW);
  digitalWrite(green, HIGH);  

  activeMode = 0;
  remoteControl = false;
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("IR Code Received: ");
    Serial.println(code);

    if (code == WARNING_CODE) {
      remoteControl = true;
      activeMode = 1;
    } else if (code == DANGER_CODE) {
      remoteControl = true;
      activeMode = 2;
    } else if (code == MORE_DANGER_CODE) {
      remoteControl = true;
      activeMode = 3;
    } else if (code == OFF_CODE) {
      remoteControl = true;
      activeMode = 0;
      resetOutputs();
    }

    IrReceiver.resume(); // prepare for next signal
  }

  int level = analogRead(gas);

  if (level >= 550) {
    Serial.println("MORE DANGER: Smoke level rising!");
    activeMode = 3;
    remoteControl = false; 
  } else if (level >= 450) {
    Serial.println("DANGER: Smoke level very HIGH!");
    activeMode = 2;
    remoteControl = false;
  } else if (level >= 400) {
    Serial.println("WARNING: Smoke detected!");
    activeMode = 1;
    remoteControl = false;
  } else if (level <= 350 && !remoteControl) {
    activeMode = 0;
    resetOutputs();
  }

  if (activeMode == 1) blinkAlert2(300);
  else if (activeMode == 2) blinkAlert2(200);
  else if (activeMode == 3) blinkAlert2(100);
  else resetOutputs(); 
}

void blinkAlert2(int intervalMs) {
  currentTime = millis();

  if (currentTime - prevBlinkTime >= intervalMs) {
    prevBlinkTime = currentTime;

    // Toggle buzzer + RED LEDs only
    digitalWrite(buzzerPin, !digitalRead(buzzerPin));
    digitalWrite(red1, !digitalRead(red1));
    digitalWrite(red2, !digitalRead(red2));
    digitalWrite(red3, !digitalRead(red3));
    digitalWrite(red4, !digitalRead(red4));

    
    digitalWrite(green, LOW);
  }
}

// --- Reset all outputs ---
void resetOutputs() {
  digitalWrite(buzzerPin, LOW);
  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(red3, LOW);
  digitalWrite(red4, LOW);
  digitalWrite(green, HIGH); 
}
