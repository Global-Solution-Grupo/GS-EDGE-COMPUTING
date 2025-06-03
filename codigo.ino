include <LiquidCrystal_I2C.h>
#include <Wire.h>  
#include <RTClib.h>   
#include <EEPROM.h>

// Ajuste do fuso horário (UTC-3 para horário de Brasília)
#define UTC_OFFSET -3

// Instancia o objeto do RTC (relógio de tempo real)
RTC_DS1307 RTC;

// Variáveis para controle de tempo
unsigned long lastSecond = 0;
unsigned long lastEEPROMWrite = 0;
unsigned long lastSerialPrint = 0;
bool alertTriggered = false;

// Configuração da EEPROM para armazenar até 180 registros (cada um com 5 bytes: 4 para o tempo e 1 para o nível)
const int maxRecords = 180;
const int recordSize = 5;
int startAddress = 0;
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

// Pinos dos LEDs e buzzer
int ledRed = 2;
int ledYellow = 4;
int ledOrange = 12;
int ledGreen = 7;
int buzzer = 8;

// Pinos dos sensores de movimento
int motionSensor1 = 9;
int motionSensor2 = 10;
int motionSensor3 = 11;

// Leituras dos sensores
int sensor1;
int sensor2;
int sensor3;

// Buffers para calcular médias a cada 10 leituras (ajuda a suavizar os dados)
float sensorBuffer1[10], sensorBuffer2[10], sensorBuffer3[10];
int bufferIndex = 0;

// Inicialização do LCD I2C 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- SETUP ---
void setup() {
    pinMode(ledRed, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledOrange, OUTPUT);
    pinMode(buzzer, OUTPUT);

    pinMode(motionSensor1, INPUT);
    pinMode(motionSensor2, INPUT);
    pinMode(motionSensor3, INPUT);

    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    RTC.begin();
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Ajuste automático com a hora de compilação

    // Mensagem de boas-vindas
    lcd.setCursor(0, 0);
    lcd.print("Seja bem-vindo");
    delay(3000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Medidor de nivel");
    lcd.setCursor(0, 1); 
    lcd.print("de agua");
    delay(3000);
    lcd.clear(); 
}

// --- LOOP PRINCIPAL ---
void loop() {
    lcd.clear();

    // Atualiza a hora
    DateTime nowRTC = RTC.now();
    long nowUnix = nowRTC.unixtime() + (UTC_OFFSET * 3600);
    DateTime now = DateTime(nowUnix);

    // Exibe hora no LCD
    lcd.print(now.hour() < 10 ? "0" : ""); lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute() < 10 ? "0" : ""); lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second() < 10 ? "0" : ""); lcd.print(now.second());

    // Executa a cada segundo
    if (millis() - lastSecond >= 1000) {
        lastSecond = millis();

        // Lê os sensores
        sensor1 = digitalRead(motionSensor1);
        sensor2 = digitalRead(motionSensor2);
        sensor3 = digitalRead(motionSensor3);

        // Armazena as leituras nos buffers
        sensorBuffer1[bufferIndex] = sensor1;
        sensorBuffer2[bufferIndex] = sensor2;
        sensorBuffer3[bufferIndex] = sensor3;
        bufferIndex++;

        // Quando 10 leituras forem coletadas, calcula a média (soma neste caso) e decide o nível de alerta
        if (bufferIndex >= 10) {
            int somasensor1 = 0, somasensor2 = 0, somasensor3 = 0;
            for (int i = 0; i < 10; i++) {
                somasensor1 += sensorBuffer1[i];
                somasensor2 += sensorBuffer2[i];
                somasensor3 += sensorBuffer3[i];
            }

            bufferIndex = 0;  // Reinicia buffer

            // Define alertas com base na presença de água (presença == 1)
            if (somasensor1 == 10) {
                if (somasensor2 == 10) {
                    if (somasensor3 == 10) {
                        // ALERTA MÁXIMO
                        digitalWrite(ledRed, HIGH);
                        digitalWrite(buzzer, HIGH);
                        Serial.println("CUIDADO! ENCHENTE IMINENTE!");
                        Serial.println("SAIA DO LOCAL IMEDIATAMENTE E ALERTE AS AUTORIDADES");
                        lcd.setCursor(0, 1);
                        lcd.print("ENCHENTE IMINENTE!");
                        delay(10000);
                        digitalWrite(ledRed, LOW);
                        digitalWrite(buzzer, LOW);

                        EEPROM.put(currentAddress, nowUnix);
                        EEPROM.put(currentAddress + 4, 3);
                    } else {
                        // ALERTA ALTO
                        digitalWrite(ledOrange, HIGH);
                        digitalWrite(buzzer, HIGH);
                        Serial.println("RISCO GRANDE DE ENCHENTE!");
                        Serial.println("Vá para um abrigo e fique em um local adequado");
                        lcd.setCursor(0, 1);
                        lcd.print("RISCO GRANDE!");
                        delay(10000);
                        digitalWrite(ledOrange, LOW);
                        digitalWrite(buzzer, LOW);

                        EEPROM.put(currentAddress, nowUnix);
                        EEPROM.put(currentAddress + 4, 2);
                    }
                } else {
                    // ALERTA MODERADO
                    digitalWrite(ledYellow, HIGH);
                    digitalWrite(buzzer, HIGH);
                    Serial.println("Risco de enchente");
                    Serial.println("Vá para um local seguro");
                    lcd.setCursor(0, 1);
                    lcd.print("RISCO DE ENCHENTE!");
                    delay(10000);
                    digitalWrite(ledYellow, LOW);
                    digitalWrite(buzzer, LOW);

                    EEPROM.put(currentAddress, nowUnix);
                    EEPROM.put(currentAddress + 4, 1);
                }
            } else {
                // SEM RISCO
                digitalWrite(ledGreen, HIGH);
                Serial.println("Não há risco de enchente.");
                lcd.setCursor(0, 1);
                lcd.print("TUDO OK!");
                delay(10000);
                digitalWrite(ledGreen, LOW);

                EEPROM.put(currentAddress, nowUnix);
                EEPROM.put(currentAddress + 4, 0);
            }

            // Avança para próximo endereço na EEPROM
            currentAddress += recordSize;
            if (currentAddress >= endAddress) {
                currentAddress = 0; // sobrescreve do início
            }

            alertTriggered = false;
        }
    }

    // Verifica se comandos foram enviados via Serial
    checkSerialCommands();
}

// --- FUNÇÃO PARA EXPORTAR DADOS VIA SERIAL ---
void checkSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        // Comando para exportar dados no formato CSV
        if (command == "export") {
            Serial.println("Timestamp,Nivel");

            for (int address = startAddress; address < endAddress; address += recordSize) {
                long timeStamp;
                int nivel;

                EEPROM.get(address, timeStamp);
                EEPROM.get(address + 4, nivel);

                if (timeStamp > 1600000000 && timeStamp < 2000000000) {
                    DateTime dt = DateTime(timeStamp);
                    Serial.print(timeStamp); Serial.print(",");
                    Serial.print(dt.day() < 10 ? "0" : ""); Serial.print(dt.day()); Serial.print("/");
                    Serial.print(dt.month() < 10 ? "0" : ""); Serial.print(dt.month()); Serial.print("/");
                    Serial.print(dt.year()); Serial.print(",");
                    Serial.print(dt.hour() < 10 ? "0" : ""); Serial.print(dt.hour()); Serial.print(":");
                    Serial.print(dt.minute() < 10 ? "0" : ""); Serial.print(dt.minute()); Serial.print(",");
                    Serial.print(nivel);
                    Serial.println();
                }
            }

            Serial.println("FIM DA EXPORTAÇÃO.");
        }
    }
}