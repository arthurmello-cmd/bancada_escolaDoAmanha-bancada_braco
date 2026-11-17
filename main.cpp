#include <Arduino.h>

// As saidas usam a mesma logica ativa em LOW da configuracao original.
constexpr uint8_t MAGAZINE = 23;
constexpr uint8_t SUCCAO = 22;
constexpr uint8_t VAPOR = 21;
constexpr uint8_t DESCIDA = 19;
constexpr uint8_t RETORNO = 18;
constexpr uint8_t SAIDAS[] = {MAGAZINE, SUCCAO, VAPOR, DESCIDA, RETORNO};
constexpr bool ATIVO_LOW = true;
bool estados[5] = {};

void saida(size_t indice, bool ligada) {
  estados[indice] = ligada;
  digitalWrite(SAIDAS[indice], (ligada != ATIVO_LOW) ? HIGH : LOW);
}

void status() {
  Serial.println("Estado das saidas (1 = ligada):");
  for (size_t i = 0; i < 5; ++i)
    Serial.printf("  D%u: %u\r\n", SAIDAS[i], estados[i] ? 1U : 0U);
}

void ajuda() {
  Serial.println("0: avancar/retornar magazine (D23)");
  Serial.println("1: ligar/desligar descida (D19)");
  Serial.println("2: ligar/desligar succao (D22)");
  Serial.println("3: ligar/desligar retorno do braco (D18)");
  Serial.println("4: desligar succao e ligar/desligar vapor (D21)");
  Serial.println("s: desligar todas as saidas | l: estado | h: ajuda");
  Serial.println("Digite o comando e envie pelo monitor serial.");
}

void comando(char c) {
  switch (c) {
    case '0': saida(0, !estados[0]); break;
    case '1':
      if (!estados[3]) saida(4, false);
      saida(3, !estados[3]);
      break;
    case '2': saida(1, !estados[1]); break;
    case '3':
      if (!estados[4]) saida(3, false);
      saida(4, !estados[4]);
      break;
    case '4':
      saida(1, false);
      saida(2, !estados[2]);
      break;
    case 's':
      for (size_t i = 0; i < 5; ++i) saida(i, false);
      break;
    case 'l': status(); return;
    case 'h': ajuda(); return;
    case '\r': case '\n': case ' ': case '\t': return;
    default: Serial.println("Comando desconhecido. Envie h para ajuda."); return;
  }
  status();
}

void setup() {
  for (size_t i = 0; i < 5; ++i) {
    digitalWrite(SAIDAS[i], ATIVO_LOW ? HIGH : LOW);
    pinMode(SAIDAS[i], OUTPUT);
  }
  Serial.begin(115200);
  Serial.println("Controle manual da MPS");
  ajuda();
  status();
}

void loop() {
  while (Serial.available()) comando(static_cast<char>(Serial.read()));
}
