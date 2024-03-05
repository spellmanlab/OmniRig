//INCLUDE RELEVANT LIBRARIES
#include <SPI.h>
#include <TrueRandom.h>
#include <elapsedMillis.h>
#include <Average.h>
#include <DFRobot_DF1201S.h>
elapsedMillis timeElapsed;
DFRobot_DF1201S DF1201S;


//SET SESSION-SPECIFIC PARAMETERS
float prop = 10;                //sets proportion of trials that are incongruent (this number over 20)
float critScore = 0.8;          //sets criterion score needed to change blocks, set >1 to never reach
int delayTime = 15;             //open time on water solenoid, in ms (~15ms for 3psi)
int LBCorrect = 1;              //actively correct for side bias? 1=Y; 0=N;
int trialStartTone=18;          
int whiteNoiseRight=19;
int whiteNoiseLeft=17;
int tone1=9; 
int tone2=7;


//INITIALIZE SESSION-SPECIFIC PLACEHOLDER VARIABLES
int session;                    //index within the corresponding sessionNames array
String sessionNames[] =  {"Simple Discrimination", "Compound Discrimination", "Intradimensional Shift", "Reversal", "Extradimensional shift", "Intradimensional shift B" , "Serial Extradimensional Shifts"};
int lightStimYN;                //whether to deliver optogenetic stimuli, 1 = No; 2 = Yes
int keyDetect;                  //will be used to input data at session outset
int A;                          //will be used to input data at session outset
int seqNum = 0;                 //set to 0 to start from beginning of set sequence in setChoose
int setChoose [] = {1, -1};     //assign set -- 1 for whisker, -1 for odor
int blockMax = 1;               //maximum number of blocks (sets) in a session
int mapID = 1;                  //used to reverse the left-right stimulus mapping on and after the Reversal session
int odorID = 1;                 //0=A,1=B
int toneID = 1;
int trialMax = 350;                     //maximum length of a session
int toneDur = 500;                      //duration of trial initiation tone
int volMax = 100;                        //sound card output volume (max 255)
int trialChoose [] = { -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1};  //used to select left/right (-1/1) reward location for each trial
int irrelChoose [] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // choose irrelevant stim -- -1 for incongruent, 1 for congruent
int crit = 0;                           //criterion reached (1) or not (0)
float lickThresh = 20;                  //threshold voltage difference for lick detection
int lickPrev = 0;                       //whether the mouse has already licked within a given time window
int waiting = 1;                        //used in while-loops
int brightness = 250;                   //for overhead light
int fadeAmount = 5;                     //for overhead light


//INITIALIZE TRIAL VARIABLES
int toneLR;
int odorLR;
int blockNum = 0;
int setID;
int trialLR;
int stimLR;
int irrelLR;
int blockTrial;


//ASSIGN VARIABLES TO ARDUINO PINS
int LEDChannel = 40;
int optoChannel = 38;
int H2OL = 7;
int H2OR = 6;
int odorAirSupply = 5;
int odorPair = 10;
int odorAB = 11;
int odorCD = 8;
int currO = odorAB;
int setIDOut = 52;
int trialOut = 53;
int trialLROut = 50;
int irrelLROut = 51;
int lickingLOut = 48;
int lickingROut = 49;
int rewardOut = 46;


//INITIALIZE PERFORMANCE VARIABLES
Average<float> runAveNG(10);    //initialize a moving window for tracking motivation
Average<float> lickAveR(3);     //initialize a moving window for detecting right licks
Average<float> lickAveL(3);     //initialize a moving window for detecting left licks
Average<float> lickBias(10);    //initialize a moving window for tracking side bias
Average<float> runAveL(10);     //initialize a moving window for tracking left trial accuracy
Average<float> runAveR(10);     //initialize a moving window for tracking right trial accuracy
Average<float> runAve(30);      //initialize a moving window for tracking overall accuracy
int RT = 0;
int ITI = 0;
int trial = 0;
int lickedR = 0;
int lickedL = 0;
int lickNumR = 0;
int lickNumL = 0;
int trialCorrect = 0;
int totalDispensed = 0;
int response = 0;


//SETUP SECTION
void setup() {
  Serial.begin(9600);
  pinMode(LEDChannel, OUTPUT); digitalWrite(LEDChannel, HIGH);
  pinMode(optoChannel, OUTPUT); digitalWrite(LEDChannel, LOW);
  pinMode(12, OUTPUT); digitalWrite(12, LOW);
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT); digitalWrite(9, LOW);
  pinMode(H2OL, OUTPUT);
  pinMode(H2OR, OUTPUT);


//START AUDIO
  Serial1.begin(115200);
  while(!DF1201S.begin(Serial1)){
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }
  DF1201S.setPrompt(false);
  DF1201S.setLED(false);
  DF1201S.setVol(/*VOL = */14); //Sets audio card volume 20 max. 
  DF1201S.switchFunction(DF1201S.MUSIC); //This command makes the speakers say "Music"
  DF1201S.setPlayMode(DF1201S.SINGLE); //Makes it so the player will stop if it reaches the end of a file

  analogWrite(LEDChannel, brightness);
  pinMode(odorAirSupply, OUTPUT); digitalWrite(odorAirSupply, LOW);
  pinMode(odorPair, OUTPUT); digitalWrite(odorPair, LOW);
  pinMode(odorAB, OUTPUT); digitalWrite(odorAB, LOW); //Low=A,High=B;
  pinMode(odorCD, OUTPUT); digitalWrite(odorCD, LOW); //Low=C,High=D;
  digitalWrite(H2OL, LOW);
  digitalWrite(H2OR, LOW);
  pinMode(setIDOut, OUTPUT); digitalWrite(setIDOut, LOW);
  pinMode(trialOut, OUTPUT); digitalWrite(trialOut, LOW);
  pinMode(trialLROut, OUTPUT); digitalWrite(trialLROut, LOW);
  pinMode(irrelLROut, OUTPUT); digitalWrite(irrelLROut, LOW);
  pinMode(lickingLOut, OUTPUT); digitalWrite(lickingLOut, LOW);
  pinMode(lickingROut, OUTPUT); digitalWrite(lickingROut, LOW);
  pinMode(rewardOut, OUTPUT); digitalWrite(rewardOut, LOW);
  digitalWrite(H2OL, HIGH);
  delay(delayTime);
  digitalWrite(H2OL, LOW);
  digitalWrite(H2OR, HIGH);
  delay(delayTime);
  digitalWrite(H2OR, LOW);
  randomSeed(analogRead(0));
  for (int q = 0; q < 10; q++) {
    runAveNG.push(1);
  }


  //CHOOSE SESSION
  delay(500);
  waiting = 1;
  Serial.println("");
  Serial.println("Choose session:");
  Serial.println("(0) SD"); Serial.println("(1) CD"); Serial.println("(2) IDS"); Serial.println("(3) Rev"); Serial.println("(4) EDS"); Serial.println("(5) IDSB"); Serial.println("(6) SEDS");
  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      session = Serial.parseInt();       // get the character
      if (session >= 0 && session <= 6) {
        waiting = 0;
      }
    }
  }

  delay(500);
  waiting = 1;
  Serial.println("");
  Serial.println("Opto stimulation during ITI ?");
  Serial.println("(1) No"); Serial.println("(2) Yes");
  while (waiting == 1) {
    if (Serial.available() > 0) {    // is a character available?
      lightStimYN = Serial.parseInt();       // get the character
      if (lightStimYN == 1 || lightStimYN == 2) {
        waiting = 0;
      }
    }
  }

  //SET SESSION-SPECIFIC PARAMETERS
  if (session >= 2) {         //ID shift, new whisker stimuli
    tone1=4;
    tone2=6;
  }
  if (session >= 3) {
    mapID = -1;               //Reversal and all sessions after
  }
  if (session >= 4) {
    seqNum = 1;               //ED shift 1
  }
  if (session >= 5) {
    currO = odorCD;           //switch odorants
  }
  if (session < 6) {          // set icg/cg proportion
    for (int i = 0; i < 11; i++) {
      irrelChoose[i] =  irrelChoose[i] * (-1);
    }
  }
  else {                      //for SEDS sessions
    for (int i = 0; i < prop; i++) {
      irrelChoose[i] = irrelChoose[i] * (-1);
    }
    blockMax = 20;
    seqNum = 0;               //start with whisker
    trialMax = 1000;
  }
  Serial.print("Begin Session ");
  Serial.println(sessionNames[session]);
  Serial.println("Press any key to begin");
  Serial.println("");
  A = 1;
  while (Serial.available() > 0) {
    Serial.read();
  }
  while (A == 1) {
    if (Serial.available()) {    // is a character available?
      waiting = 0;
      A = 0;
    }
  }

  digitalWrite(odorAirSupply, LOW);
  digitalWrite(odorPair, LOW);
  digitalWrite(odorAB, LOW); //Low=A,High=B;
  digitalWrite(odorCD, LOW);
  Serial.println("");
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
  Serial.print("response"); Serial.print(" ");
  Serial.print("trialCorrect"); Serial.print(" ");
  Serial.print("runAveL"); Serial.print(" ");
  Serial.print("runAveR"); Serial.print(" ");
  Serial.print("runAveNG"); Serial.print(" ");
  Serial.print("runAve"); Serial.print(" ");
  Serial.print("totalDispensed"); Serial.print(" ");
  delay(3000);
  timeElapsed = 0;
}














//START TASK
void loop() {

  crit = 0;
  blockNum = blockNum + 1;
  blockTrial = 0;


  //SET RELEVANT STIMULUS MODALITY
  if (blockNum > 1) {
    setID = setID * -1;
  }
  else {
    setID = setChoose[seqNum];
  }
  if (setID == -1) {
    digitalWrite(setIDOut, LOW); //send data out for continuous logging, for imaging
  }
  else {
    digitalWrite(setIDOut, HIGH);
  }


  //RE-INITIALIZE PERFORMANCE TRACKING VARIABLES
  for (int q = 0; q < 30; q++) {
    runAveL.push(0);
    runAveR.push(0);
    runAve.push(0);
  }


  //TRIAL BLOCK
  while (crit == 0 || session < 6) {
    if (trial <= trialMax && totalDispensed < 1100 && runAveNG.mean() >= 0.2 && blockNum <= blockMax && (crit == 0 || session < 6)) {
      trial = trial + 1;
      blockTrial = blockTrial + 1;


      //CHOOSE TRIAL STIMULI
      if (LBCorrect == 1) {
        trialLR = trialChoose[round(random(3 + 2 * lickBias.mean(), 9 + 2 * lickBias.mean()))];
      }
      else {
        trialLR = trialChoose[random(0, 2)];
      }
      
      stimLR = trialLR * mapID;                         //because of reversal, stimLR and trialLR are separate variables
      irrelLR = irrelChoose[random(0, 20)] * stimLR;    //set trial to be congruent or incongruent
      RT = 0;
      lickedR = 0; lickedL = 0;
      lickNumL = 0; lickNumR = 0;
      trialCorrect = 0;
      if (setID == -1) {
        odorLR = stimLR;
        toneLR = irrelLR;
      }
      else {
        odorLR = irrelLR;
        toneLR = stimLR;
      }


      //BEGIN TRIAL
      digitalWrite(trialOut, HIGH);


      //PLAY TRIAL START TONE
      DF1201S.playFileNum(trialStartTone);
      DF1201S.setPlayTime(/*Play Time = */0);
      digitalWrite(trialOut, HIGH);
      delay(toneDur);


      //BEGIN ODOR PRESENTATION
      if (session > 0){
        if (currO == odorCD) {
          digitalWrite(odorPair, HIGH);
        }
        if (odorLR == -1) {
          digitalWrite(currO, LOW);
          digitalWrite(odorAirSupply, HIGH);
        }
        else {
          digitalWrite(currO, HIGH);
          digitalWrite(odorAirSupply, HIGH);
        }
      }
       DF1201S.setPlayTime(/*Play Time = */3);
      delay(50);
      timeElapsed = 0;

      if (trialLR == -1) {
        digitalWrite(trialLROut, LOW); //send data out for continuous logging, for imaging
      }
      else {
        digitalWrite(trialLROut, HIGH);
      }
      if (irrelLR == -1) {
        digitalWrite(irrelLROut, LOW);
      }
      else {
        digitalWrite(irrelLROut, HIGH);
      }


      //BEGIN WHISKER STIM PRESENTATION
      if (toneLR == 1) {
        DF1201S.playFileNum(tone2);
        DF1201S.setPlayTime(/*Play Time = */0);
      }
      else {
        DF1201S.playFileNum(tone1);
        DF1201S.setPlayTime(/*Play Time = */0);
      }


      //DETECT LICKS DURING STIMULUS PRESENTATION
      while (timeElapsed <= 2500) {
        lickedR = 0; lickedL = 0;
        lickAveR.push(analogRead(A5) - analogRead(A0));
        lickAveL.push(analogRead(A0) - analogRead(A5));
        if (lickAveR.mean() > lickThresh) {
          lickedR = 1;
          digitalWrite(lickingROut, HIGH);
          delay(20);
          digitalWrite(lickingROut, LOW);
        }
        else if (lickAveL.mean() > lickThresh) {
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


      //STOP STIMULI
      DF1201S.setPlayTime(/*Play Time = */3);
      digitalWrite(odorAB, LOW); digitalWrite(odorCD, LOW);
      digitalWrite(odorAirSupply, LOW); digitalWrite(odorPair, LOW);
      digitalWrite(trialLROut, LOW);
      digitalWrite(irrelLROut, LOW);
      lickedR = 0; lickedL = 0;
      lickAveR.push(0); lickAveR.push(0); lickAveR.push(0);
      lickAveL.push(0); lickAveL.push(0); lickAveL.push(0);
      delay(50);
      timeElapsed = 0;


      //WAIT FOR RESPONSE
      while (lickedR == 0 && lickedL == 0 && timeElapsed <= 1500) {
        lickAveR.push(analogRead(A5) - analogRead(A0));
        lickAveL.push(analogRead(A0) - analogRead(A5));
        if (lickAveR.minimum() > lickThresh) {
          lickedR = 1;
          RT = timeElapsed;
          digitalWrite(lickingROut, HIGH); delay(20); digitalWrite(lickingROut, LOW);
        }
        else if (lickAveL.minimum() > lickThresh) {
          lickedL = 1;
          RT = timeElapsed;
          digitalWrite(lickingLOut, HIGH); delay(20); digitalWrite(lickingLOut, LOW);
        }
      }


      //RECORD RESULT AND DELIVER REWARD OR NOT
      if (trialLR == 1) {     //if the reward is on the right
        if (lickedR == 1) {
          trialCorrect = 1;
          runAveR.push(1);
        }
        if (trialCorrect == 1) {
          digitalWrite(H2OR, HIGH); digitalWrite(rewardOut, HIGH);
          delay(delayTime);
          digitalWrite(H2OR, LOW); digitalWrite(rewardOut, LOW);
          DF1201S.playFileNum(whiteNoiseRight);
          DF1201S.setPlayTime(/*Play Time = */0);
          totalDispensed = totalDispensed + 4;
        }
        else if (lickedL == 1) {
          runAveR.push(0);
          DF1201S.playFileNum(whiteNoiseRight);
          DF1201S.setPlayTime(/*Play Time = */0);
        }
      }
      else if (trialLR == -1) {
        if (lickedL == 1) {
          trialCorrect = 1;
          runAveL.push(1);
        }
        if (trialCorrect == 1) {
          digitalWrite(rewardOut, HIGH); digitalWrite(H2OL, HIGH);
          delay(delayTime);
          digitalWrite(H2OL, LOW); digitalWrite(rewardOut, LOW);
          DF1201S.playFileNum(whiteNoiseLeft);
          DF1201S.setPlayTime(/*Play Time = */0);
          totalDispensed = totalDispensed + 4;
        }
        else if (lickedR == 1) {
          runAveL.push(0);
          DF1201S.playFileNum(whiteNoiseLeft);
          DF1201S.setPlayTime(/*Play Time = */0);
        }
      }

      if (lickedL == 1 || lickedR == 1) {     //...if the animal responded
        runAveNG.push(1);
        response = 1;
        runAve.push(trialCorrect);
      }
      else {
        runAveNG.push(0);
        response = 0;
      }


      //KEEP TRACK OF SIDE BIAS
      if (trialLR == -1) {
        if (trialCorrect == 0) {
          lickBias.push(-1);
          lickBias.push(-1);
        }
        else {
          lickBias.push(0);
        }
      }
      else {
        if (trialCorrect == 0) {
          lickBias.push(1);
          lickBias.push(1);
        }
        else {
          lickBias.push(0);
        }
      }


      //CHECK FOR END OF BLOCK/SESSION
      if (crit == 0 && runAve.mean() >= 0.8 && runAveL.mean() >= 0.5 && runAveR.mean() >= 0.5 && runAveNG.mean() >= 0.8) {
        crit = 1;
      }


      //PRINT TRIAL DATA
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
      Serial.print(response); Serial.print(" ");
      Serial.print(trialCorrect); Serial.print(" ");
      Serial.print(runAveL.mean()); Serial.print(" ");
      Serial.print(runAveR.mean()); Serial.print(" ");
      Serial.print(runAveNG.mean()); Serial.print(" ");
      Serial.print(runAve.mean()); Serial.print(" ");
      Serial.print(totalDispensed); Serial.print(" ");


      //END TRIAL, BEGIN ITI
      digitalWrite(trialOut, LOW);
      timeElapsed = 0;
      ITI = random(8000, 10000);
      if (lightStimYN == 2){
        tone(optoChannel, 32, ITI - 2000);
      }
      delay(ITI);

    }   


    //END SESSION
    else {
      for (int i = 0; i <= 10; i++) {
        Serial.println();
      }
      Serial.print("Session finished.");
      if (session < 6) {
        Serial.print("  Animal has ");
        if (crit == 0 || trial < 100) {
          Serial.print("NOT ");
        }
        Serial.print("PASSED! Please log score in spreadsheet.");
        Serial.println(" Give ");
        Serial.print(1100 - totalDispensed);
        Serial.print(" uL H2O to supplement.");
      }
      else {
        Serial.print("Animal has completed ");
        Serial.print(blockNum - 1);
        Serial.print(" trial blocks. Please log score in spreadsheet.");
        Serial.println(" Give ");
        Serial.print(1100 - totalDispensed);
        Serial.print(" uL H2O to supplement.");
      }

      digitalWrite(H2OL, LOW); digitalWrite(H2OR, LOW);
      digitalWrite(odorAirSupply, LOW); digitalWrite(odorAB, LOW);
      while (1) {
        analogWrite(LEDChannel, brightness);
        brightness = brightness + fadeAmount;
        if (brightness == 0 || brightness == 255) {
          fadeAmount = -fadeAmount;
        }
        delay(10);
      }
    }
  }
}
