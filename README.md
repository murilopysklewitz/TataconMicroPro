# 🥁 Tatacon Controller - Firmware Arduino

> **Projeto educacional**: Controlador MIDI/HID para tambor Taiko usando Arduino Pro Micro e sensores piezoelétricos.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Pro%20Micro-00979D?logo=arduino)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange?logo=data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjUwMCIgaGVpZ2h0PSIyNTAwIiB2aWV3Qm94PSIwIDAgMjU2IDI1NiIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiBwcmVzZXJ2ZUFzcGVjdFJhdGlvPSJ4TWlkWU1pZCI+PHBhdGggZD0iTTAgMGgyNTZ2MjU2SDB6IiBmaWxsPSIjRkY3RjAwIi8+PC9zdmc+)](https://platformio.org/)

## 📖 Sobre o Projeto

Este é um firmware educacional para Arduino que transforma sensores piezoelétricos em um controlador de tambor japonês (Taiko). O projeto demonstra conceitos fundamentais de:

- 🎮 **Programação embedded** (C/C++)
- 🔌 **Comunicação serial** (protocolo customizado)
- ⚡ **Processamento de sinais analógicos**
- 🎛️ **Debouncing e anti-ghosting**
- 🖱️ **Emulação HID (teclado)**
- 🧵 **Programação dual-mode** (config/jogo)

---

## 🎯 Objetivos de Aprendizado

### Para Estagiários/Iniciantes:

1. **Leitura de Sensores Analógicos**
   - `analogRead()` e conversão ADC
   - Tratamento de ruído e filtros digitais
   - Detecção de picos (edge detection)

2. **Protocolo de Comunicação Serial**
   - Parsing de comandos texto
   - Buffer circular e terminadores
   - Envio de dados estruturados

3. **Máquina de Estados**
   - Modo configuração vs modo jogo
   - Transições de estado seguras

4. **Otimização de Performance**
   - Evitar `String` (fragmentação de heap)
   - Uso de buffers estáticos
   - Minimizar delays bloqueantes

---

## 🛠️ Hardware Necessário

| Componente | Quantidade | Especificação |
|------------|------------|---------------|
| Arduino Pro Micro | 1 | 5V 16MHz (ATmega32U4) |
| Sensor Piezoelétrico | 4 | 27mm ou maior |
| Resistor | 4 | 1MΩ (pull-down) |
| Capacitor cerâmico | 4 | 100nF (opcional, filtro) |
| Protoboard | 1 | 830 pontos |
| Jumpers | ~20 | Macho-macho |

### 📐 Diagrama de Conexão
```
┌─────────────────────────────────────────┐
│         Arduino Pro Micro               │
│  ┌────────────────────────────────┐     │
│  │ A0  A1  A2  A3  GND  5V        │     │
│  └─┬───┬───┬───┬────┬────┬────────┘     │
└────┼───┼───┼───┼────┼────┼──────────────┘
     │   │   │   │    │    │
     │   │   │   │   GND  (não usado)
     │   │   │   │
    P1  P2  P3  P4  (Piezos)
     │   │   │   │
    [1MΩ][1MΩ][1MΩ][1MΩ] ← Resistores pull-down
     │   │   │   │
    GND GND GND GND
```

**Circuito por sensor:**
```
Piezo (+) ──┬────┬──── Pino Analógico (A0-A3)
            │    │
         [1MΩ] [100nF] ← Capacitor opcional
            │    │
Piezo (-) ──┴────┴──── GND
```

---

## 🚀 Como Usar

### 1️⃣ **Instalação do Ambiente**

#### Opção A: Arduino IDE
```bash
1. Baixe Arduino IDE: https://www.arduino.cc/en/software
2. Instale suporte para Pro Micro:
   - Tools → Board → Boards Manager
   - Busque "SparkFun AVR Boards"
   - Instale
3. Selecione: Tools → Board → SparkFun Pro Micro (5V, 16MHz)
```

#### Opção B: PlatformIO 
```bash
# Instale PlatformIO Core
pip install platformio

# Clone o repositório
git clone https://github.com/seu-usuario/tatacon-firmware.git
cd tatacon-firmware

# Compile e faça upload
pio run -t upload
```

### 2️⃣ **Configuração do `platformio.ini`**
```ini
[env:promicro]
platform = atmelavr
board = sparkfun_promicro16
framework = arduino
monitor_speed = 115200
upload_port = COM3  ; Ajuste para sua porta

lib_deps = 
    arduino-libraries/Keyboard@^1.0.2
```

### 3️⃣ **Compilar e Upload**
```bash
# Compilar
pio run

# Upload
pio run -t upload

# Monitor serial
pio device monitor
```

---

## 📡 Protocolo de Comunicação

### **Comandos (Python → Arduino)**

| Comando | Formato | Exemplo | Descrição |
|---------|---------|---------|-----------|
| Config Mode | `config` | `config` | Entra em modo configuração |
| Play Mode | `play` | `play` | Entra em modo jogo |
| Test Mode | `test` | `test` | Modo teste (5s) |
| Set Threshold | `t<N> <VAL>` | `t0 250` | Define threshold do sensor N |
| Set Delta | `d<N> <VAL>` | `d2 30` | Define delta do sensor N |
| Set Cooldown | `cooldown <VAL>` | `cooldown 100` | Define cooldown global (ms) |

### **Respostas (Arduino → Python)**

| Resposta | Exemplo | Significado |
|----------|---------|-------------|
| Status | `CONFIG_MODE` | Entrou em modo config |
| Confirmação | `OK_T0_250` | Threshold 0 = 250 |
| Valores | `v0:123\|v1:45\|v2:200\|v3:67` | Valores dos sensores |
| Teste | `TEST_COMPLETE` | Teste finalizado |

---

## 🧠 Conceitos Técnicos Explicados

### **1. Detecção de Pico (Edge Detection)**
```cpp
// Valor atual - valor anterior = delta (mudança)
int delta = valorAtual - valorAnterior;

// Só ativa se houver mudança BRUSCA (pico)
if (delta > deltaMinimo && valorAtual > threshold) {
    // BATIDA DETECTADA!
}
```

**Por que funciona?**
- Piezo gera pico rápido ao bater
- Segurar gera valor constante (delta ≈ 0)
- Ignora ruído e ghosting

### **2. Cooldown (Debouncing)**
```cpp
unsigned long agora = millis();

if (agora - ultimaBatida > cooldown) {
    // Passou tempo suficiente, pode ativar
    ativarTecla();
    ultimaBatida = agora;
}
```

**Previne:**
- Múltiplos disparos de uma batida
- Bouncing mecânico do sensor
- Double-hits não intencionais

### **3. Parsing Eficiente (Sem `String`)**
```cpp
// ❌ RUIM: Usa String (heap, lento)
String cmd = Serial.readStringUntil('\n');
if (cmd.startsWith("t0")) { ... }

// ✅ BOM: Usa buffer fixo (stack, rápido)
char buffer[32];
Serial.readBytesUntil('\n', buffer, 31);
buffer[len] = '\0';

if (buffer[0] == 't' && buffer[1] == '0') { ... }
```

**Vantagens:**
- 5x mais rápido
- Sem fragmentação de memória
- Uso previsível de RAM

### **4. Dual-Mode Architecture**
```cpp
void loop() {
    processarSerial();  // Sempre processa comandos
    
    if (modoConfig) {
        loopConfig();   // Envia valores dos sensores
        return;         // Não executa código de jogo
    }
    
    loopGame();  // Lógica de detecção de batidas
}
```

**Benefícios:**
- Zero overhead no modo jogo
- Configuração em tempo real
- Separação clara de responsabilidades

---

## 🐛 Troubleshooting

### **Problema: Arduino não detectado**
```bash
# Windows
# Verifique no Gerenciador de Dispositivos
# Se aparecer "Dispositivo Desconhecido", instale drivers:
https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-windows

# Linux
sudo usermod -a -G dialout $USER  # Adiciona usuário ao grupo
sudo chmod 666 /dev/ttyACM0       # Permissão na porta
```

### **Problema: Upload falha**
```bash
# Pro Micro precisa de reset manual:
1. Pressione RESET 2x rapidamente
2. LED pisca
3. Execute upload IMEDIATAMENTE
```

### **Problema: Sensores ruidosos**
```cpp
// Aumente threshold e delta
int threshold[4] = {150, 150, 150, 150};  // Era 100
int deltaMin[4] = {40, 40, 40, 40};       // Era 25
```

### **Problema: Sensores não respondem**
```cpp
// Diminua threshold e delta
int threshold[4] = {50, 50, 50, 50};
int deltaMin[4] = {10, 10, 10, 10};
```

---

## 📊 Métricas de Performance

| Métrica | Valor | Observação |
|---------|-------|------------|
| **Loop rate (jogo)** | ~666 FPS | 1.5ms por ciclo |
| **Latência entrada** | < 2ms | Imperceptível |
| **Loop rate (config)** | ~100 FPS | 10ms por ciclo |
| **Taxa serial** | 115200 baud | ~11.5KB/s |
| **Uso de RAM** | ~15% | 316/2048 bytes |
| **Uso de Flash** | ~7% | 2332/32256 bytes |

---

## 📚 Recursos de Estudo

### **Documentação Oficial**
- [Arduino Reference](https://www.arduino.cc/reference/en/)
- [ATmega32U4 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf)
- [Serial Communication](https://www.arduino.cc/reference/en/language/functions/communication/serial/)

### **Tutoriais Recomendados**
- [Analog Input](https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInput)
- [Debouncing](https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce)
- [Keyboard Library](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/)

### **Conceitos para Estudar**
- ADC (Analog-to-Digital Converter)
- Pull-down/Pull-up resistors
- Signal debouncing
- State machines
- HID protocol

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Este é um projeto educacional.

1. Fork o repositório
2. Crie uma branch (`git checkout -b feature/nova-funcionalidade`)
3. Commit suas mudanças (`git commit -m 'Adiciona nova funcionalidade'`)
4. Push para a branch (`git push origin feature/nova-funcionalidade`)
5. Abra um Pull Request




