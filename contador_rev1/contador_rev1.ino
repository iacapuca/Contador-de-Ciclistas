/*
     /\  \         /\  \         /\__\         /\  \         /\  \         /\  \         /\  \         /\  \    
    /::\  \       /::\  \       /::|  |        \:\  \       /::\  \       /::\  \       /::\  \       /::\  \   
   /:/\:\  \     /:/\:\  \     /:|:|  |         \:\  \     /:/\:\  \     /:/\:\  \     /:/\:\  \     /:/\:\  \  
  /:/  \:\  \   /:/  \:\  \   /:/|:|  |__       /::\  \   /::\~\:\  \   /:/  \:\__\   /:/  \:\  \   /::\~\:\  \ 
 /:/__/ \:\__\ /:/__/ \:\__\ /:/ |:| /\__\     /:/\:\__\ /:/\:\ \:\__\ /:/__/ \:|__| /:/__/ \:\__\ /:/\:\ \:\__\
 \:\  \  \/__/ \:\  \ /:/  / \/__|:|/:/  /    /:/  \/__/ \/__\:\/:/  / \:\  \ /:/  / \:\  \ /:/  / \/_|::\/:/  /
  \:\  \        \:\  /:/  /      |:/:/  /    /:/  /           \::/  /   \:\  /:/  /   \:\  /:/  /     |:|::/  / 
   \:\  \        \:\/:/  /       |::/  /     \/__/            /:/  /     \:\/:/  /     \:\/:/  /      |:|\/__/  
    \:\__\        \::/  /        /:/  /                      /:/  /       \::/__/       \::/  /       |:|  |    
     \/__/         \/__/         \/__/                       \/__/         ~~            \/__/         \|__|    
*/

//////////////////
// EQUIPAMENTOS //
//////////////////

/*

Microprocessador integrado na placa
*Arduíno 

Contador do tempo e data em que ele começou a contar.
*RTC_DS1307 RTC;

Sensor de distância ultrassom
* 

Gravação de dados
* 

Bateria
* 

Caixa de proteção
*

*/

////////////////////////
// PASSOS DO SOFTWARE //
////////////////////////

/*

Passo 1: Identificar o ciclista
* Mede a distância do ciclista ao sensor
* Verifica se o ciclista continua por lá
* Verifica hora que o ciclista passou
* Verifica a quantidade de tempo que o ciclista levou para passar

Passo 2: Gravar os dados
* Verifica se o SD está funcionando
* Gravar data, hora, tempo que levou para passar, distância que ele passou

Passo 3: Montar o equipamento
* Transformar o software num hardware possível de ser instalado em qualquer local

Passo 3: Pós processamento (fora do Arduíno)
* Tratar dados para verificar se é possível determinar, além do número de ciclistas, a direção e a velocidade do ciclista

Passo 4: Disponibilização dos dados
* Montar plataforma para mostrar os dados

*/

/////////////////////////////
// BIBLIOTECAS DE INCLUSÃO //
/////////////////////////////

#include <NewPing.h>          // Biblioteca para facilitar o ping do sonar http://playground.arduino.cc/Code/NewPing
#include <SD.h>               // Biblioteca de leitura do cartão SD https://www.arduino.cc/en/Reference/SD
#include <SPI.h>              // Biblioteca para leitura Serial https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>             // Biblioteca para comunicar com equipamento IC2 https://www.arduino.cc/en/Reference/Wire
#include <RTClib.h>           // Biblioteca para uso do RTC (VERIFICAR FUNCIONAMENTO)

/////////////////////////////
// DEFINIÇÕES E CONSTANTES //
/////////////////////////////

// PINAGEM //

// LED de aviso
#define LED 13                // LED interno do arduíno

// SD CARD
#define chipSelect 4          // Pino de chipSelect
//const int chipSelect = 4;

// SONAR
#define TRIGGER_PIN  9        // Pino de trigger
#define ECHO_PIN     10       // Pino de echo

// DELAY //
#define DELAY_ERROR 2000      // Delay em caso de erro de gravação no SD
#define DELAY_PISCA 50        // Delay do piscar do LED

// CONFIGURAÇÂO //

// GRAVAÇÃO
#define FILENAME_DATA "datalog.csv"     // Nome do arquivo a ser gravado

// SONAR
#define MAX_DISTANCE 80       // Distância máxima que o sonar enxerga
#define MIN_DISTANCE 0        // Distância mínima que o sonar enxerga

// CONTAGEM
#define minPingCountToAcceptHuman 1     // quantia mínima de "pingadas" que um objeto deve receber para ser considerado um humano
#define MAXBLACKOUINMILIS_INICIAL 100   // Tempo que ele espera que o ciclista ainda possa estar por lá (valor inicial)
#define MAXBLACKOUINMILIS_RATIO 2       // se o monitoramento for permanente, o tempo de monitoramento vai dividindo nessa taxa
#define MAXBLACKOUINMILIS_SCOPE 0       // se o monitoramento for permanente, o tempo de monitoramento vai subtraindo nessa taxa
#define MAXBLACKOUINMILIS_MIN 10        // tempo mínimo de espera

// BOOLEANAS //

#define ATIVA_LED false                      // Ativa o modo de piscar do LED
#define ATIVA_SD false                       // Ativa quando o SD está incluído
#define ATIVA_RTC false                      // Ativa o modo do RTC
#define ATIVA_SERIALPRINT true               // Ativa o serial print (modo computador)
#define ATIVA_MONITORAMENTOPERMANENTE false  // Reinicia o tempo de espera caso o ciclista ainda esteja na frente do sensor

////////////////////////
// VARIÁVEIS INICIAIS //
////////////////////////

// SONAR
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);    // Cria o objeto do SONAR
int dist_cm = 0;                                       // Distância em cm do objeto ao radar

// CONTAGEM EM SI
int humanCount = 0;                          // Quantidade de ciclistas
boolean iSeeHuman = false;                   // Quando ele avista um ciclista
boolean iTrackHuman = false;                 // Quando ele mantém vendo um ciclista
long lastTimeISawHuman;                      // Tempo que ele passa avistando um ciclista
int numberOfPingsObsevedHumanIsInRange = 0;  // assim que observamos uma pessoa, nos contamos um número de vezes que essa pessoa fica na frente do dispositivo
int maxBlackoutInMillis = MAXBLACKOUINMILIS_INICIAL;

// RTC
DateTime now;

////////////////////////
// FUNÇÕES DE CHAMADA //
////////////////////////

void printHistory() {
     Serial.write("Historico\n\n");

     Serial.print("Contador: ");
     Serial.println(humanCount);
     Serial.print("Distancia: ");
     Serial.printn(dist_cm);
     Serial.print("Pings: ");
     Serial.printn(numberOfPingsObsevedHumanIsInRange);
     
     if (ATIVA_RTC) {
          now = rtc.now();
          Serial.print("Hora: ");
          Serial.print(now.year(), DEC);
          Serial.print('/');
          Serial.print(now.month(), DEC);
          Serial.print('/');
          Serial.print(now.day(), DEC);
          Serial.print(' ');
          Serial.print(",");
          Serial.print(now.hour(), DEC);
          Serial.print(':');
          Serial.print(now.minute(), DEC);
          Serial.print(':');
          Serial.println(now.second(), DEC);
     }
}

void blinkHumanCountedLed() {
      for(int i = 0; i < 5; i++) {
            digitalWrite(LED, HIGH);
            delay(DELAY_PISCA);
            digitalWrite(LED, LOW);
            delay(DELAY_PISCA);
      }
}

void LogToSD() {
     if(ATIVA_RTC)
          now = rtc.now();

     File dataFile = SD.open(FILENAME_DATA, FILE_WRITE);
     
     if(dataFile) {
          dataFile.print(humanCount);
          dataFile.print(",");
          dataFile.print(numberOfPingsObsevedHumanIsInRange);
          dataFile.print(",");
          dataFile.print(dist_cm);
     
          if(ATIVA_RTC) {
               dataFile.print(",");
               dataFile.print(now.year(), DEC);
               dataFile.print('/');
               dataFile.print(now.month(), DEC);
               dataFile.print('/');
               dataFile.print(now.day(), DEC);
               dataFile.print(' ');
               dataFile.print(",");
               dataFile.print(now.hour(), DEC);
               dataFile.print(':');
               dataFile.print(now.minute(), DEC);
               dataFile.print(':');
               dataFile.print(now.second(), DEC);
          }
          dataFile.println();
          dataFile.close();
     } else {
          if(ATIVA_SERIALPRINT)
               Serial.println("Erro ao gravar o arquivo.");
     }
}

/////////////////////////
// FUNÇÕES DO PROGRAMA //
/////////////////////////

// FUNÇÃO DE SETUP DO ARDUÍNO //
void setup() {
     Serial.begin(9600);
     while (!Serial) {
          ; // wait for serial port to connect. Needed for native USB port only
     }
     
     if (ATIVA_LED) {
          pinMode(LED, OUTPUT);
          blinkHumanCountedLed();
          Serial.printn("Pisca LED ao contar: ATIVADO");
     }
     
     if (ATIVA_SD) {
          Serial.print("Iniciando cartão SD...");
           if (!SD.begin(chipSelect)) {
               Serial.println("O cartão SD falhou ou não está presente");
               return;     // Não tem mais o que fazer, volta sem configurar (necessário?)
          } else Serial.println("Cartão Inicializado.");
     }

     if (ATIVA_RTC) {
          Wire.begin();
          RTC.begin();
          if (! RTC.isrunning()) {
               Serial.println("RTC não está rodando");
               // following line sets the RTC to the date & time this sketch was compiled
               // uncomment it & upload to set the time, date and start run the RTC!
               RTC.adjust(DateTime(__DATE__, __TIME__));
           }
     }
    printHistory();
}

// FUNÇÃO DE LOOP DO ARDUÍNO //

void loop() {
     
     //DateTime now = RTC.now(); // Objeto DateTime
     dist_cm = sonar.ping_cm();         // Lê o valor da distância medida no SONAR
     iSeeHuman = dist_cm < MAX_DISTANCE && dist_cm > MIN_DISTANCE; // Se a distância está entre esses valores, considera que um humano está passando
 
     // Primeiro localiza o humano e verifica se o modo track está desligado
      if(iSeeHuman && !iTrackHuman) {                  // Fez o primeiro contato com o humano
          numberOfPingsObsevedHumanIsInRange += 1;   // Soma 1 ao número de pings de reconhecimento
          iTrackHuman = true;                        // Passa a rastrear o humano
          lastTimeISawHuman = millis();              // Marca o horário em que viu o humano (depois trocar para o RTC)
          // Enquanto 1) ele ver um humano 2) ainda não deus os pings necessários
          while (iTrackHuman && numberOfPingsObsevedHumanIsInRange < minPingCountToAcceptHuman) {
               // Nova leitura
               dist_cm = sonar.ping_cm();         // Lê o valor da distância medida no SONAR
               if(dist_cm < MAX_DISTANCE && dist_cm > MIN_DISTANCE) {
                    numberOfPingsObsevedHumanIsInRange += 1;   // Soma 1 ao número de pings de reconhecimento
                    if (ATIVA_SD) {
                         LogToSD();
                    } else {
                         if (ATIVA_SERIALPRINT)
                              printHistory();
                    }
                    if(ATIVA_MONITORAMENTOPERMANENTE) {
                         lastTimeISawHuman = millis();
                         maxBlackoutInMillis = maxBlackoutInMillis/MAXBLACKOUINMILIS_RATIO - MAXBLACKOUINMILIS_SCOPE;
                         if(maxBlackoutInMillis < MAXBLACKOUINMILIS_MINI) 
                              maxBlackoutInMillis = MAXBLACKOUINMILIS_MINI;
                    }
                    if(numberOfPingsObsevedHumanIsInRange >= minPingCountToAcceptHuman) {
                         humanCount++;
                         if(ATIVA_LED)
                              blinkHumanCountedLed();
                    }
               }
               // Caso tenha muito tempo que ele não vê um humano, ele desativa a perseguição
               if (millis() - lastTimeISawHuman) >= maxBlackoutInMillis) iTrackHuman == false;
          }
     }

     numberOfPingsObsevedHumanIsInRange = 0;
     maxBlackoutInMillis = MAXBLACKOUINMILIS_INICIAL;

     // MODO DEBUG - Verificar na tela
     if(ATIVA_SERIALPRINT) {
          printHistory();
     }

}
