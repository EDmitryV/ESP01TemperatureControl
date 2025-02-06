#include <ESP8266WiFi.h>
#include <espnow.h>

#define RELAY_PIN 0  // GPIO0 для управления реле

bool relayState = false;         // Текущее состояние реле
unsigned long lastSwitchTime = 0; // Время последнего переключения реле

void setup() {
  Serial.begin(115200);

  // Инициализация пина реле
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Реле выключено по умолчанию (активный LOW)

  // Инициализация ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Ошибка инициализации ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Основной цикл пуст, вся работа выполняется в обработчике приема данных
}

struct struct_message {
  float currentTemp;
  float tempThresholdHigh;
  float tempThresholdLow;
};

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  struct_message receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Получена температура: ");
  Serial.println(receivedData.currentTemp);
  Serial.print("Верхний порог: ");
  Serial.println(receivedData.tempThresholdHigh);
  Serial.print("Нижний порог: ");
  Serial.println(receivedData.tempThresholdLow);

  unsigned long currentTime = millis();
  
    if (receivedData.currentTemp < receivedData.tempThresholdLow && !relayState) {
      digitalWrite(RELAY_PIN, LOW);  // Включить реле (активный LOW)
      relayState = true;
      lastSwitchTime = currentTime;
      Serial.println("Реле включено");
    } else if (receivedData.currentTemp >= receivedData.tempThresholdHigh && relayState) {
      digitalWrite(RELAY_PIN, HIGH); // Выключить реле (активный LOW)
      relayState = false;
      lastSwitchTime = currentTime;
      Serial.println("Реле выключено");
    }
  
}
 
