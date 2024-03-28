#include <SPI.h>
#include <TrueRandom.h>
#include <elapsedMillis.h>
#include <Average.h>
elapsedMillis timeElapsed;

#include <DFRobot_DF1201S.h>
DFRobot_DF1201S DF1201S;

Average<float> runAveNG(10);
Average<float> lickAveR(3);
Average<float> lickAveL(3);
Average<float> lickBias(5);
Average<float> setBias(5);
Average<float> runAve(100); //***determines the moving average of block size for criterion
Average<float> runAveL(10);
Average<float> runAveR(10);

int session;
int ix;
int blockTrials = 1; //set to 1 to run blocks of 20 trials, 0 to do simple discrimination
int blockLength = 20;
float critScore = 0.8; //determines criterion score on incongruent trials needed to change blocks
int delayTime = 30; //open time on solenoid
int setID = 1; //initialize to 1 for odor relevant (logged as setID=-1)
int blockNum = 0;
int setChoose [] = { 1, -1};
int trialLR;
int stimLR=0;
int irrelLR=0;
int trialChoose [] = { -1, 1};//{-1,1};//for running without lickBias
int H2OL = 7;
int H2OR = 6;
int odorAirSupply = 4;
int odorPair = 10;
int odorAB = 11; //LOW=A;HIGH=B
int odorCD = 8; //LOW=A;HIGH=B
int setIDOut = 52;
int trialOut = 53;
int trialLROut = 50;
int irrelLROut = 51;
int lickingLOut = 48;
int lickingROut = 49;
int rewardOut = 46;
float hiScore = 0;
int brightness = 250;
int fadeAmount = 5;
int lickThresh = 25;
int lickPrev = 0;
int odorID = 1; //0=A,1=B
int toneID = 1;
int RT = 0;
int ITI = 0;
int trial = 0;
int lickedR = 0;
int lickedL = 0;
int lickNumR = 0;
int lickNumL = 0;
int trialCorrect = 0;
int totalDispensed = 0;
unsigned long startTime = 0;
String sessionNames[] =  {"Shaping(4)", "Shaping(5)"};
String stim[] = {"Whisker", "Odor"};


void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT); digitalWrite(9, LOW);
  pinMode(12, OUTPUT); digitalWrite(12, LOW);
  pinMode(H2OL, OUTPUT);  digitalWrite(H2OL, LOW);
  pinMode(H2OR, OUTPUT); digitalWrite(H2OR, LOW);
  pinMode(setIDOut, OUTPUT); digitalWrite(setIDOut, LOW);
  pinMode(trialOut, OUTPUT); digitalWrite(trialOut, LOW);
  pinMode(trialLROut, OUTPUT); digitalWrite(trialLROut, LOW);
  pinMode(irrelLROut, OUTPUT); digitalWrite(irrelLROut, LOW);
  pinMode(lickingLOut, OUTPUT); digitalWrite(lickingLOut, LOW);
  pinMode(lickingROut, OUTPUT); digitalWrite(lickingROut, LOW);
  pinMode(rewardOut, OUTPUT); digitalWrite(rewardOut, LOW);
  Serial1.begin(115200);
  while (!DF1201S.begin(Serial1)) {
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }
  DF1201S.setPrompt(false);
  DF1201S.setLED(false);
  DF1201S.setBaudRate(115200);
  DF1201S.setVol(/*VOL = */20); //Sets audio card volume 20 max.
  DF1201S.switchFunction(DF1201S.MUSIC); //This command makes the speakers say "Music"
  DF1201S.setPlayMode(DF1201S.SINGLE); //Makes it so the player will stop if it reaches the end of a file
  pinMode(odorAirSupply, OUTPUT); digitalWrite(odorAirSupply, LOW);
  pinMode(odorPair, OUTPUT); digitalWrite(odorPair, LOW);
  pinMode(odorAB, OUTPUT); digitalWrite(odorAB, LOW); //Low=A,High=B;
  pinMode(odorCD, OUTPUT); digitalWrite(odorCD, LOW);
  digitalWrite(H2OL, HIGH);
  delay(delayTime);
  digitalWrite(H2OL, LOW);
  digitalWrite(H2OR, HIGH);
  delay(delayTime);
  digitalWrite(H2OR, LOW);
  int waiting = 1;
  delay(500);
  Serial.println("Choose session:");
  Serial.println("(1) SH4"); Serial.println("(2) SH5");


  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      session = Serial.parseInt();       // get the character
      if (session > 0) {
        waiting = 0;
      }
    }
  }
  waiting = 1;
  delay(500);
  Serial.println("Choose Stimulus:");
  Serial.println("(1) Whisker (2) Odor");

  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      ix = Serial.parseInt();       // get the character
      if (ix > 0) {
        waiting = 0;
      }
    }
  }
  setID = (setChoose[ix - 1]);

  Serial.print("Begin Session ");
  Serial.println(sessionNames[session - 1] + " with " + stim[ix - 1]);

  for (int q = 0; q < 100; q++) {
    runAve.push(0);
    runAveNG.push(1);
  }

  timeElapsed = 0;
  startTime = millis();
  randomSeed(analogRead(0));
  trialLR = trialChoose[random(0, 2)];

  Serial.println(" ");
  Serial.print("trial"); Serial.print(" ");
  Serial.print("blockNum"); Serial.print(" ");
  Serial.print("ITI"); Serial.print(" ");
  Serial.print("setID"); Serial.print(" ");
  Serial.print("trialLR"); Serial.print(" ");
  Serial.print("stimLR"); Serial.print(" ");
  Serial.print("irrelLR"); Serial.print(" ");
  Serial.print("lickNumL"); Serial.print(" ");
  Serial.print("lickNumR"); Serial.print(" ");
  Serial.print("RT"); Serial.print(" ");
  Serial.print("trialCorrect"); Serial.print(" ");
  Serial.print("runAveL"); Serial.print(" ");
  Serial.print("runAveR"); Serial.print(" ");
  Serial.print("runAve"); Serial.print(" ");
  Serial.print("runAveNG"); Serial.print(" ");
  Serial.print("totalDispensed"); Serial.print(" ");
  delay(1000);
}



void loop() {

  trialLR = trialLR * (-1); //1=R, -1=L
  blockNum = 0;
  if (blockTrials == 0) {
    blockLength = random(1, 5);
  }
  if (session == 1) {
    if (trialLR == -1) {
      digitalWrite(H2OL, HIGH);
      delay(delayTime);
      digitalWrite(H2OL, LOW);
    }
    else {
      digitalWrite(H2OR, HIGH);
      delay(delayTime);
      digitalWrite(H2OR, LOW);
    }
  }
  while (blockNum < blockLength) {


    if (totalDispensed < 1100 && runAveNG.mean() >= 0.1 ) {
      blockNum = blockNum + 1;
      trial = trial + 1;
      ITI = timeElapsed;
      RT = 0;
      lickedR = 0; lickedL = 0;
      lickNumL = 0; lickNumR = 0;
      trialCorrect = 0;


      DF1201S.playFileNum(18);
      DF1201S.setPlayTime(/*Play Time = */0);
      digitalWrite(trialOut, HIGH);
      delay(500);
      DF1201S.setPlayTime(/*Play Time = */3);
      delay(50);
      timeElapsed = 0;


      //BEGIN WHISKER STIM PRESENTATION
      if (setID == 1) { //ODOR PRESENTATION
        if (setID == -1) {
          if (trialLR == 1) {
            digitalWrite(odorAB, HIGH);
            digitalWrite(odorAirSupply, HIGH);
          } else {
            digitalWrite(odorAirSupply, HIGH);
          }
        }
        if (trialLR == 1) {
          DF1201S.playFileNum(7);
          DF1201S.setPlayTime(/*Play Time = */0);
          digitalWrite(trialLROut, HIGH);
        }
        else {
          DF1201S.playFileNum(9);
          DF1201S.setPlayTime(/*Play Time = */0);
          digitalWrite(trialLROut, LOW);
        }
      }

      while (timeElapsed <= 2500) {
        lickedR = 0; lickedL = 0;
        lickAveR.push(analogRead(A5) - analogRead(A0));
        lickAveL.push(analogRead(A0) - analogRead(A5));
        if (lickAveR.minimum() > lickThresh) {
          lickedR = 1;
          digitalWrite(lickingROut, HIGH);
          delay(20);
          digitalWrite(lickingROut, LOW);
        }
        else if (lickAveL.minimum() > lickThresh) {
          lickedL = 1;
          digitalWrite(lickingLOut, HIGH);
          delay(20);
          digitalWrite(lickingLOut, LOW);
        }
        if (lickPrev == 0 && lickedR == 1) {
          lickNumR = lickNumR + 1;
        }
        else if (lickPrev == 0 && lickedL == 1) {
          lickNumL = lickNumL + 1;
        }
        if (lickedR == 1 || lickedL == 1) {
          lickPrev = 1;
        }
        else {
          lickPrev = 0;
        }
        delay(5);
      }

      DF1201S.setPlayTime(/*Play Time = */3);
      digitalWrite(odorAB, LOW);
      digitalWrite(odorCD, LOW);
      digitalWrite(odorAirSupply, LOW);
      digitalWrite(odorPair, LOW);
      digitalWrite(trialLROut, LOW);

      delay(50);
      lickedR = 0; lickedL = 0;
      lickAveR.push(0); lickAveR.push(0); lickAveR.push(0);
      lickAveL.push(0); lickAveL.push(0); lickAveL.push(0);
      if (session == 1) {
        if (trialLR == 1) {
          while (lickedR == 0 && timeElapsed <= 4000) {
            lickAveR.push(analogRead(A5) - analogRead(A0));
            lickAveL.push(analogRead(A0) - analogRead(A5));
            if (lickAveR.minimum() > lickThresh) {
              lickedR = 1;
              digitalWrite(lickingROut, HIGH); delay(20); digitalWrite(lickingROut, LOW);
              RT = timeElapsed - 2500;
            }
          }
        }
        else if (trialLR == -1) {
          while (lickedL == 0 && timeElapsed <= 4000) {
            lickAveR.push(analogRead(A5) - analogRead(A0));
            lickAveL.push(analogRead(A0) - analogRead(A5));
            if (lickAveL.minimum() > lickThresh) {
              lickedL = 1;
              digitalWrite(lickingLOut, HIGH); delay(20); digitalWrite(lickingLOut, LOW);
              RT = timeElapsed - 2500;
            }
          }
        }
        if (lickedR == 1) {
          if (trialLR == 1) {
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
            trialCorrect = 1;
            runAve.push(trialCorrect);
            digitalWrite(H2OR, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OR, LOW);digitalWrite(rewardOut, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else {
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
            runAve.push(0);
            digitalWrite(H2OL, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
          }
        }
        else if (lickedL == 1) {
          if (trialLR == -1) {
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
            trialCorrect = 1;
            runAve.push(trialCorrect);
            digitalWrite(H2OL, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else {
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
            runAve.push(0);
            digitalWrite(H2OR, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OR, LOW);digitalWrite(rewardOut, LOW);
          }
        }
        else if (runAveNG.mean() <= 0.7 && runAveNG.mean() >= 0.3) {
          if (trialLR == 1) {
            digitalWrite(H2OR, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OR, LOW);digitalWrite(rewardOut, LOW);
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
            totalDispensed = totalDispensed + 4;
          }
          else {
            digitalWrite(H2OL, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
            totalDispensed = totalDispensed + 4;
          }
        }

        if (lickedL == 1 || lickedR == 1) {
          runAveNG.push(1);
        }
        else {
          runAveNG.push(0);
        }

      }

      else {      //if session is not SH4
        timeElapsed = 0;
        while (lickedR == 0 && lickedL == 0 && timeElapsed <= 1500) {
          lickAveR.push(analogRead(A5) - analogRead(A0));
          lickAveL.push(analogRead(A0) - analogRead(A5));
          if (lickAveR.minimum() > lickThresh) {
            lickedR = 1;
            RT = timeElapsed;
          }
          else if (lickAveL.minimum() > lickThresh)  {
            lickedL = 1;
            RT = timeElapsed;
          }
        }

    
        if (trialLR == 1) {
          if (lickedR == 1) {
            trialCorrect = 1;
            runAveR.push(1);
          }
          if (trialCorrect == 1) {
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
            digitalWrite(H2OR, HIGH);
            delay(delayTime);
            digitalWrite(H2OR, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else if (runAveNG.mean() >= 0.3 && runAveNG.mean() <= 0.6) {
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
            digitalWrite(H2OR, HIGH);
            delay(delayTime);
            digitalWrite(H2OR, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else if (lickedL == 1) {
            runAveR.push(0);
            DF1201S.playFileNum(19);
            DF1201S.setPlayTime(/*Play Time = */0);
          }
        }
        else if (trialLR == -1) {
          if (lickedL == 1) {
            trialCorrect = 1;
            runAveL.push(1);
          }
          if (trialCorrect == 1) {
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
            digitalWrite(H2OL, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else if (runAveNG.mean() >= 0.3 && runAveNG.mean() <= 0.6) {
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
            digitalWrite(H2OL, HIGH);digitalWrite(rewardOut, HIGH);
            delay(delayTime);
            digitalWrite(H2OL, LOW);digitalWrite(rewardOut, LOW);
            totalDispensed = totalDispensed + 4;
          }
          else if (lickedR == 1) {
            runAveL.push(0);
            DF1201S.playFileNum(17);
            DF1201S.setPlayTime(/*Play Time = */0);
          }
        }

        if (lickedL == 1 || lickedR == 1) {
          runAveNG.push(1);
          runAve.push(trialCorrect);
        }
        else {
          runAveNG.push(0);
        }
      }

      
      Serial.println(" ");
      Serial.print(trial); Serial.print(" ");
      Serial.print(blockNum); Serial.print(" ");
      Serial.print(ITI); Serial.print(" ");
      Serial.print(setID); Serial.print(" ");
      Serial.print(trialLR); Serial.print(" ");
      Serial.print(stimLR); Serial.print(" ");
      Serial.print(irrelLR); Serial.print(" ");
      Serial.print(lickNumL); Serial.print(" ");
      Serial.print(lickNumR); Serial.print(" ");
      Serial.print(RT); Serial.print(" ");
      Serial.print(trialCorrect); Serial.print(" ");
      Serial.print(runAveL.mean()); Serial.print(" ");
      Serial.print(runAveR.mean()); Serial.print(" ");
      Serial.print(runAve.mean()); Serial.print(" ");
      Serial.print(runAveNG.mean()); Serial.print(" ");
      Serial.print(totalDispensed); Serial.print(" ");

      digitalWrite(trialOut, LOW);
      if (runAve.mean() > hiScore) {
        hiScore = runAve.mean();
      }

      timeElapsed = 0;
      delay(random(3000, 4500));
    }
    else {
      digitalWrite(H2OL, LOW); digitalWrite(H2OR, LOW);
      digitalWrite(odorAirSupply, LOW); digitalWrite(odorAB, LOW); digitalWrite(odorCD, LOW); digitalWrite(odorPair, LOW);
      for (int i = 0; i <= 10; i++) {
        Serial.println();
      }
      if (session == 1) {
        Serial.print("Session finished, "); Serial.print(totalDispensed);
        Serial.print("uL consumed. Animal has ");
        if (totalDispensed < 500) {
          Serial.print("NOT");
        }
      }
      else {
        Serial.print("Session finished, score is "); Serial.print(hiScore * 100); Serial.println("%");
        Serial.print("Animal has ");
        if (hiScore < 0.8) {  //requirement of >80% perf at session end
          Serial.print("NOT");
        }
      }

      Serial.print(" passed. Please log score in spreadsheet.");
      while (1) {
        analogWrite(13, brightness);
        brightness = brightness + fadeAmount;
        if (brightness == 0 || brightness == 255) {
          fadeAmount = -fadeAmount;
        }
        delay(10);
      }
    }
  }
}
