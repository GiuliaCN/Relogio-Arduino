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

typedef enum {
  NORMAL, CONFIG
} Estado;

Estado estado;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

const int pinBUp = 13;
const int pinBDown = 12;

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("SSD1306 allocation failed"));
    Serial.flush();
    abort();
  }
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  
  // Limpa o buffer inicial da Adafruit (caso exiba o logo deles de início)
  display.clearDisplay();
  display.display();

  pinMode(pinBUp, INPUT);
  pinMode(pinBDown, INPUT);

  estado = NORMAL; // Inicia estado inicial como Normal
}

void loop() {
  // tirar essa parte depois
  String estadoStr = "";
  if (estado == NORMAL) estadoStr = "Normal";
  else estadoStr = "Config";
  Serial.println("Estado do relogio: " + estadoStr);

  boolean botaoUp = digitalRead(pinBUp);
  boolean botaoDown = digitalRead(pinBDown);

  boolean condicaoEntraConfig = botaoUp == true || botaoDown == true;
  if(condicaoEntraConfig) 
  {
    Serial.println("Mudou de Estado: Normal -> Config");
    estado = CONFIG;
  } 

  if(estado == NORMAL) rotinaNormal();
  else if(estado == CONFIG) rotinaConfig();
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

// Rotina do estado normal
void rotinaNormal(){
  
  // Atualizando a tela
  display.clearDisplay(); // Limpa a tela anterior para não sobrepor texto
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Escreve umidade no display
  display.setCursor(2,2);  
  display.print("Umidade: ");
  display.print(DHT.humidity, 1);
  Serial.print("Umidade: ");
  Serial.println(DHT.humidity, 1);
  
  DateTime now = rtc.now();
  doTime(now);  
  
  // Escreve hora no display
  display.setCursor(2,20);
  display.print("Hora: ");
  display.print(now.hour(), DEC);
  display.print(':');
  if(now.minute() < 10) display.print('0');
  display.print(now.minute(), DEC);
  display.print(':');
  if(now.second() < 10) display.print('0');
  display.print(now.second(), DEC);

  display.display(); 
  
  delay(1000);
}

// Rotina do estado Config
void rotinaConfig(){  
  Serial.println("Entrei na rotina do config");  
  delay(100);
}