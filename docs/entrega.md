# Sobre a entrega do projeto
Projeto Arduino
Interface básica de usuário
período: de 30/4 a 28/5

## Apresentação
O objetivo do Projeto Arduino é conhecer a plataforma de prototipagem visando sua
aplicação em projetos de inovação tecnológica.
Será montada uma interface de usuário utilizando um relógio de tempo real e um sensor de
temperatura. Essa interface pode servir como base para o desenvolvimento de projetos para
IoT.
Circuito
O circuito será montado utilizando um kit de desenvolvimento Arduino Uno e os módulos RTC
DS3231, display SSD1306 e o sensor de temperatura e humidade DTH111. Deverá incorporar
também botões que permitam o ajuste da hora, sem a necessidade de estar conectado ao
notebook.

## Material necessário:
1 kit Arduino Uno
1 relógio de tempo real RTC DS3231
1 display OLED SSD1306 0.96” (128 X 64 pixels)
1 sensor de temperatura e humidade DHT11 ou DHT22
1 protoboard 400 pinos
20 jumpers macho-macho de 20 cm
2 push-button (botões de pressionar)

## Requisitos

**Formato do display**
O display deverá fornecer as seguintes informações:
• Hora – no formato 24h (hh:mm:ss)
• Dia da semana
• Data – no formato dd/mm/aaaa
• Temperatura em °C e humidade em %
• O logo da equipe

**Botões de ajuste**
O projeto deve conter dois botões (push button) para fazer o ajuste manual do relógio,
possibilitando o uso sem o notebook.

## Bibliotecas utilizadas
Adafruit_GFX.h
Biblioteca para símbolos e gráficos.
Adafruit_SSD1306.h
Biblioteca para utilizar o display SSD1306
https://blog.eletrogate.com/guia-completo-do-display-oled-parte-1-o-que-e-como-funciona-2/
https://blog.eletrogate.com/guia-completo-do-display-oled-parte-2-como-programar-3/
RTClib.h
Biblioteca para utilizar o RTC DS3231
https://portal.vidadesilicio.com.br/real-time-clock-rtc-ds3231/
Wire.h
Biblioteca utilizada para inicializar a comunicação I2C
https://capsistema.com.br/index.php/2020/11/27/como-usar-i2c-no-arduino-comunicacao-entre-duas-placas-arduino/
DHT sensor library de Adafruit para o DHT22 - https://docs.arduino.cc/libraries/dht-sensor-library/

## Referências
Livro sobre programação do Arduino
McRoberts, Michael; Arduino básico; [trad. Rafael Zanolli]. São Paulo: Novatec Editora, 2011.
Programação do arduino
https://www.electrofun.pt/blog/curso-arduino-0-introducao/
TinkerCad
https://www.tinkercad.com/
Vídeo sobre TinkerCad
https://youtu.be/j7ePCEKYKyQ
Simulador Arduino
https://wokwi.com/
