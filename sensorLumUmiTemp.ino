#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <DHT.h>
 
#define LDR_PIN A0
#define DHTPIN 2
#define DHTTYPE DHT11
#define BUTTON_NEXT 7
#define BUTTON_SELECT 4
#define LED_VERDE 8
#define LED_AMARELO 9
#define LED_VERMELHO 10
#define BUZZER 11
#define UTC_OFFSET -3
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
 
int tipoVinho = 0;
const char* opcoes[] = {"Tinto", "Branco", "Ambos"};
 
 
bool buzzerAtivo = false;
unsigned long buzzerInicio = 0;
bool buzzerLigado = false;
 
int luzMin = -1;
int luzMax = -1;
 
bool mostrarIndices = false;
 
const int tamanhoBuffer = 5;
int bufferLDR[tamanhoBuffer] = {0};
int indiceBuffer = 0;
bool bufferCompleto = false;
 
int lastLoggedMinute = -1;
 
struct Registro {
  byte dia;
  byte mes;
  byte hora;
  byte minuto;
  float temp;
  float umid;
  float lum;
  byte estado; // alerta ou crítico
};
 
int endereco = 0;
 
const int maxRegistros = EEPROM.length() / sizeof(Registro); // 128
int inicioFila = 0;
int totalRegistros = 0;
 
Registro ultimoRegistro;
bool existeUltimo = false;
 
// Animação do cavalo
byte framesCavalo[2][8][8] = {
  {
    {0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x03, 0x07, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x05, 0x1F, 0x1D, 0x1F, 0x16, 0x06},
    {0x0C, 0x18, 0x10, 0x00, 0x01, 0x01, 0x01, 0x00},
    {0x1F, 0x1F, 0x1E, 0x17, 0x00, 0x00, 0x10, 0x00},
    {0x1F, 0x1F, 0x03, 0x02, 0x14, 0x04, 0x02, 0x00},
    {0x1C, 0x1C, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00}
  },
  {
    {0x00, 0x00, 0x00, 0x07, 0x0F, 0x0E, 0x1C, 0x18},
    {0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x01, 0x03, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x14, 0x1C, 0x1A, 0x1E, 0x1F, 0x13, 0x10, 0x10},
    {0x13, 0x13, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00},
    {0x1F, 0x07, 0x0E, 0x06, 0x01, 0x00, 0x00, 0x00},
    {0x0F, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00},
    {0x10, 0x18, 0x0C, 0x02, 0x02, 0x11, 0x00, 0x00}
  }
};
 
 
 
void escolherTipoVinho();
void calibrarLuminosidade();
void adicionarAoBuffer(int leitura);
float calcularMediaBuffer();
int avaliarAmbiente(float temp, float umid, float lum);
void atualizarDisplay(int estado, float temp, float umid, float lum);
void controlarLEDs(int estado);
void controlarBuzzer(int estado);
void salvarRegistroEEPROM(float temp, float umid, float lum);
void imprimirRegistrosEEPROM();
 
void animacaoCavalo() {
  for (int pos = -3; pos <= 13; pos++) {
    int f = pos % 2;
    if (f < 0) f += 2;
 
    lcd.clear();
    for (int i = 0; i < 8; i++) {
      lcd.createChar(i, framesCavalo[f][i]);
    }
    for (int c = 0; c < 4; c++) {
      int x = pos + c;
      if (x >= 0 && x < 16) {
        lcd.setCursor(x, 0); lcd.write(byte(c));
        lcd.setCursor(x, 1); lcd.write(byte(c + 4));
      }
    }
    delay(300);
  }
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Boas Vindas     ");
  lcd.setCursor(0, 1);
  lcd.print("Cyber Horses    ");
  delay(3000);
  lcd.clear();
}
 
 
 
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  dht.begin();
  lcd.begin(16, 2);
  lcd.backlight();
 
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
 
  animacaoCavalo();
 
  lcd.setCursor(0, 0);
  lcd.print("Selecione o tipo");
  lcd.setCursor(0, 1);
  lcd.print("de vinho");
  delay(5000);
 
  escolherTipoVinho();
  calibrarLuminosidade();
  lcd.clear();
}
 
void loop() {
  DateTime now = rtc.now();
  int offsetSeconds = UTC_OFFSET * 3600;
  now = now.unixtime() + offsetSeconds;
  DateTime adjustedTime = DateTime(now);
 
  // Removido: get_log()
 
  if (adjustedTime.minute() != lastLoggedMinute) {
    lastLoggedMinute = adjustedTime.minute();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
 
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
 
  int leituraLDR = analogRead(LDR_PIN);
  adicionarAoBuffer(leituraLDR);
  float mediaLDR = calcularMediaBuffer();
 
  float luminosidade = map(mediaLDR, luzMin, luzMax, 100, 0);
  luminosidade = constrain(luminosidade, 100, 0);
 
  int estadoAmbiente = avaliarAmbiente(temperatura, umidade, luminosidade);
 
  if (estadoAmbiente > 0 && digitalRead(BUTTON_SELECT) == LOW) {
    mostrarIndices = !mostrarIndices;
    delay(300);
  }
 
  atualizarDisplay(estadoAmbiente, temperatura, umidade, luminosidade);
  controlarLEDs(estadoAmbiente);
  controlarBuzzer(estadoAmbiente);
  if (estadoAmbiente > 0) {  // só registra se alerta ou crítico
  salvarRegistroEEPROM(temperatura, umidade, luminosidade);
}
 
}
 
 
void adicionarAoBuffer(int leitura) {
  bufferLDR[indiceBuffer] = leitura;
  indiceBuffer++;
  if (indiceBuffer >= tamanhoBuffer) {
    indiceBuffer = 0;
    bufferCompleto = true;
  }
}
 
float calcularMediaBuffer() {
  int soma = 0;
  int quantidade = bufferCompleto ? tamanhoBuffer : indiceBuffer;
  if (quantidade == 0) return analogRead(LDR_PIN);
  for (int i = 0; i < quantidade; i++) {
    soma += bufferLDR[i];
  }
  return (float)soma / quantidade;
}
 
void escolherTipoVinho() {
  int opcao = 0;
  bool selecionado = false;
  bool ultimoEstadoNext = HIGH;
  bool ultimoEstadoSelect = HIGH;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Escolha o vinho:");
  lcd.setCursor(0, 1);
  lcd.print(opcoes[opcao]);
 
  while (!selecionado) {
    bool estadoNext = digitalRead(BUTTON_NEXT);
    bool estadoSelect = digitalRead(BUTTON_SELECT);
 
    if (estadoNext == LOW && ultimoEstadoNext == HIGH) {
      opcao = (opcao + 1) % 3;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(opcoes[opcao]);
      delay(300);
    }
 
    if (estadoSelect == LOW && ultimoEstadoSelect == HIGH) {
      tipoVinho = opcao;
      selecionado = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selecionado:");
      lcd.setCursor(0, 1);
      lcd.print(opcoes[tipoVinho]);
      delay(1000);
    }
 
    ultimoEstadoNext = estadoNext;
    ultimoEstadoSelect = estadoSelect;
  }
}
 
void calibrarLuminosidade() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aperte p/ calib.");
  lcd.setCursor(0, 1);
  lcd.print("luz minima");
 
  while (digitalRead(BUTTON_SELECT) == HIGH);
  delay(300);
  luzMin = analogRead(LDR_PIN);
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Minima definida");
  delay(1000);
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aperte p/ calib.");
  lcd.setCursor(0, 1);
  lcd.print("luz maxima");
 
  while (digitalRead(BUTTON_SELECT) == HIGH);
  delay(300);
  luzMax = analogRead(LDR_PIN);
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Maxima definida");
  delay(1000);
}
 
int avaliarAmbiente(float temp, float umid, float lum) {
  int estadoTemp = 0, estadoUmid = 0, estadoLum = 0;
 
  if (tipoVinho == 0) {
    if (temp >= 10 && temp <= 16) estadoTemp = 0;
    else if ((temp >= 9 && temp < 10) || (temp > 16 && temp <= 17)) estadoTemp = 1;
    else estadoTemp = 2;
  } else if (tipoVinho == 1) {
    if (temp >= 8 && temp <= 12) estadoTemp = 0;
    else if ((temp >= 7 && temp < 8) || (temp > 12 && temp <= 13)) estadoTemp = 1;
    else estadoTemp = 2;
  } else {
    if (temp >= 10 && temp <= 12) estadoTemp = 0;
    else if ((temp >= 9 && temp < 10) || (temp > 12 && temp <= 13)) estadoTemp = 1;
    else estadoTemp = 2;
  }
 
  if (umid >= 60 && umid <= 80) estadoUmid = 0;
  else if ((umid >= 50 && umid < 60) || (umid > 80 && umid <= 90)) estadoUmid = 1;
  else estadoUmid = 2;
 
  if (lum <= 30) estadoLum = 0;
  else if (lum <= 50) estadoLum = 1;
  else estadoLum = 2;
 
  if (estadoTemp == 2 || estadoUmid == 2 || estadoLum == 2) return 2;
  if (estadoTemp == 1 || estadoUmid == 1 || estadoLum == 1) return 1;
  return 0;
}
 
void atualizarDisplay(int estado, float temp, float umid, float lum) {
  static bool alternar = false;
  static int ultimoEstado = -1;
  static bool ultimaMostraIndice = false;
  static unsigned long ultimoUpdate = 0;
 
  // Só atualiza o display a cada 1000 ms
  if (millis() - ultimoUpdate < 1000 && estado == ultimoEstado && mostrarIndices == ultimaMostraIndice) return;
 
  ultimoUpdate = millis();
  ultimoEstado = estado;
  ultimaMostraIndice = mostrarIndices;
 
  lcd.clear();
 
  if (estado == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Ambiente estavel");
    lcd.setCursor(0, 1);
    lcd.print(":)");
  } else if (mostrarIndices) {
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print((int)temp); lcd.print((char)223); lcd.print("C");
    lcd.print(" U:"); lcd.print((int)umid); lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("L:"); lcd.print((int)lum); lcd.print("%");
  } else {
    alternar = !alternar;
    if (alternar) {
      lcd.setCursor(0, 0);
      lcd.print(estado == 1 ? "Status: alerta" : "Ambiente CRITICO!");
      lcd.setCursor(0, 1);
      lcd.print(estado == 1 ? ":|" : ":(");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Pressione p/ ver");
      lcd.setCursor(0, 1);
      lcd.print("os indices");
    }
  }
}
 
void controlarLEDs(int estado) {
  digitalWrite(LED_VERDE, estado == 0);
  digitalWrite(LED_AMARELO, estado == 1);
  digitalWrite(LED_VERMELHO, estado == 2);
}
 
void controlarBuzzer(int estado) {
  int frequencia = 0;
  if (estado == 1) {
    frequencia = 1000;
  } else if (estado == 2) {
    frequencia = 2000;
  }
 
  if (estado == 1 || estado == 2) {
    if (!buzzerAtivo) {
      buzzerAtivo = true;
      buzzerInicio = millis();
      buzzerLigado = true;
      tone(BUZZER, frequencia);
    } else {
      unsigned long tempoAtual = millis();
      if (buzzerLigado && tempoAtual - buzzerInicio >= 3000) {
        noTone(BUZZER);
        buzzerLigado = false;
        buzzerInicio = tempoAtual;
      } else if (!buzzerLigado && tempoAtual - buzzerInicio >= 1000) {
        tone(BUZZER, frequencia);
        buzzerLigado = true;
        buzzerInicio = tempoAtual;
      }
    }
  } else {
    buzzerAtivo = false;
    buzzerLigado = false;
    noTone(BUZZER);
  }
}
 
void salvarRegistroEEPROM(float temp, float umid, float lum) {
  DateTime agora = rtc.now();
 
  Registro novo;
  novo.dia = agora.day();
  novo.mes = agora.month();
  novo.hora = agora.hour();
  novo.minuto = agora.minute();
  novo.temp = temp;
  novo.umid = umid;
  novo.lum = lum;
  novo.estado = avaliarAmbiente(temp, umid, lum);
 
  // Verifica se já existe e se é igual ao último (evita repetição)
  if (existeUltimo &&
      novo.temp == ultimoRegistro.temp &&
      novo.umid == ultimoRegistro.umid &&
      novo.lum == ultimoRegistro.lum &&
      novo.estado == ultimoRegistro.estado) {
    return;  // Não salva registro duplicado
  }
 
  // Salva novo registro na posição correta
  int posicao;
  if (totalRegistros < maxRegistros) {
    posicao = totalRegistros;
    totalRegistros++;
  } else {
    // FIFO: sobrescreve o mais antigo
    posicao = inicioFila;
    inicioFila = (inicioFila + 1) % maxRegistros;
  }
 
  int enderecoEscrita = posicao * sizeof(Registro);
  EEPROM.put(enderecoEscrita, novo);
  ultimoRegistro = novo;
  existeUltimo = true;
 
  // Imprime novo registro
  Serial.print("NOVO ALERTA SALVO: ");
  Serial.print(novo.dia); Serial.print("/");
  Serial.print(novo.mes); Serial.print(" ");
  Serial.print(novo.hora); Serial.print(":");
  Serial.print(novo.minuto); Serial.print(" | T:");
  Serial.print(novo.temp); Serial.print("°C U:");
  Serial.print(novo.umid); Serial.print("% L:");
  Serial.print(novo.lum); Serial.print("% | Estado: ");
  Serial.println(novo.estado == 1 ? "Alerta" : "Critico");
}
 
 
void imprimirRegistrosEEPROM() {
  Registro reg;
  Serial.println("---- Registros EEPROM ----");
 
  for (int i = 0; i < totalRegistros; i++) {
    int pos = (inicioFila + i) % maxRegistros;
    int enderecoLeitura = pos * sizeof(Registro);
    EEPROM.get(enderecoLeitura, reg);
 
    Serial.print("Data: ");
    Serial.print(reg.dia);
    Serial.print("/");
    Serial.print(reg.mes);
    Serial.print(" - ");
    Serial.print(reg.hora);
    Serial.print(":");
    Serial.print(reg.minuto);
 
    Serial.print(" | Temp: ");
    Serial.print(reg.temp); Serial.print("°C");
    Serial.print(" | Umid: ");
    Serial.print(reg.umid); Serial.print("%");
    Serial.print(" | Lum: ");
    Serial.print(reg.lum); Serial.print("%");
 
 
    Serial.print(" | Estado: ");
    Serial.println(reg.estado == 1 ? "Alerta" : "Critico");
  }
 
  Serial.println("---------------------------");
}
