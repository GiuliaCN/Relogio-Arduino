#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h> 

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);

// OLED Display 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RTC Configuration
RTC_DS3231 rtc;

// logo
#define ALIEN_WIDTH 16
#define ALIEN_HEIGHT 16
static const unsigned char PROGMEM alien_bmp[] = {
  0b00001000, 0b00010000,
  0b00000100, 0b00100000,
  0b00001111, 0b11110000,
  0b00011011, 0b11011000,
  0b00111111, 0b11111100,
  0b00101111, 0b11110100,
  0b00101000, 0b00010100,
  0b00000110, 0b01100000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

typedef enum {
NORMAL, CONFIG
} Estado;

Estado estado;

char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

const int pinBUp = 3;
const int pinBDown = 4;

enum CampoConfig {
  CFG_HORA,
  CFG_MINUTO,
  CFG_DIA,
  CFG_MES,
  CFG_ANO,
  CFG_SALVAR
};

CampoConfig campoAtual = CFG_HORA;

int cfgHora;
int cfgMinuto;
int cfgDia;
int cfgMes;
int cfgAno;

unsigned long ultimoCliqueConfig = 0;
const unsigned long TIMEOUT_CONFIG = 10000;
const unsigned long TEMPO_ENTRAR_CONFIG = 2000;

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    Serial.flush();
    abort();
  }
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // pega o horário do pc

  dht.begin(); // inicia o DHT
  delay(2000);

  // Limpa o buffer inicial da Adafruit (caso exiba o logo deles de início)
  display.clearDisplay();
  display.display();

  pinMode(pinBUp, INPUT_PULLUP); //agora entra no modo config quando os dois botões estão pressionados/LOW
  pinMode(pinBDown, INPUT_PULLUP);
  estado = NORMAL; // Inicia estado inicial como Normal
}

int segundoAnterior = -1;

void loop() {
  // tirar essa parte depois
  String estadoStr = "";
  if (estado == NORMAL) estadoStr = "Normal";
  else estadoStr = "Config";
  Serial.println("Estado do relogio: " + estadoStr);

  if (estado == NORMAL) {
    verificaEntradaConfig();
    rotinaNormal();
  } 
  else if (estado == CONFIG) {
    rotinaConfig();
  }
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
  DateTime now = rtc.now();
  
  if(now.second() != segundoAnterior) {
    segundoAnterior = now.second();
    
    // Atualizando a tela
    display.clearDisplay(); // Limpa a tela anterior para não sobrepor texto
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Escreve umidade no display
    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature(); //aqui ja leu a temperatura
    display.setCursor(2,48);  
    display.print("Umid: ");
    display.print(umidade, 1);
    display.print(" %");
    Serial.print("Umidade: ");
    Serial.println(umidade, 1);

    // Escreve temperatura no display
    display.setCursor(2,36);  
    display.print("Temp: ");
    display.print(temperatura, 1);
    display.print(" C");
    Serial.print("Temperatura: ");
    Serial.println(temperatura, 1);
    
    doTime(now);  
    
    // Escreve data no display 
    display.setCursor(2,0); 
    display.print("Data: ");
    if(now.day() < 10) display.print('0');
    display.print(now.day(), DEC);
    display.print('/');
    if(now.month() < 10) display.print('0'); 
    display.print(now.month(), DEC); 
    display.print('/');  
    display.print(now.year(), DEC);

    // Escreve dia da semana no display 
    display.setCursor(2,12); 
    display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    
    // Escreve hora no display
    display.setCursor(2,24);
    display.print("Hora: ");
    display.print(now.hour(), DEC);
    display.print(':');
    if(now.minute() < 10) display.print('0');
    display.print(now.minute(), DEC);
    display.print(':');
    if(now.second() < 10) display.print('0');
    display.print(now.second(), DEC);
    
    display.drawBitmap(100, 50, alien_bmp, ALIEN_WIDTH, ALIEN_HEIGHT, SSD1306_WHITE); //desenha o logo
    display.display(); 
  }
}

// Rotina do estado Config
void rotinaConfig(){  
  Serial.println("Entrei na rotina do config");  
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(4, 4);
  display.print("Modo CONFIG");
  display.display();

  // leitura dos botoes
  bool upPressionado = digitalRead(pinBUp);
  bool downPressionado = digitalRead(pinBDown);

  // se tem clique, mantem no config
  if (upPressionado || downPressionado) {
    ultimoCliqueConfig = millis();
  }

  // timeout para sair do config
  if (millis() - ultimoCliqueConfig > TIMEOUT_CONFIG) {
    estado = NORMAL;
    return;
  }
}

void verificaEntradaConfig() {
  static unsigned long inicioPressionado = 0;

  bool upPressionado = digitalRead(pinBUp);
  bool downPressionado = digitalRead(pinBDown);

  if (upPressionado || downPressionado) {
    if (inicioPressionado == 0) {
      inicioPressionado = millis();
    }

    if (millis() - inicioPressionado >= TEMPO_ENTRAR_CONFIG) {      
      Serial.println("Mudou de Estado: Normal -> Config");
      estado = CONFIG;
      //entrarConfig(); -> configuração dos valores base para configurar
      inicioPressionado = 0;
    }
  } else {
    inicioPressionado = 0;
  }
}