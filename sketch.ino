#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Webhook
const char* serverUrl = "https://webhook.site/5f06aca3-38b4-42a4-a918-8fd798a50b8b";

// sensor
const int sensorPin = 34; // Pino analógico
const int adcResolution = 4095; // Resolução do ADC (ESP32 usa 12 bits)
const float maxCurrent = 30.0; // Corrente máxima simulada (em Amperes)

//  Wi-Fi
void conectaWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

//  simula leitura de corrente
float lerSensor() {
  int valorADC = analogRead(sensorPin);
  float corrente = (valorADC / (float)adcResolution) * maxCurrent; // Simula uma corrente de 0 a 30A
  return corrente;
}

// envia dados
void enviarDados(float potencia, float consumo, float consumoEstimado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

   
    String jsonPayload = "{";
    jsonPayload += "\"potencia\": \"" + String(potencia, 2) + " W\", ";
    jsonPayload += "\"consumo_instantaneo\": \"" + String(consumo, 4) + " kWh\", ";
    jsonPayload += "\"consumo_estimado_mensal\": \"" + String(consumoEstimado, 2) + " kWh\"";
    jsonPayload += "}";

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // POST
    int httpResponseCode = http.POST(jsonPayload);

    // resposta
    if (httpResponseCode > 0) {
      Serial.print("Resposta do servidor: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Erro ao enviar os dados. Código HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wi-Fi desconectado!");
  }
}

void setup() {
  
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  conectaWiFi();
}

void loop() {
  
  float corrente = lerSensor();
  float potencia = corrente * 220.0; // Simula potência em 220V
  float consumo = (potencia * 10) / 3600.0; // Consumo em kWh (10 segundos de intervalo)
  float consumoEstimado = consumo * 720.0; // Consumo mensal estimado (30 dias)

  // Exibição
  Serial.print("Potência: ");
  Serial.print(potencia);
  Serial.print(" W | Consumo: ");
  Serial.print(consumo);
  Serial.print(" kWh | Previsão: ");
  Serial.print(consumoEstimado);
  Serial.println(" kWh");

  // servidor
  enviarDados(potencia, consumo, consumoEstimado);

  // Leituras constantes
  delay(10000);
}
