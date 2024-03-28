#include <SPI.h>
#include <TrueRandom.h>
#include <elapsedMillis.h>
#include <Average.h>

//NEW AUDIO
#include <DFRobot_DF1201S.h>
DFRobot_DF1201S DF1201S;

elapsedMillis timeElapsed;
Average<float> lickAve(3);  //a moving window for lick detection
int session;
int blockLength = 1; //length of left/right licking blocks
int blockCount = 0; //progress within each block
int blockState = 1; //determines left/right assignment of current block
int delayTime=35; //open time for H2O solenoids
int waiting=1;
int lickThresh = 25; //voltage threshold for lick detection
int lickTime = 0;
int startTime = 0; 
int totalDispensed = 0;

int tone_length = 500;
int tone_num = 17;
int tone_num_left = 17;
int tone_num_both = 18;
int tone_num_right = 19;

//ARDUINO PIN ASSIGNMENTS//
int H2OL = 7;
int H2OR = 6;
int roomLight = 53;

void setup() {
  pinMode(H2OL, OUTPUT);
  pinMode(H2OR, OUTPUT);
  pinMode(roomLight, OUTPUT);
    digitalWrite(H2OL, HIGH);
    delay(delayTime*2);
    digitalWrite(H2OL, LOW);
    digitalWrite(H2OR, HIGH);
    delay(delayTime*2);
    digitalWrite(H2OR, LOW);
  Serial.begin(9600);
  digitalWrite(roomLight, HIGH);


//NEW AUDIO
Serial1.begin(115200);
while(!DF1201S.begin(Serial1)){
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
}
DF1201S.setPrompt(false);
DF1201S.setLED(false);
DF1201S.setBaudRate(115200);
DF1201S.setVol(/*VOL = */20); //Sets audio card volume 20 max. 
DF1201S.switchFunction(DF1201S.MUSIC); //This command makes the speakers say "Music"
DF1201S.setPlayMode(DF1201S.SINGLE); //Makes it so the player will stop if it reaches the end of a file

  timeElapsed = 0;
  startTime = millis();
  randomSeed(analogRead(0));

Serial.println("Choose session:");
  Serial.println("(1) Random (HB 1/2)");
  Serial.println("(2) Blocked L/R (HB 3)");
  Serial.println("(3) + Lick-triggered stimuli (HB 3B)");
  Serial.println("(4) + Tone-Prompted Stimuli (HB 3C)");
  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      session = Serial.parseInt();       // get the character
      if (session > 0) {
        waiting = 0;
      }
    }
  }
  
if(session == 1){
        Serial.println(" Starting HB 1 / 2");
      }
else if(session == 2){
        Serial.println("Starting HB 3");
      }
else if(session == 4){
        Serial.println("Starting HB 3C");
      }
else{
        Serial.println("Starting HB 3B");
      }
      
  delay(1000);
  digitalWrite(53,HIGH);
}


void loop()
{
if(session==1){
  randomSeed(analogRead(A3));
  blockLength = random(1, 4);
  blockCount = 0;
  blockState = blockState * (-1);
  while (blockCount < blockLength) {  
    lickAve.push(analogRead(A5) - analogRead(A0));  
    if (lickAve.minimum() > lickThresh && blockState == 1) {
      digitalWrite(H2OR, HIGH);
      delay(delayTime);
      digitalWrite(H2OR, LOW);
      blockCount = blockCount + 1;
      lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }

    if (lickAve.maximum() < (-1)*lickThresh && blockState == (-1)) {
      digitalWrite(H2OL, HIGH);
      delay(delayTime);
      digitalWrite(H2OL, LOW);
      blockCount = blockCount + 1;
      lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4;
      Serial.println(totalDispensed);
      Serial.print(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }
    delay(5);
  }
  }
  
else if(session==2){
  if (totalDispensed < 1200){
  if(blockLength<20){blockLength=blockLength+1;}
  else{blockLength=20;}
  blockCount = 1;
  blockState = blockState * (-1);
  while (blockCount < blockLength) {
    lickAve.push(analogRead(A5) - analogRead(A0));
    if (lickAve.minimum() > lickThresh && blockState == 1) {
      digitalWrite(H2OR, HIGH);
      delay(delayTime);
      digitalWrite(H2OR, LOW);
      blockCount = blockCount + 1;
       lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }
    if (lickAve.maximum() < (-1)*lickThresh && blockState == (-1)) {
      digitalWrite(H2OL, HIGH);
      delay(delayTime);
      digitalWrite(H2OL, LOW);
      blockCount = blockCount + 1;
      lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4                                                                                                                                                                                            ;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }
    delay(5);
  }
  }
  }

else if(session==4){
  if (totalDispensed < 1200){
  if(blockLength<20){blockLength=blockLength+1;}
  else{blockLength=20;}
  blockCount = 1;
  blockState = blockState * (-1);
  while (blockCount < blockLength) {
    lickAve.push(analogRead(A5) - analogRead(A0));
    if (lickAve.minimum() > lickThresh && blockState == 1) {
      PlayTone(tone_num,300);
      digitalWrite(H2OR, HIGH);
      delay(delayTime);
      digitalWrite(H2OR, LOW);
      blockCount = blockCount + 1;
       lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }
    if (lickAve.maximum() < (-1)*lickThresh && blockState == (-1)) {
      PlayTone(tone_num,300);
      digitalWrite(H2OL, HIGH);
      delay(delayTime);
      digitalWrite(H2OL, LOW);
      blockCount = blockCount + 1;
      lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4                                                                                                                                                                                            ;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      delay(random(500, 1500));
    }
    delay(5);
  }
  }
  }
else{
  if (totalDispensed < 1200){
  if(blockLength<20){blockLength=blockLength+1;}
  else{blockLength=20;}
  blockCount = 1;
  blockState = blockState * (-1);
  while (blockCount < blockLength) {
    lickAve.push(analogRead(A5) - analogRead(A0));
    if (lickAve.minimum() > lickThresh && blockState == 1) {
      digitalWrite(H2OR, HIGH);
      delay(delayTime);
      digitalWrite(H2OR, LOW);
      blockCount = blockCount + 1;
       lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 

  DF1201S.playFileNum(7);
  DF1201S.setPlayTime(/*Play Time = */0);
  delay(random(500, 1000));
  DF1201S.setPlayTime(/*Play Time = */3);
  delay(random(1000, 1500));

    }
    if (lickAve.maximum() < (-1)*lickThresh && blockState == (-1)) {
      digitalWrite(H2OL, HIGH);
      delay(delayTime);
      digitalWrite(H2OL, LOW);
      blockCount = blockCount + 1;
      lickTime = (millis() - startTime)/1000;
      totalDispensed = totalDispensed + 4                                                                                                                                                                                            ;
      Serial.println(totalDispensed);
      Serial.println(lickTime);
      Serial.println(" seconds elapsed"); 
      
  DF1201S.playFileNum(9);
  DF1201S.setPlayTime(/*Play Time = */0);
  delay(random(500, 1000));
  DF1201S.setPlayTime(/*Play Time = */3);
  delay(random(1000, 1500));

    }
    delay(5);
  }
  }
  }
    
}

void PlayTone(int tone_num, int tone_length){
  DF1201S.playFileNum(tone_num);
  DF1201S.setPlayTime(/*Play Time = */0);
  delay(tone_length);
  DF1201S.setPlayTime(/*Play Time = */3);
};
