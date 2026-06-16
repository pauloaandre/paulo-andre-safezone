#include <Arduino.h>

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


bool maquinaBloqueada = false;

enum EstadoSistema {
  SEGURO,
  ALERTA,
  PERIGO,
  EMERGENCIA
};

EstadoSistema estadoAtual = SEGURO;

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

  if (duracao == 0) {
    Serial.println("[ULTRASSOM] Sem eco recebido!");
    return 999;
  }

  Serial.print("[ULTRASSOM] Duracao: ");
  Serial.print(duracao);
  Serial.println(" us");

  return duracao / 58;
}


void mostrarEstado() {
  Serial.print("[ESTADO] ");

  switch (estadoAtual) {
    case SEGURO:
      Serial.println("SEGURO");
      break;

    case ALERTA:
      Serial.println("ALERTA");
      break;

    case PERIGO:
      Serial.println("PERIGO");
      break;

    case EMERGENCIA:
      Serial.println("EMERGENCIA");
      break;
  }
}

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println("========================");
  Serial.println("      SAFEZONE");
  Serial.println("========================");

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
  Serial.println("Aguardando deteccao...");
}


void loop() {
  if (!digitalRead(BOTAO_STOP)) {
    Serial.println();
    Serial.println(">>> BOTAO STOP PRESSIONADO <<<");

    maquinaBloqueada = true;
    estadoAtual = EMERGENCIA;

    digitalWrite(RELE_PIN, HIGH);
    ledSeguro();
    mostrarEstado();
    delay(300);
  }

  if (!digitalRead(RESET_BUTTON)) {
    Serial.println();
    Serial.println(">>> BOTAO RESET PRESSIONADO <<<");

    maquinaBloqueada = false;
    estadoAtual = SEGURO;

    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
    ledSeguro();
    mostrarEstado();
    delay(300);
  }
  if (maquinaBloqueada) {
    Serial.println("[INFO] Sistema bloqueado");

    delay(500);
    return;
  }

  // LEITURA PIR
  bool movimento = digitalRead(PIR_SENSOR);

  Serial.print("[PIR] Movimento: ");
  Serial.println(movimento ? "SIM" : "NAO");

  // SEM MOVIMENTO
  if (!movimento) {
    estadoAtual = SEGURO;
    ledSeguro();
    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
    mostrarEstado();
    delay(500);
    return;
  }

  // MOVIMENTO DETECTADO
  Serial.println();
  Serial.println("===== MOVIMENTO DETECTADO =====");

  uint32_t distancia = medirDistancia();

  Serial.print("[DISTANCIA] ");
  Serial.print(distancia);
  Serial.println(" cm");

  // PERIGO
  if (distancia <= 10) {
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

    Serial.println("*** PERIGO ***");
    Serial.println("MAQUINA BLOQUEADA");

    mostrarEstado();
  }

  // ALERTA
  else if (distancia <= 15) {
    estadoAtual = ALERTA;
    ledAlerta();
    Serial.println("*** ALERTA DE PROXIMIDADE ***");

    mostrarEstado();
  }

  // SEGURO
  else {
    estadoAtual = SEGURO;
    ledSeguro();
    digitalWrite(RELE_PIN, LOW);
    noTone(BUZZER_PIN);
    Serial.println("AREA SEGURA");
    mostrarEstado();
  }

  Serial.println("----------------------------");

  delay(500);
}