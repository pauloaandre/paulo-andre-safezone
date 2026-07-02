#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// PINOS
#define PIR_SENSOR 15
#define TRIG_PIN 18
#define ECHO_PIN 19
#define LED_VERMELHO 5
#define LED_VERDE 4
#define LED_AZUL 21
#define BUZZER_PIN 27
#define RELE_PIN 2
#define BOTAO_STOP 23
#define RESET_BUTTON 22

// CONFIGURAÇÕES WIFI / MQTT
const char* ssid = "M52 de paulo"; //nome da rede wifi
const char* password = "12345678"; //senha da rede wifi
const char* broker_mqtt = "broker.hivemq.com";
const int porta_mqtt = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


bool maquinaBloqueada = false;
unsigned long lastPublish = 0;

enum EstadoSistema {
  SEGURO,
  ALERTA,
  PERIGO,
  PARADO
};

EstadoSistema estadoAtual = SEGURO;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Comando recebido [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (msg == "reset") {
    maquinaBloqueada = false;
    estadoAtual = SEGURO;
    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
    client.publish("safezone/evento", "Reset remoto via Dashboard");
  } 
  else if (msg == "stop") {
    maquinaBloqueada = true;
    estadoAtual = PARADO;
    digitalWrite(RELE_PIN, HIGH);
    client.publish("safezone/evento", "PARADA remota via Dashboard");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32SafeZone-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      client.subscribe("safezone/comando");
      client.publish("safezone/evento", "ESP32 Conectada");
      publicarDados(medirDistancia(), digitalRead(PIR_SENSOR));
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void publicarDados(uint32_t distancia, bool movimento) {
  String statusStr = "";
  switch (estadoAtual) {
    case SEGURO: statusStr = "SEGURO"; break;
    case ALERTA: statusStr = "ALERTA"; break;
    case PERIGO: statusStr = "PERIGO"; break;
    case PARADO: statusStr = "PARADO"; break;
  }

  client.publish("safezone/status", statusStr.c_str(), true);
  client.publish("safezone/distancia", String(distancia).c_str(), true);
  client.publish("safezone/movimento", movimento ? "SIM" : "NAO", true);
}

// LED RGB
void ledSeguro() {
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AZUL, LOW);
}

void ledAlerta() {
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AZUL, LOW);
}

void ledPerigo() {
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);
}

// DISTÂNCIA
uint32_t medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracao == 0) return 999;
  return duracao / 58;
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(broker_mqtt, porta_mqtt);
  client.setCallback(callback);

  pinMode(PIR_SENSOR, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);
  pinMode(BOTAO_STOP, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  digitalWrite(RELE_PIN, LOW);
  ledSeguro();

  Serial.println("Sistema iniciado");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // BOTÕES FÍSICOS
  if (!digitalRead(BOTAO_STOP)) {
    maquinaBloqueada = true;
    estadoAtual = PARADO;
    digitalWrite(RELE_PIN, HIGH);
    client.publish("safezone/evento", "Botão STOP físico pressionado");
    delay(300);
  }

  if (!digitalRead(RESET_BUTTON)) {
    maquinaBloqueada = false;
    estadoAtual = SEGURO;
    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
    client.publish("safezone/evento", "Botão RESET físico pressionado");
    delay(300);
  }

  if (maquinaBloqueada) {
    if (millis() - lastPublish > 2000) {
      publicarDados(medirDistancia(), digitalRead(PIR_SENSOR));
      lastPublish = millis();
    }
    return;
  }

  bool movimento = digitalRead(PIR_SENSOR);
  uint32_t distancia = medirDistancia();

  if (!movimento) {
    estadoAtual = SEGURO;
    ledSeguro();
    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
  } else {
    if (distancia <= 15) {
      estadoAtual = PERIGO;
      maquinaBloqueada = true;
      digitalWrite(RELE_PIN, HIGH);
      ledPerigo();
      for (int i = 0; i < 5; i++) {
        tone(BUZZER_PIN, 1000);
        delay(200);

        noTone(BUZZER_PIN);
        delay(200);
      }
      client.publish("safezone/evento", "Invasão detectada - MÁQUINA BLOQUEADA");
    } else if (distancia <= 25) {
      estadoAtual = ALERTA;
      ledAlerta();
    } else {
      estadoAtual = SEGURO;
      ledSeguro();
      digitalWrite(RELE_PIN, LOW);
      noTone(BUZZER_PIN);
    }
  }

  // Publicação periódica (independente de movimento)
  if (millis() - lastPublish > 2000) {
    publicarDados(distancia, movimento);
    lastPublish = millis();
  }

  delay(100);
}
