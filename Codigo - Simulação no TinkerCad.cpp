#include <LiquidCrystal.h>

LiquidCrystal led(12, 13, A0, A1, A2, A3);

// Definições dos pinos
#define PINO_BOMBA_1_IN1 2
#define PINO_BOMBA_1_IN2 3

#define PINO_BOMBA_2_IN1 4
#define PINO_BOMBA_2_IN2 5

#define PINO_BOTAO1 6
#define PINO_BOTAO2 7

#define PINO_MISTURADOR 8
#define PINO_SOLENOIDE 9

#define PINO_SENSOR_VAZIO 10
#define PINO_SENSOR_CHEIO 11

// Estados da maquina selecionados
#define ESTADO_INICIAL 0    // Estado Inicial;
#define ESTADO_BOMBA1 1     // Estado Acionar a bomba B1 por 5 segundos;
#define ESTADO_BOMBA2 2     // Estado Acionar a bomba B2 por até que o sensor de nível cheio seja ativado;
#define ESTADO_MISTURADOR 3 // Estado Ligar o misturador por 5 segundos;
#define ESTADO_SOLENOIDE 4  // Estado Acionar a válvula solenoide até esvaziar o recipiente (sensor de nível vazio ativado).

// Constantes atualizar o estado da maquina
uint8_t estadoAtual;
uint8_t estadoAnterior;

int estado_sensor_vazio;
int estado_sensor_cheio;

int novo_estado_sensor_vazio;
int novo_estado_sensor_cheio;

// Definições para ligar e desligar bombas 
#define LIGAR_BOMBA true
#define DESLIGAR_BOMBA false

#define numBtn 2

// Variavel de apoio para o tempo que o btn esta pressionado
unsigned long tempoDeInicio = 0;

// Função para Debounce
bool verificarBtn(const uint8_t button);

// Função para ligar e exibir mensagens do Lcd
void LCD(const char *msg_1, const char *msg_2);

// Função para atualizar o estado da maquina
void atualizarEstado();

unsigned long tempoDebounce = 50;                     // Tempo de Debounce
unsigned long tempoDebounceAnterior[numBtn] = {0, 0}; // Tempo anterior de debounce de cada botão

const uint8_t btns[numBtn] = {6, 7};

uint8_t leituraAnterior[numBtn];          // A leitura anterior do pino de entrada
uint8_t estadoBtn[numBtn] = {HIGH, HIGH}; // A leitura atual do pino de entrada

// Função para acionar bombas
void acionar_bomba(int in1, int in2, bool acao)
{
    if (acao == LIGAR_BOMBA)
    {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
    }
    else
    {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
    }
}

void setup()
{
    // put your setup code here, to run once:
    pinMode(PINO_BOMBA_1_IN1, OUTPUT);
    pinMode(PINO_BOMBA_1_IN2, OUTPUT);
    pinMode(PINO_BOMBA_2_IN1, OUTPUT);
    pinMode(PINO_BOMBA_2_IN2, OUTPUT);
    pinMode(PINO_BOTAO1, INPUT_PULLUP);
    pinMode(PINO_BOTAO2, INPUT_PULLUP);
    pinMode(PINO_SENSOR_VAZIO, INPUT_PULLUP);
    pinMode(PINO_SENSOR_CHEIO, INPUT_PULLUP);

    pinMode(PINO_MISTURADOR, OUTPUT);
    pinMode(PINO_SOLENOIDE, OUTPUT);

    acionar_bomba(PINO_BOMBA_1_IN1, PINO_BOMBA_1_IN2, DESLIGAR_BOMBA);
    acionar_bomba(PINO_BOMBA_2_IN1, PINO_BOMBA_2_IN2, DESLIGAR_BOMBA);

    estado_sensor_vazio = digitalRead(PINO_SENSOR_VAZIO);
    novo_estado_sensor_vazio = estado_sensor_vazio;

    estado_sensor_cheio = digitalRead(PINO_SENSOR_CHEIO);
    novo_estado_sensor_cheio = estado_sensor_cheio;
    Serial.begin(9600);

    led.begin(16, 2);

    // Gera aleatoriedade
    randomSeed(analogRead(0));

    // Configura todas as leituras anteriores como baixas
    for (int i = 0; i < numBtn; i++)
    {
        leituraAnterior[i] = HIGH;
    }

    // Configurações iniciais do codigo
    estadoAtual = ESTADO_INICIAL;
    estadoAnterior = estadoAtual;
    LCD("Iniciando...", "      (*^-^)/");
    delay(1000);
}

void loop()
{

    // Verifica qual o estado Atual da maquina e o exercuta
    switch (estadoAtual)
    {
    case ESTADO_INICIAL:

        LCD("!Sistema Pronto!", "    \\(^_^)/");
        LCD("!Sistema Pronto!", "    \\(^_-)/");

        // Verifica se o C1 foi pressionado 
        if (verificarBtn(0) == true)
        {
            tempoDeInicio = millis();
            LCD("Pressione C1 pra", " Iniciar Ciclo!");
			
            // Aguarda confimação para inicio de ciclo
            // Caso não confirme sera cancelado em 10 segundos
            while (millis() - tempoDeInicio < 10000)
            {

                if (verificarBtn(0) == true)
                {
                    estadoAtual = ESTADO_BOMBA1;
                    break;
                }
                else if (verificarBtn(1) == true)
                {
                    break;
                }
            }
        }
     	// Verifica se o C2 foi pressionado para esvaziamento
        else if (verificarBtn(1) == true)
        {
            LCD("Esvaciando o", "   Recipiente...");

            digitalWrite(PINO_SOLENOIDE, HIGH);

            while (digitalRead(PINO_SENSOR_VAZIO) == HIGH)
            {
            }
            digitalWrite(PINO_SOLENOIDE, LOW);
            LCD("    Esvaciado", "");
            delay(1000);
            estadoAtual = ESTADO_INICIAL;
        }
        break;
    case ESTADO_BOMBA1:
		
        acionar_bomba(PINO_BOMBA_1_IN1, PINO_BOMBA_1_IN2, LIGAR_BOMBA);
        tempoDeInicio = millis();

      	// Bomba ativa por 5 segundos, a menos que pressione C2
        while (millis() - tempoDeInicio < 5000)
        {
            LCD(" Bomba 1 ativa!", "    ~(^_^)~");
          
            // Verifica se deve interromper o sistema
            if (verificarBtn(1) == true)
            {
                acionar_bomba(PINO_BOMBA_1_IN1, PINO_BOMBA_1_IN2, DESLIGAR_BOMBA);
                unsigned long tempoDeDelay = millis();
                LCD("Sistema   (*-*)", "Interrompido");
                delay(1500);
                LCD("Deseja Continua?", "Sim(C1) Nao(C2)");
				
              	
                while (true)
                {

                    if (verificarBtn(0) == true)
                    {
                        acionar_bomba(PINO_BOMBA_1_IN1, PINO_BOMBA_1_IN2, LIGAR_BOMBA);
                        tempoDeInicio = tempoDeDelay;
                        break;
                    }
                    else if (verificarBtn(1) == true)
                    {
                        estadoAtual = ESTADO_INICIAL;
                        tempoDeInicio = 0;
                        break;
                    }
                }
            }
        }
        acionar_bomba(PINO_BOMBA_1_IN1, PINO_BOMBA_1_IN2, DESLIGAR_BOMBA);

        atualizarEstado();

        break;
    case ESTADO_BOMBA2:

        acionar_bomba(PINO_BOMBA_2_IN1, PINO_BOMBA_2_IN2, LIGAR_BOMBA);

      	// Bomba 2 ativa por até que o sensor de nível cheio seja ativado
        while (digitalRead(PINO_SENSOR_CHEIO) == HIGH && estadoAtual == ESTADO_BOMBA2)
        {
            LCD(" Bomba 2 ativa!", "    ~(-_-)~");

          	// Verifica se deve interromper o sistema
            if (verificarBtn(1) == true)
            {
                acionar_bomba(PINO_BOMBA_2_IN1, PINO_BOMBA_2_IN2, DESLIGAR_BOMBA);

                LCD("Sistema   (*-*)", "Interrompido");
                delay(1500);
                LCD("Deseja Continua?", "Sim(C1) Nao(C2)");

                
                while (true)
                {

                    if (verificarBtn(0) == true)
                    {
                        acionar_bomba(PINO_BOMBA_2_IN1, PINO_BOMBA_2_IN2, LIGAR_BOMBA);
                        break;
                    }
                    else if (verificarBtn(1) == true)
                    {
                        estadoAtual = ESTADO_INICIAL;
                        tempoDeInicio = 0;
                        break;
                    }
                }
            }
        }

        acionar_bomba(PINO_BOMBA_2_IN1, PINO_BOMBA_2_IN2, DESLIGAR_BOMBA);

        atualizarEstado();

        break;
    case ESTADO_MISTURADOR:

        digitalWrite(PINO_MISTURADOR, HIGH);
        tempoDeInicio = millis();

        // Misturador ligado por 5 segundos
        while (millis() - tempoDeInicio < 5000)
        {
            LCD("Misturador", "Ligado    (@_@)*");
          
          	// Verifica se deve interromper o sistema
            if (verificarBtn(1) == true)
            {
                digitalWrite(PINO_MISTURADOR, LOW);
                unsigned long tempoDeDelay = millis();
                LCD("Sistema   (*-*)", "Interrompido");
                delay(1500);
                LCD("Deseja Continua?", "Sim(C1) Nao(C2)");

                while (true)
                {

                    if (verificarBtn(0) == true)
                    {
                        digitalWrite(PINO_MISTURADOR, HIGH);
                        tempoDeInicio = tempoDeDelay;
                        break;
                    }
                    else if (verificarBtn(1) == true)
                    {
                        estadoAtual = ESTADO_INICIAL;
                        tempoDeInicio = 0;
                        break;
                    }
                }
            }
        }

        digitalWrite(PINO_MISTURADOR, LOW);

        atualizarEstado();

        break;
    case ESTADO_SOLENOIDE:

        digitalWrite(PINO_SOLENOIDE, HIGH);
		
        // Válvula solenoide ativa até esvaziar o recipiente
        while (digitalRead(PINO_SENSOR_VAZIO) == HIGH && estadoAtual == ESTADO_SOLENOIDE)
        {
            LCD("Valvula     ~~~~", "   Solenoide");

            // Verifica se deve interromper o sistema
            if (verificarBtn(1) == true)
            {
                digitalWrite(PINO_SOLENOIDE, LOW);

                LCD("Sistema   (*-*)", "Interrompido");
                delay(1500);
                LCD("Deseja Continua?", "Sim(C1) Nao(C2)");

                while (true)
                {

                    if (verificarBtn(0) == true)
                    {
                        digitalWrite(PINO_SOLENOIDE, HIGH);
                        break;
                    }
                    else if (verificarBtn(1) == true)
                    {
                        estadoAtual = ESTADO_INICIAL;
                        tempoDeInicio = 0;
                        break;
                    }
                }
            }
        }
        digitalWrite(PINO_SOLENOIDE, LOW);

        atualizarEstado();
        break;
    }
}

// Função para verificar se os botões foram pressionados e lidar com o debounce
bool verificarBtn(const uint8_t button)
{
    // Ler o estado do botão para uma variável local:
    uint8_t leitura = digitalRead(btns[button]);

    // Verificar se houve uma mudança no estado do botão
    if (leitura != leituraAnterior[button])
    {
        // Redefinir o temporizador de debounce
        tempoDebounceAnterior[button] = millis();
    }
    /* fase de teste */

    // Depois que o botão for ativo, irá esperar soltar
    while (digitalRead(btns[button]) == LOW)
    {
    }
    /* fase de teste */

    if (millis() - tempoDebounceAnterior[button] > tempoDebounce)
    {
        // Se passou tempo suficiente desde a última verificação, consideramos como uma mudança válida
        if (leitura != estadoBtn[button])
        {
            if (leitura == LOW)
            {
                leituraAnterior[button] = leitura;
                return true; // Retorna true se o novo estado do botão for ALTO
            }
        }
    }

    leituraAnterior[button] = leitura;
    return false;
}

// Função para ligar e exibir mensagens do Lcd
void LCD(const char *msg_1, const char *msg_2)
{
    led.clear();

    led.setCursor(0, 0);
    led.print(msg_1);

    led.setCursor(0, 1);
    led.print(msg_2);
    delay(500);
}

// Função para atualizar o estado da maquina
void atualizarEstado()
{

    if (estadoAtual == ESTADO_SOLENOIDE)
    {
        estadoAtual = ESTADO_INICIAL;
    }
    else if (estadoAtual == estadoAnterior + 1 || estadoAtual == estadoAnterior)
    {
        estadoAtual++;
        estadoAnterior = estadoAtual;
    }
    estadoAnterior = estadoAtual;
}