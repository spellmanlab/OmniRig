#include <SPI.h>
#include <TrueRandom.h>
#include <elapsedMillis.h>
#include <Average.h>
elapsedMillis timeElapsed;

#include <DFRobot_DF1201S.h>
DFRobot_DF1201S DF1201S;
int H2OL = 7;
int H2OR = 6;
int odorAirSupply = 5;
int odorPair = 10;
int odorAB = 11;  //LOW=A;HIGH=B
int odorCD = 8;   //LOW=C;HIGH=D
int optoChan = 38;
int whiskerVolumes[] = { 5, 10, 25, 50 };
int waiting = 1;                        //used in while-loops
int session = 0;
int runLoop = 1;

void setup() {
  Serial1.begin(115200);
  while (!DF1201S.begin(Serial1)) {
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }
  DF1201S.setPrompt(false);
  DF1201S.setLED(false);
  DF1201S.setVol(/*VOL = */ 20);          //Sets audio card volume 20 max.
  DF1201S.switchFunction(DF1201S.MUSIC);  //This command makes the speakers say "Music"
  DF1201S.setPlayMode(DF1201S.SINGLE);    //Makes it so the player will stop if it reaches the end of a file
                                          //END NEW

  pinMode(3, OUTPUT);
  pinMode(2, INPUT);
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  pinMode(H2OL, OUTPUT);
  pinMode(H2OR, OUTPUT);
  pinMode(odorAirSupply, OUTPUT);
  pinMode(odorPair, OUTPUT);
  pinMode(odorAB, OUTPUT);  //Low=A,High=B;
  pinMode(odorCD, OUTPUT);  //Low=C,High=D;
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(53, OUTPUT);
  digitalWrite(H2OL, LOW);
  digitalWrite(H2OR, LOW);
  digitalWrite(9, LOW);
  digitalWrite(odorAirSupply, LOW);
  digitalWrite(odorPair, LOW);
  digitalWrite(odorAB, LOW);
  digitalWrite(odorCD, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(38, LOW);
  digitalWrite(40, LOW);
  digitalWrite(41, LOW);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  digitalWrite(46, LOW);
  digitalWrite(47, LOW);
  digitalWrite(48, LOW);
  digitalWrite(49, LOW);
  digitalWrite(50, LOW);
  digitalWrite(51, LOW);
  digitalWrite(52, LOW);
  digitalWrite(53, LOW);
  timeElapsed = 0;
  randomSeed(analogRead(0));


  Serial.println("How many test loops?");
  while (waiting == 1) {
    if (Serial.available() > 0) {   // is a character available?
      runLoop = Serial.parseInt();  // get the character
      if (runLoop > 0) {
        waiting = 0;
      }
    }
  }
  waiting=1;

  Serial.println("Choose action:");
  Serial.println("(1) Test Water Lines");
  Serial.println("(2) Play Tones");
  Serial.println("(3) Test Odorant Lines");
  Serial.println("(4) Test Lick Detectors");
  Serial.println("(5) Opto Pulses");

}

void loop() {

  while (waiting == 1) {
    if (Serial.available() > 0) {   // is a character available?
      session = Serial.parseInt();  // get the character
      if (session > 0) {
        waiting = 0;
      }
    }
  }
  waiting=1;

  if (session == 1) {
    //TEST WATER SPOUTS
    for (int i=1; i<=runLoop; i++){
      delay(100);
        Serial.println("Testing left water spout");
      digitalWrite(H2OL, HIGH);
      delay(35);
      digitalWrite(H2OL, LOW);
      delay(1000);
      Serial.println("Testing right water spout");
      digitalWrite(H2OR, HIGH);
      delay(35);
      digitalWrite(H2OR, LOW);
      delay(1000);
    }
  }

  else if (session == 2) {
    Serial.println("Testing tones");
    for (int i=1; i<=runLoop; i++){
    // TEST TONES//
      delay(1000);
      DF1201S.playFileNum(17);
      DF1201S.setPlayTime(/*Play Time = */ 0);
      delay(500);
      DF1201S.setPlayTime(/*Play Time = */ 4);
      delay(1000);
  
      DF1201S.playFileNum(19);
      DF1201S.setPlayTime(/*Play Time = */ 0);
      delay(500);
      DF1201S.setPlayTime(/*Play Time = */ 4);

    }    
  } 
  
    else if (session == 3) {
    Serial.println("Testing odorant lines");
    for (int i=1; i<=runLoop; i++){
      ////// TEST ODORANT LINES //
      digitalWrite(odorAirSupply, HIGH);
      digitalWrite(odorPair, LOW);
      digitalWrite(odorAB, LOW);
      delay(2000);
      digitalWrite(odorAB, HIGH);
      delay(1000);
      digitalWrite(odorAB, LOW);
      digitalWrite(odorPair, HIGH);
      digitalWrite(odorCD, LOW);
      delay(1000);
      digitalWrite(odorCD, HIGH);
      delay(1000);
      digitalWrite(odorAirSupply, LOW);
      digitalWrite(odorPair, LOW);
      digitalWrite(odorAB, LOW);
      digitalWrite(odorCD, LOW);
      delay(2000);
    }
  }

  else if (session == 4) {
    timeElapsed = 0;
    Serial.println("Testing lick detectors!");
    while(timeElapsed < 10000){
    //// TEST LICK DETECTORS //
    Serial.println(analogRead(A0) - analogRead(A5));
    delay(10);
    }
      Serial.println("Choose action:");
      Serial.println("(1) Test Water Lines");
      Serial.println("(2) Play Tones");
      Serial.println("(3) Test Odorant Lines");
      Serial.println("(4) Test Lick Detectors");
      Serial.println("(5) Opto Pulses");
  }

  else if (session == 5) {
    Serial.println("Testing Opto Pulses!");
    for (int i=1; i<=runLoop; i++){
      ///// TEST OPTO PULSES //////
      digitalWrite(46,HIGH);
      digitalWrite(optoChan, HIGH);
      delay(100);
      digitalWrite(optoChan, LOW);
      digitalWrite(46,LOW);
      delay(4000);
    }
  }


  delay(1000);

}

;
