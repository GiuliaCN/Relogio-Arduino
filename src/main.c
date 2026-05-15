#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <dht.h>

dht DHT;

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RTC Configuration
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int pinBUp = 13;
const int pinBDown = 12;

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Limpa o buffer inicial da Adafruit (caso exiba o logo deles de início)
  display.clearDisplay();
  display.display();

  // Correção de digitação nos pinModes que estavam duplicados para o mesmo botão
  pinMode(pinBUp, INPUT);
  pinMode(pinBDown, INPUT); 
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    //abort();
  }
}

void loop() {
  boolean b1 = digitalRead(pinBUp);
  boolean b2 = digitalRead(pinBDown); // Corrigido para ler o botão Down correto
  
  Serial.print("Umidade: ");
  Serial.println(DHT.humidity, 1);

  if(b1) {
    Serial.println("B1");
    delay(100);
  }
  if(b2) {
    Serial.println("B2"); 
    delay(100);
  }
  
  DateTime now = rtc.now();
  doTime(now);
  
  // Atualizando a tela
  display.clearDisplay(); // Limpa a tela anterior para não sobrepor texto
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);
  
  // Exemplo opcional: printando a hora também na tela para testar
  display.print(now.hour(), DEC);
  display.print(':');
  if(now.minute() < 10) display.print('0');
  display.print(now.minute(), DEC);

  display.display(); 
  
  delay(1000);
}

void doTime(DateTime now) {
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}