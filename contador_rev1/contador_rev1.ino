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

/////////////////////////////
// BIBLIOTECAS DE INCLUSÃO //
/////////////////////////////

#include <NewPing.h>          // Biblioteca para facilitar o ping do sonar
#include <SD.h>               // Biblioteca de leitura do cartão SD
#include <SPI.h>              // Biblioteca para ??????????????
#include <Wire.h>             // Biblioteca para ??????????????
//#include "RTClib.h"         // Biblioteca para uso do RTC

// Contar o tempo e data em que ele começou a contar.
//RTC_DS1307 RTC;

/////////////////////////////
// DEFINIÇÕES E CONSTANTES //
/////////////////////////////

// PINAGEM //

// LED de aviso
#define LED 13                // LED interno do arduíno

// SD CARD
//#define CHIPSELECT_PIN 4;   // Pino de chipSelect
const int chipSelect = 4;

// SONAR
#define TRIGGER_PIN  9        // Pino de trigger
#define ECHO_PIN     10       // Pino de echo

// CONFIGURAÇÂO //
// SONAR
#define MAX_DISTANCE 80       // Distância máxima que o sonar enxerga
#define MIN_DISTANCE 0;       // Distância mínima que o sonar enxerga

// CONTAGEM
int numberOfPingsObsevedHumanIsInRange = 0;     // assim que observamos uma pessoa, nos contamos um número de vezes que essa pessoa fica na frente do dispositivo
static int minPingCountToAcceptHuman = 1;       // quantia mínima de "pingadas" que um objeto deve receber para ser considerado um humano
static float maxBlackoutInMillis = 100;         // 
static int iSeeHumanThresholdInCm = 50;         // 

// LED
const boolean DEBUG = false;                    // quando setado TRUE, o led vai blinkar toda vez que detectar um humano

////////////////////////
// VARIÁVEIS INICIAIS //
////////////////////////

// SONAR
int dist_cm = 0;              // Distância em cm do objeto ao radar

// CONTAGEM EM SI
int humanCount = 0;           // Quantidade de ciclistas

boolean iSeeHuman = false;    // Quando ele avista um ciclista
boolean iTrackHuman = false;  // Quando ele avista 
long lastTimeISawHuman;       // Tempo que ele passa avistando um ciclista

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);   // Cria o objeto do SONAR

////////////////////////
// FUNÇÕES DE CHAMADA //
////////////////////////

void printHistory() {
      Serial.write("Historico\n\n");
      //Serial.print("Hora: ");
      //Serial.println(tempo);
      Serial.print("Contador: ");
      Serial.println(humanCount);
      Serial.println();
}

void blinkHumanCountedLed() {
      for(int i = 0; i < 5; i++) {
            digitalWrite(LED, HIGH);
            delay(50);
            digitalWrite(LED, LOW);
            delay(50);
      }
}

/*  void LogToSD() {
    DateTime now = rtc.now();
    File dataFile = SD.open("datalog.csv", FILE_WRITE);

    dataFile.print(humanCount);
    dataFile.print(",");

    dataFile.print(dist_cm);
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
    dataFile.println();
    dataFile.close();
}

// if the file isn't open, pop up an error:
  else {
    //Serial.println("error opening datalog.txt");
    Serial.print("File Error");
    delay(2000);
      }
 }
 */

/////////////////////////
// FUNÇÕES DO PROGRAMA //
/////////////////////////

// FUNÇÃO DE SETUP DO ARDUÍNO //
void setup() {
      Serial.begin(9600);

/*
      if(DEBUG) {
*/
            pinMode(LED, OUTPUT);
            blinkHumanCountedLed();
/*
      Wire.begin();
      RTC.begin();
      Serial.print("Iniciando cartão SD...");

      if (!SD.begin(chipSelect)) {
          Serial.println("O cartão SD falhou ou não está presente");
          return;     // don't do anything more:
      }
      Serial.println("Cartão Inicializado.");
}
*/

/*
      if (! RTC.isrunning()) {
            Serial.println("RTC não está rodando");
            // following line sets the RTC to the date & time this sketch was compiled
            // uncomment it & upload to set the time, date and start run the RTC!
            RTC.adjust(DateTime(__DATE__, __TIME__));
      }
*/
    delay(500);
    printHistory();
}

// FUNÇÃO DE LOOP DO ARDUÍNO //
void loop() {
  // sensor.ping_cm()sera utilizado no lugar de pingDistanceInCm()
  // melhor usar uma variavel no lugar do sensor.ping_cm()
  // iSeeHuman = sensor.ping_cm() < iSeeHumanThresholdInCm && sensor.ping_cm() > 5
  //dist_cm = pingDistanceInCm();
  //DateTime now = RTC.now(); // Objeto DateTime
      int  dist_cm = sonar.ping_cm();
      iSeeHuman = dist_cm < iSeeHumanThresholdInCm;// && dist_cm > MIN_DISTANCE;
 
//      if(dist_cm > 0) {
            Serial.print(iSeeHuman);
            Serial.print(",");
            Serial.print(humanCount);
            Serial.print(",");
            Serial.print(dist_cm);
            Serial.print(",");
            Serial.println(millis());
//      }

      if(iSeeHuman && !iTrackHuman) {           // we have first contact with human
            numberOfPingsObsevedHumanIsInRange += 1;
            iTrackHuman = true;
            lastTimeISawHuman = millis();
      } 
      else if(iSeeHuman && iTrackHuman) {
            if((millis() - lastTimeISawHuman) < maxBlackoutInMillis * (1+2*(numberOfPingsObsevedHumanIsInRange==1))) {
                  // wo dont see him now, but we believe he may still be in sight
            } 
            else {
                  iTrackHuman = false;                    // we havent seen him for too long
                  if(numberOfPingsObsevedHumanIsInRange == minPingCountToAcceptHuman) {
                        humanCount++;
                        if(DEBUG)
                              blinkHumanCountedLed();
                        //LogToSD();
                  }
            }
      } 
      else {
            numberOfPingsObsevedHumanIsInRange = 0;
      }
}
