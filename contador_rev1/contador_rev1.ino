/*

      ___           ___           ___           ___                       ___           ___       ___
     /\  \         /\__\         /\  \         /\  \          ___        /\  \         /\__\     /\  \
    /::\  \       /::|  |       /::\  \       /::\  \        /\  \      /::\  \       /:/  /    /::\  \
   /:/\:\  \     /:|:|  |      /:/\:\  \     /:/\:\  \       \:\  \    /:/\:\  \     /:/  /    /:/\:\  \
  /::\~\:\  \   /:/|:|__|__   /::\~\:\  \   /:/  \:\  \      /::\__\  /:/  \:\  \   /:/  /    /:/  \:\  \
 /:/\:\ \:\__\ /:/ |::::\__\ /:/\:\ \:\__\ /:/__/ \:\__\  __/:/\/__/ /:/__/ \:\__\ /:/__/    /:/__/ \:\__\
 \/__\:\/:/  / \/__/~~/:/  / \:\~\:\ \/__/ \:\  \  \/__/ /\/:/  /    \:\  \  \/__/ \:\  \    \:\  \ /:/  /
      \::/  /        /:/  /   \:\ \:\__\    \:\  \       \::/__/      \:\  \        \:\  \    \:\  /:/  /
      /:/  /        /:/  /     \:\ \/__/     \:\  \       \:\__\       \:\  \        \:\  \    \:\/:/  /
     /:/  /        /:/  /       \:\__\        \:\__\       \/__/        \:\__\        \:\__\    \::/  /
     \/__/         \/__/         \/__/         \/__/                     \/__/         \/__/     \/__/

*/
#include <NewPing.h> // Biblioteca para facilitar o ping
#include <SD.h> // Biblioteca de leitura do cartão SD
#include <SPI.h>
#include <Wire.h>
//#include "RTClib.h" // Biblioteca para uso do RTC
// Contar o tempo e data em que ele começou a contar.

//RTC_DS1307 RTC;

#define LED 13

#define TRIGGER_PIN  9
#define ECHO_PIN     10

#define MAX_DISTANCE 80
#define MIN_DISTANCE 0;

const int chipSelect = 4;

int humanCount = 0;

boolean iSeeHuman = false;
boolean iTrackHuman = false;
long lastTimeISawHuman;

int dist_cm = 0;

// assim que observamos uma pessoa, nos contamos um número de vezes que essa pessoa fica na frente do dispositivo
int numberOfPingsObsevedHumanIsInRange = 0;

// quantia mínima de "pingadas" que um objeto deve receber para ser considerado um humano
static int minPingCountToAcceptHuman = 1;

static float maxBlackoutInMillis = 100;
static int iSeeHumanThresholdInCm = 50;

// quando setado TRUE, o led vai blinkar toda vez que detectar um humano
const boolean DEBUG = false;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

  void printHistory()
  {
    Serial.write("Historico\n\n");
      //Serial.print("Hora: ");
      //Serial.println(tempo);
      Serial.print("Contador: ");
      Serial.println(humanCount);
      Serial.println();
    }

  void blinkHumanCountedLed(){
    for(int i = 0; i < 5; i++){
      digitalWrite(LED, HIGH);
      delay(50);
      digitalWrite(LED, LOW);
      delay(50);
    }
  }


/*  void LogToSD(){
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

void setup() {
  
    Serial.begin(9600);

//  if(DEBUG){
    
    pinMode(LED, OUTPUT);
    blinkHumanCountedLed();
//    Wire.begin();
    //RTC.begin();
//  Serial.print("Iniciando cartão SD...");

//  if (!SD.begin(chipSelect)) {
//    Serial.println("O cartão SD falhou ou não está presente");
    // don't do anything more:
//    return;
//  }
//  Serial.println("Cartão Inicializado.");
//}

  /*  if (! RTC.isrunning()) {
    Serial.println("RTC não está rodando");
    // following line sets the RTC to the date & time this sketch was compiled
    // uncomment it & upload to set the time, date and start run the RTC!
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }*/
    delay(500);
    printHistory();
}

void loop() {
  // sensor.ping_cm()sera utilizado no lugar de pingDistanceInCm()
  // melhor usar uma variavel no lugar do sensor.ping_cm()
  // iSeeHuman = sensor.ping_cm() < iSeeHumanThresholdInCm && sensor.ping_cm() > 5
  //dist_cm = pingDistanceInCm();
  //DateTime now = RTC.now(); // Objeto DateTime
int  dist_cm = sonar.ping_cm();
  iSeeHuman = dist_cm < iSeeHumanThresholdInCm;// && dist_cm > MIN_DISTANCE;
 
//  if(dist_cm > 0) {
  Serial.print(iSeeHuman);
  Serial.print(",");
  Serial.print(humanCount);
  Serial.print(",");
  Serial.print(dist_cm);
  Serial.print(",");
  Serial.println(millis());
//  }




  if(iSeeHuman && !iTrackHuman) // we have first contact with human
  {
    numberOfPingsObsevedHumanIsInRange += 1;
    iTrackHuman = true;
    lastTimeISawHuman = millis();
  }
  else if(iSeeHuman && iTrackHuman){
    if((millis() - lastTimeISawHuman) < maxBlackoutInMillis * (1+2*(numberOfPingsObsevedHumanIsInRange==1)))
    {
      // wo dont see him now, but we believe he may still be in sight
    }
    else
    {
      // we havent seen him for too long
        iTrackHuman = false;

        if(numberOfPingsObsevedHumanIsInRange == minPingCountToAcceptHuman){
          humanCount++;
          if(DEBUG)
            blinkHumanCountedLed();
          //LogToSD();
    }
  }
}
else
{
      numberOfPingsObsevedHumanIsInRange = 0;
}
}

