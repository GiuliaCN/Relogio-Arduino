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

    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature();

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Borda
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

    // Topo: dia da semana + data
    display.setTextSize(1);
    display.setCursor(4, 4);
    display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    display.print(" ");

    if(now.day() < 10) display.print('0');
    display.print(now.day());
    display.print('/');

    if(now.month() < 10) display.print('0');
    display.print(now.month());
    display.print('/');

    display.print(now.year());

    // Ícone
    display.drawBitmap(108, 3, alien_bmp, ALIEN_WIDTH, ALIEN_HEIGHT, SSD1306_WHITE);

    // Hora grande
    display.setTextSize(2);
    display.setCursor(16, 25);

    if(now.hour() < 10) display.print('0');
    display.print(now.hour());
    display.print(':');

    if(now.minute() < 10) display.print('0');
    display.print(now.minute());
    display.print(':');

    if(now.second() < 10) display.print('0');
    display.print(now.second());

    // Rodapé: temperatura e umidade
    display.setTextSize(1);

    display.setCursor(5, 53);
    display.print(temperatura, 1);
    display.print(" C");

    display.setCursor(82, 53);
    display.print(umidade, 1);
    display.print(" %");

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