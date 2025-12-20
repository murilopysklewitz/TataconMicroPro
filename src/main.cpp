#include <Arduino.h>
#include <Keyboard.h>

const int DON_ESQ = A0;
const int KA_ESQ  = A1;
const int DON_DIR = A2;
const int KA_DIR  = A3;

const int pinos[4] = { DON_ESQ, KA_ESQ, DON_DIR, KA_DIR };
const char teclas[4] = { 'f', 'd', 'j', 'k' };

int threshold[4] = { 100, 180, 170, 180 };
int deltaMin[4]  = { 25,  25,  25,  25  };

unsigned long cooldown = 70;
unsigned long lastHit[4] = {0,0,0,0};
int lastValue[4] = {0,0,0,0};

bool configMode = false;

void processSerial() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "config") { configMode = true; Serial.println("MODE:CONFIG"); }
    else if (cmd == "play") { configMode = false; Serial.println("MODE:PLAY"); }
    
    else if (cmd.startsWith("t")) {
      int sensor = cmd.substring(1,2).toInt();
      int val = cmd.substring(3).toInt();
      if(sensor >=0 && sensor <4) threshold[sensor] = val;
      Serial.print("SET:Threshold["); Serial.print(sensor); Serial.print("]="); Serial.println(val);
    }
    else if (cmd.startsWith("d")) {
      int sensor = cmd.substring(1,2).toInt();
      int val = cmd.substring(3).toInt();
      if(sensor >=0 && sensor <4) deltaMin[sensor] = val;
      Serial.print("SET:Delta["); Serial.print(sensor); Serial.print("]="); Serial.println(val);
    }
    else if (cmd.startsWith("c ")) {
      cooldown = cmd.substring(2).toInt();
      Serial.print("SET:Cooldown="); Serial.println(cooldown);
    }
  }
}

void loopGame() {
  unsigned long now = millis();

  for (int i = 0; i < 4; i++) {
    int v  = analogRead(pinos[i]);
    int dv = v - lastValue[i];

    if (dv > deltaMin[i] &&
        v > threshold[i] &&
        now - lastHit[i] > cooldown) {

      Keyboard.press(teclas[i]);
      delay(20);  
      Keyboard.release(teclas[i]);

      lastHit[i] = now;
    }

    lastValue[i] = v;
  }

  delay(1);
}

void loopConfig() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 50) { 
    for(int i=0; i<4; i++) {
      Serial.print(i); Serial.print(":"); Serial.print(analogRead(pinos[i]));
      if(i<3) Serial.print(",");
    }
    Serial.println();
    lastPrint = millis();
  }
  return;
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();

  for (int i = 0; i < 4; i++) {
    lastValue[i] = analogRead(pinos[i]);
  }
}

void loop() {
  processSerial();

  if (configMode) {
    loopConfig();
  }

  loopGame();
  
}