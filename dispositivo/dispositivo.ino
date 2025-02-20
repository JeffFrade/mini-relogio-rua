// Usar ESP32 (ESP32-WROOM-DA Module)

#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include <WiFi.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi
const char* SSID = "Jeffin"; // rede wifi
const char* PASSWORD = "31643493"; // senha da rede wifi
const char* HOSTNAME = "relogio_rua_abc";

// NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;

// gmtOffset_sec:
// -7200 => Horário de Fernando de Noronha (-2)
// -10800 => Horário de Brasília (-3)
// -14400 => Horário do Amazonas (-4)
// -18000 => Horário do Acre (-5)
// Como calcular o valor de gmtOffset_sec = 3600 * fuso_horario (Ex: Brasília -3).

// Horário de verão:
const int daylightOffset_sec = 0;

// Caso ativo: 3600
// Caso inativo: 0

// DHT
#define dhtType DHT22
// Caso use DHT11:
// #define dhtType DHT11
#define dhtPin 23

// OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C

void initWiFi();
void printLocalTime();
void displayHeader(int pos);

DHT dht(dhtPin, dhtType);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String header = "Teste";

void setup() {
  Serial.begin(9600);

  while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("Erro ao inicializar display"));
  }

  initWiFi();
  dht.begin();
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

}

void loop() {
  printLocalTime();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  displayHeader(32);
  display.setTextSize(1);
  display.println("Temperatura:");
  display.setTextSize(2);
  display.println(String(temperature, 1) + " C");

  display.setTextSize(1);
  display.println("Umidade:");
  display.setTextSize(2);
  display.println(String(humidity, 2) + "%");
  display.display();

  Serial.println("Hora: " + header);
  Serial.println("Temperatura: " + String(temperature, 1) + " C");
  Serial.println("Umidade: " + String(humidity, 2) + "%");
  Serial.println("--------------------------");

  delay(1000);
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));
 
  WiFi.setHostname(HOSTNAME);
  
  WiFi.begin(SSID, PASSWORD);

  displayHeader(32);
  display.setTextSize(1);
  display.println("Conectando Wi-Fi");
  display.display();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");

    displayHeader(32);
    display.setTextSize(1);
    display.println("Conectando...");
    display.display();
  }

  displayHeader(32);
  display.setTextSize(1);
  display.println("Conectado!");
  display.display();

  delay(2000);
  
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Falha ao obter horário do NTP");

    displayHeader(32);
    display.setTextSize(1);
    display.println("Falha ao obter horário do NTP");
    display.display();

    delay(2000);

    ESP.restart();
  }
  
  char timeStringBuff[6];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
  header = timeStringBuff;
}

void displayHeader(int pos) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(pos, 0);
  display.setTextSize(2);
  display.println(header);
}
