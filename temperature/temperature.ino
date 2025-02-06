#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <espnow.h>

// Настройки точки доступа
const char* ssid = "WiFiName";
const char* password = "WiFiPassword";

// Пины подключения
#define ONE_WIRE_BUS 2  // GPIO2 для DS18B20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ESP8266WebServer server(80);

// Пороговые значения температуры
float tempThresholdHigh = 25.0;
float tempThresholdLow = 20.0;
float currentTemp = 22;

// MAC-адрес модуля с реле
uint8_t relayMacAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

void setup() {
  Serial.begin(115200);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к Wi-Fi...");
  }
  Serial.println("Подключено к Wi-Fi");
  Serial.print("IP-адрес: ");
  Serial.println(WiFi.localIP());

  // Инициализация датчика температуры
  sensors.begin();

  // Настройка веб-сервера
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("Веб-сервер запущен.");

  // Инициализация ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Ошибка инициализации ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(relayMacAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

struct struct_message {
  float currentTemp;
  float tempThresholdHigh;
  float tempThresholdLow;
};

void loop() {
  server.handleClient();

  // Измерение температуры
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);
  Serial.print("Текущая температура: ");
  Serial.println(currentTemp);

  // Заполнение структуры данными
  struct_message dataToSend;
  dataToSend.currentTemp = currentTemp;
  dataToSend.tempThresholdHigh = tempThresholdHigh;
  dataToSend.tempThresholdLow = tempThresholdLow;

  // Отправка структуры на модуль с реле
  esp_now_send(relayMacAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));

  delay(5000);  // Измерение каждые 5 секунд
}

void handleRoot() {
  String html = "<html lang='ru-RU'><head><meta charset='UTF-8'></head><body>";
  html += "<h1>Текущая температура: " + String(currentTemp) + "С</h1>";
  html += "<h1>Настройка пороговых температур</h1>";
  html += "<form action=\"/set\" method=\"POST\">";
  html += "Верхний порог: <input type=\"text\" name=\"high\" value=\"" + String(tempThresholdHigh) + "\"><br>";
  html += "Нижний порог: <input type=\"text\" name=\"low\" value=\"" + String(tempThresholdLow) + "\"><br>";
  html += "<input type=\"submit\" value=\"Сохранить\">";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("high")) {
    tempThresholdHigh = server.arg("high").toFloat();
  }
  if (server.hasArg("low")) {
    tempThresholdLow = server.arg("low").toFloat();
  }
  server.send(200, "text/html", "<html lang='ru-RU'><head><meta charset='UTF-8'></head><body><h1>Настройки сохранены</h1><a href=\"/\">Назад</a></body></html>");
}