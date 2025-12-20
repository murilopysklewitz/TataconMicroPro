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

void processSerial(){
  if (Serial.available() < 1) return;

  char buffer[32];
  byte len = Serial.readBytesUntil('\n', buffer, 31);
  if(len == 0) return;
  buffer[len] = '\0';

  if(strcmp(buffer, "config") == 0){
    configMode = true;
    Serial.println("CONFIG_MODE");
    return;
  }
  
  if(strcmp(buffer, "play") == 0){
    configMode = false;
    Serial.println("GAME_MODE");
    return;
  }
  
  if(strcmp(buffer, "test") == 0){
    Serial.println("TEST_MODE_START");
    unsigned long start = millis();
    
    while (millis() - start < 5000) {
      for (int i = 0; i < 4; i++) {
        Serial.print("v");
        Serial.print(i);
        Serial.print(":");
        Serial.print(analogRead(pinos[i]));
        if (i < 3) Serial.print("|");
      }
      Serial.println();
      delay(50);
    }
    
    Serial.println("TEST_COMPLETE");
    return;
  }
  
  if (strncmp(buffer, "cooldown ", 9) == 0) {
    cooldown = atoi(buffer + 9);
    Serial.print("OK_COOLDOWN_");
    Serial.println(cooldown);
    return;
  }

  if(len >= 4 && buffer[2] == ' '){
    char type = buffer[0];
    int sensor = buffer[1] - '0';
    int value = atoi(buffer + 3);

    if(sensor >= 0 && sensor < 4){
      if(type == 't'){
        threshold[sensor] = value;
        Serial.print("OK_T");
        Serial.print(sensor);
        Serial.print("_");
        Serial.println(value);
        
      } else if(type == 'd'){
        deltaMin[sensor] = value;
        Serial.print("OK_D");
        Serial.print(sensor);
        Serial.print("_");
        Serial.println(value);
      }
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
  processSerial();
  
  static unsigned long lastSend = 0;

  if (millis() - lastSend > 100) {
    for (int i = 0; i < 4; i++) {
      Serial.print("v");
      Serial.print(i);
      Serial.print(":");
      Serial.print(analogRead(pinos[i]));
      if (i < 3) Serial.print("|");
    }
    Serial.println();
    lastSend = millis();
  }
  
  delay(10);  
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();

  for (int i = 0; i < 4; i++) {
    lastValue[i] = analogRead(pinos[i]);
  }
}

void loop() {
  if(!configMode){
    loopGame();
    return;
  }

  loopConfig();
}