#include <Arduino.h>
#include <Keyboard.h>

// ---------------- Pinos ----------------
const int DON_ESQ = A0;
const int KA_ESQ  = A1;
const int DON_DIR = A2;
const int KA_DIR  = A3;

const int pinos[4] = { DON_ESQ, KA_ESQ, DON_DIR, KA_DIR };
const char teclas[4] = { 'f', 'd', 'j', 'k' };

// -------------- Sensibilidade --------------
int threshold[4] = { 80, 180, 80, 180 };
int deltaMin[4]  = { 25,  25,  25,  25  };

unsigned long cooldown = 35;
unsigned long lastHit[4] = {0,0,0,0};
int lastValue[4] = {0,0,0,0};

void setup() {
  Keyboard.begin();

  for (int i = 0; i < 4; i++) {
    lastValue[i] = analogRead(pinos[i]);
  }
  delay(500);
}

void loop() {
  unsigned long now = millis();

  for (int i = 0; i < 4; i++) {
    int v  = analogRead(pinos[i]);
    int dv = v - lastValue[i];

    if (dv > deltaMin[i] &&
        v > threshold[i] &&
        now - lastHit[i] > cooldown) {

      Keyboard.press(teclas[i]);
      delay(20);   // 2 ms (ideal)
      Keyboard.release(teclas[i]);

      lastHit[i] = now;
    }

    lastValue[i] = v;
  }

  delayMicroseconds(500);
}
