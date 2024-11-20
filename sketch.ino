#include <WiFi.h>
#include <HTTPClient.h>

// Configuração do Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// URL do servidor Flask ou Webhook
const char* serverUrl = "https://webhook.site/5f06aca3-38b4-42a4-a918-8fd798a50b8b";

// Configuração do sensor
const int sensorPin = 34; // Pino analógico
const int adcResolution = 4095; // Resolução do ADC (ESP32 usa 12 bits)
const float maxCurrent = 30.0; // Corrente máxima simulada (em Amperes)

// Função para conectar ao Wi-Fi
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

// Função para ler o sensor (simula leitura de corrente)
float lerSensor() {
  int valorADC = analogRead(sensorPin);
  float corrente = (valorADC / (float)adcResolution) * maxCurrent; // Simula uma corrente de 0 a 30A
  return corrente;
}

// Função para enviar dados ao servidor
void enviarDados(float potencia, float consumo, float consumoEstimado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Prepara os dados em formato JSON
    String jsonPayload = "{";
    jsonPayload += "\"potencia\": \"" + String(potencia, 2) + " W\", ";
    jsonPayload += "\"consumo_instantaneo\": \"" + String(consumo, 4) + " kWh\", ";
    jsonPayload += "\"consumo_estimado_mensal\": \"" + String(consumoEstimado, 2) + " kWh\"";
    jsonPayload += "}";

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Envia o POST
    int httpResponseCode = http.POST(jsonPayload);

    // Exibe a resposta do servidor
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
  // Inicialização
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  conectaWiFi();
}

void loop() {
  // Leitura e cálculos
  float corrente = lerSensor();
  float potencia = corrente * 220.0; // Simula potência em 220V
  float consumo = (potencia * 10) / 3600.0; // Consumo em kWh (10 segundos de intervalo)
  float consumoEstimado = consumo * 720.0; // Consumo mensal estimado (30 dias)

  // Exibe no console
  Serial.print("Potência: ");
  Serial.print(potencia);
  Serial.print(" W | Consumo: ");
  Serial.print(consumo);
  Serial.print(" kWh | Previsão: ");
  Serial.print(consumoEstimado);
  Serial.println(" kWh");

  // Envia os dados para o servidor
  enviarDados(potencia, consumo, consumoEstimado);

  // Aguarda 10 segundos antes da próxima leitura
  delay(10000);
}
