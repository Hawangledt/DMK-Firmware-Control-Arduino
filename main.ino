/*
  Definição de pinos de Saída
*/

// Sinalizador
#define LUZ_VERMELHA 7
#define LUZ_AMARELA 6
#define LUZ_VERDE 5

// Gases
#define GAS_PISTOLA 8             // valvula 4 pistola
#define GAS_PROTECAO_EXTERNO_1 2  //valvula 1 protecao
#define GAS_PROTECAO_EXTERNO_2 3  //alvula 2 protecao
#define GAS_REFRIGERACAO 4        //valvula 3

/*
  Definição de pinos de Entrada
*/

// Arduíno
#define BOTAO_INICIAR 13
// Parada de Emergencia
#define LER_PE 11

// -------------------------------------------------------------------

int p0 = 10;
int t0 = 0;
//
int p1 = 50;
int t1 = 500;
//
int p2 = 50;
int t2 = 500;
//
int p3 = 10;
int t3 = 500;


/*
  Definição do Protocolo de Comunicação
*/

#include <SPI.h>
#include <Ethernet.h>

// Desabilita o Controle Externo da Pistola de Solda
#define DESABILITA_AD_MODE "GET /ADMODE.cgi?button_DEC=AD%C4%A3%CA%BD%BD%FB%D6%B9"
// Desabilita a Comunicacao externa, liberando o controle por Ethernet
#define HABILITA_COMUNICACAO "ExtEN.cgi?Disable=%CD%E2%B2%BF%CA%B9%C4%DC%B9%D8"
// Desabilita o Controle do Guide Lazer de forma externa, liberando o controle por Ethernet
#define HABILITA_COMUNICACAO_GUIDE_LASER "GET /EXTGuideLaserEN.cgi?Disable=%BA%EC%B9%E2%C4%DA%BF%D8"

// Habilita o laser de referencia
#define HABILITA_GUIDE_LASER "GET /GuideLaserON.cgi?Enable=%BA%EC%B9%E2%BF%AA"
// Desabilita o laser de referencia
#define DESABILITA_GUIDE_LASER "GET /GuideLaserON.cgi?Disable=%BA%EC%B9%E2%B9%D8"

// Habilita a emissão do laser
#define HABILITA_EMISSAO "GET /Emission.cgi?Enable=%B3%F6%B9%E2"
// Desabilita a emissão do laser
#define DESABILITA_EMISSAO "GET /Emission.cgi?Disable=%B9%D8%B9%E2" 
// Modifica a potencia do laser
#define SETAR_POTENCIA "GET /SetPower.cgi?Power=@&button_SetPower=Set"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Endereço MAC da placa Ethernet
IPAddress server(192, 168, 0, 10);                    // Endereço IP do servidor HTTP
EthernetClient client;

// --------------------------------------------------------------------

/*
Definição das Maquina de estados
*/

enum estados { ESTADO_INICIAL,
               PARAMETROS,
               PROTECAO,
               EMISSAO,
               REFRIGERACAO,
               PE };

enum estados _estado;

// -------------------------------------------------------------------

/*
  Funçâo auxiliar
*/

void validar_parametros() {
  // WIP
}

void setar_parametros() {
  // WIP - Criar biblioteca para Ler Cartao SD

int p0 = 10;
int t0 = 0;
//
int p1 = 50;
int t1 = 500;
//
int p2 = 50;
int t2 = 500;
//
int p3 = 10;
int t3 = 500;
}

// -------------------------------------------------------------------

/* 
  Gases
*/

void start_gas_protecao() {
  digitalWrite(GAS_PISTOLA, LOW);
  digitalWrite(GAS_PROTECAO_EXTERNO_1, LOW);
  digitalWrite(GAS_PROTECAO_EXTERNO_2, LOW);
}

void stop_gas_protecao() {
  digitalWrite(GAS_PISTOLA, HIGH);
  digitalWrite(GAS_PROTECAO_EXTERNO_1, HIGH);
  digitalWrite(GAS_PROTECAO_EXTERNO_2, HIGH);
}

void start_gas_refrigeracao() {
  digitalWrite(GAS_REFRIGERACAO, LOW);
  delay(20);
}

void stop_gas_refrigeracao() {
  digitalWrite(GAS_REFRIGERACAO, HIGH);
}

// -------------------------------------------------------------------

/*
  Comunicação com a Fonte
*/

// função base para comunicar com a fonte
void comunica_fonte_lazer(String parametro) {
  if (client.connect(server, 80)) {
    Serial.println("Enviando Comando Get ...");
    client.println(parametro);
    client.println("Host: 192.168.0.10");
    client.println("Connection: close");
    client.println();
    Serial.println("Comando Get enviado!");
  } else {
    Serial.println("Falha na conexão ...");
  }
}

// funções auxiliares de controle da fonte
void ativar_emissao() {
  Serial.println("Ativar Emissão");
  comunica_fonte_lazer(HABILITA_EMISSAO);
}

void parar_emissao() {
  Serial.println("Parar Emissão");
  comunica_fonte_lazer(DESABILITA_EMISSAO);
}

void setar_potencia(int potencia) {
  String pot = String(potencia);
  String msg = "Potencia alterada para ";
  Serial.println(msg + pot);
  String nova_potencia = SETAR_POTENCIA;
  nova_potencia.replace("@", pot);
  comunica_fonte_lazer(nova_potencia);
}

void iniciar_guide_laser() {
  Serial.println("Ativar Guide Lazer");
  comunica_fonte_lazer(HABILITA_GUIDE_LASER);
}

void parar_guide_laser() {
  Serial.println("Dasativar Guide Lazer");
  comunica_fonte_lazer(DESABILITA_GUIDE_LASER);
}

// Funcções de procedimento de solda
void iniciar_solda() {
  Serial.println("Iniciar Solda");

  int tempo_de_inicio = millis();
  int tempo_decorrido = 0;
  int potencia_atual = 0;

  setar_potencia(p0);
  delay(20);
  ativar_emissao();
  delay(20);

  // Rampa de subida
  while (tempo_decorrido < t1) {
    potencia_atual = map(tempo_decorrido, t0, t1, p0, p1);  // Calcula a potência atual
    setar_potencia(potencia_atual);
    tempo_decorrido = millis() - tempo_de_inicio;  // Atualiza o tempo decorrido
    delay(20);
  }

  // Reta
  for (int i = 0; i < t2 / 10; i++) {
    tempo_decorrido = millis() - tempo_de_inicio;  // Atualiza o tempo decorrido
    delay(20);
  }

  // Rampa de descida
  while (tempo_decorrido < (t1 + t2 + t3)) {
    potencia_atual = map(tempo_decorrido, (t1 + t2), (t1 + t2 + t3), p2, p3);  // Calcula a potência atual
    setar_potencia(potencia_atual);
    tempo_decorrido = millis() - tempo_de_inicio;  // Atualiza o tempo decorrido
    delay(20);
  }
}

void parar_solda() {
  parar_emissao();
}

// -------------------------------------------------------------------

void set_aviso_luminoso(int cor) {
  digitalWrite(LUZ_VERMELHA, HIGH);
  digitalWrite(LUZ_AMARELA, HIGH);
  digitalWrite(LUZ_VERDE, HIGH);
  digitalWrite(cor, LOW);
}

void setup() {
  // estação de solda
  //    comunicação via ethernet, com exceção da PE
  // sinalizador
  pinMode(LUZ_VERMELHA, OUTPUT);
  pinMode(LUZ_AMARELA, OUTPUT);
  pinMode(LUZ_VERDE, OUTPUT);
  // gás
  pinMode(GAS_PISTOLA, OUTPUT);
  pinMode(GAS_PROTECAO_EXTERNO_1, OUTPUT);
  pinMode(GAS_PROTECAO_EXTERNO_2, OUTPUT);
  pinMode(GAS_REFRIGERACAO, OUTPUT);
  // arduíno
  pinMode(BOTAO_INICIAR, INPUT);
  // PE
  pinMode(LER_PE, INPUT);
  digitalWrite(LUZ_VERMELHA, HIGH);
  digitalWrite(LUZ_AMARELA, HIGH);
  digitalWrite(LUZ_VERDE, HIGH);
  digitalWrite(GAS_PISTOLA, HIGH);
  digitalWrite(GAS_PROTECAO_EXTERNO_1, HIGH);
  digitalWrite(GAS_PROTECAO_EXTERNO_2, HIGH);
  digitalWrite(GAS_REFRIGERACAO, HIGH);

  Ethernet.begin(mac);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Iniciando...");

  //Estabece comunicacão com a fonte laser
  comunica_fonte_lazer(DESABILITA_AD_MODE);
  delay(100);
  comunica_fonte_lazer(HABILITA_COMUNICACAO);
  delay(100);
  comunica_fonte_lazer(HABILITA_COMUNICACAO_GUIDE_LASER);
  delay(100);
}

// -------------------------------------------------------------------
// -------------------------MAQUINAS DE ESTADO------------------------
// -------------------------------------------------------------------

void estado_inicial() {
  Serial.println("#ESTADO: Inicial");
  set_aviso_luminoso(LUZ_VERDE);
  iniciar_guide_laser();
  while (digitalRead(BOTAO_INICIAR) == 0) {
    delay(20);
  }
  parar_guide_laser();
  Serial.println("Botao Iniciar foi pressionado");
  set_aviso_luminoso(LUZ_VERMELHA);
  _estado = PARAMETROS;
  delay(1000);
}

void estado_parametros() {
  Serial.println("#ESTADO: Parametros");
  validar_parametros();
  setar_parametros();
  _estado = PROTECAO;
}

void estado_protecao() {
  Serial.println("#ESTADO: Protecão");
  start_gas_protecao();
  delay(2000);
  _estado = EMISSAO;
}

void estado_emissao() {
  Serial.println("#ESTADO: Emissão");
  iniciar_solda();
  parar_solda();
  delay(500);
  _estado = REFRIGERACAO;
}

void estado_refrigeracao() {
  Serial.println("#ESTADO: Regrigeracão");
  stop_gas_protecao();
  start_gas_refrigeracao();
  delay(1000);
  stop_gas_refrigeracao();
  _estado = ESTADO_INICIAL;
}

void estado_pe() {
  Serial.println("#ESTADO: PE");
  set_aviso_luminoso(LUZ_AMARELA);
  delay(3000);
  if (BOTAO_INICIAR > 0) {
    _estado = ESTADO_INICIAL;
  } else {
    _estado = PE;
  }
}

void loop() {
  switch (_estado) {
    case ESTADO_INICIAL:
      estado_inicial();
      break;

    case PARAMETROS:
      estado_parametros();
      break;

    case PROTECAO:
      estado_protecao();
      break;

    case EMISSAO:
      estado_emissao();
      break;

    case REFRIGERACAO:
      estado_refrigeracao();
      break;

    case PE:
      estado_pe();
      break;
  }
}
