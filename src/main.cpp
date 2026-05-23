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

// Pinos dos botões
const int pinEsq = 12;
const int pinDir = 13;

// Estados do relógio
typedef enum {
NORMAL, CONFIG
} Estado;
Estado estado; // global para controlar o estado atual do relógio

// Dias da semana em Português
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

// Configurações para o modo CONFIG
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

//-------------------------------

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

  pinMode(pinEsq, INPUT);
  pinMode(pinDir, INPUT);

  estado = NORMAL; // Inicia estado inicial como Normal
}

void loop() {
  if (estado == NORMAL) {
    verificaEntradaConfig();
    rotinaNormal();
  } 
  else if (estado == CONFIG) {
    rotinaConfig();
  }
}

void rotinaNormal(){
  static int segundoAnterior = -1;
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

void rotinaConfig(){  
  /** Variáveis para evitar múltiplas leituras do mesmo clique, 
   * inicia true para evitar ação imediata ao entrar no config **/
  static bool esqAnterior = true;
  static bool dirAnterior = true;

  // leitura dos botoes
  bool esqPressionado = digitalRead(pinEsq);
  bool dirPressionado = digitalRead(pinDir);

  // se pressiona botão esquerdo incrementa o campo atual
  if (esqPressionado && !esqAnterior) {
    incrementarCampo();
    ultimoCliqueConfig = millis();
  }

  // se pressiona botão direito passa para o próximo campo
  if (dirPressionado && !dirAnterior) {
    proximoCampo();
    ultimoCliqueConfig = millis();
  }

  // timeout para sair do config
  if (millis() - ultimoCliqueConfig > TIMEOUT_CONFIG) {
    estado = NORMAL;
    return;
  }

  esqAnterior = esqPressionado;
  dirAnterior = dirPressionado;

  desenharTelaConfig();
  
  Serial.print("Tempo até sair do config: ");
  Serial.println(TIMEOUT_CONFIG - millis() + ultimoCliqueConfig);
}

void verificaEntradaConfig() {
  static unsigned long inicioPressionado = 0;

  bool esqPressionado = digitalRead(pinEsq);
  bool dirPressionado = digitalRead(pinDir);

  if (esqPressionado || dirPressionado) {
    if (inicioPressionado == 0) {
      inicioPressionado = millis();
    }

    if (millis() - inicioPressionado >= TEMPO_ENTRAR_CONFIG) {
      entrarConfig();
      inicioPressionado = 0;
    }
  } else {
    inicioPressionado = 0;
  }
}

// Inicializa as variáveis de configuração com o horário atual do RTC e entra no modo CONFIG
void entrarConfig() {
  DateTime now = rtc.now();

  cfgHora = now.hour();
  cfgMinuto = now.minute();
  cfgDia = now.day();
  cfgMes = now.month();
  cfgAno = now.year();

  campoAtual = CFG_HORA;
  ultimoCliqueConfig = millis();
  estado = CONFIG;
}

// -------------------------------
// Funções CONFIG
void proximoCampo() {
  campoAtual = (CampoConfig)(campoAtual + 1);

  if (campoAtual > CFG_SALVAR) {
    campoAtual = CFG_HORA;
  }
}

void incrementarCampo() {
  switch (campoAtual) {
    case CFG_HORA:
      cfgHora = (cfgHora + 1) % 24;
      break;

    case CFG_MINUTO:
      cfgMinuto = (cfgMinuto + 1) % 60;
      break;

    case CFG_DIA:
      cfgDia++;
      if (cfgDia > 31) cfgDia = 1;
      break;

    case CFG_MES:
      cfgMes++;
      if (cfgMes > 12) cfgMes = 1;
      break;

    case CFG_ANO:
      cfgAno++;
      if (cfgAno > 2035) cfgAno = 2024;
      break;

    case CFG_SALVAR:
      salvarConfig();
      estado = NORMAL;
      break;
  }
}

void salvarConfig() {
  rtc.adjust(DateTime(cfgAno, cfgMes, cfgDia, cfgHora, cfgMinuto, 0));
}

void desenharTelaConfig() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(4, 4);
  display.print("CONFIGURAR");

  display.setTextSize(2);
  display.setCursor(16, 22);

  if (cfgHora < 10) display.print('0');
  display.print(cfgHora);
  display.print(':');

  if (cfgMinuto < 10) display.print('0');
  display.print(cfgMinuto);

  display.setTextSize(1);
  display.setCursor(20, 42);

  if (cfgDia < 10) display.print('0');
  display.print(cfgDia);
  display.print('/');

  if (cfgMes < 10) display.print('0');
  display.print(cfgMes);
  display.print('/');

  display.print(cfgAno);

  // Indicador do campo
  display.setCursor(4, 54);

  switch (campoAtual) {
    case CFG_HORA:
      display.print("Editando: hora");
      break;
    case CFG_MINUTO:
      display.print("Editando: minuto");
      break;
    case CFG_DIA:
      display.print("Editando: dia");
      break;
    case CFG_MES:
      display.print("Editando: mes");
      break;
    case CFG_ANO:
      display.print("Editando: ano");
      break;
    case CFG_SALVAR:
      display.print("UP para salvar");
      break;
  }
  
  // Ícone
  display.drawBitmap(108, 3, alien_bmp, ALIEN_WIDTH, ALIEN_HEIGHT, SSD1306_WHITE);

  display.display();
}