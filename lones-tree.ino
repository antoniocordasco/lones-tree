
#include <FastLED.h>
#define LED_PIN     8
#define MIC_PIN     A1
#define NUM_LEDS    9
CRGB leds[NUM_LEDS];
int leds_r[NUM_LEDS];
int leds_g[NUM_LEDS];
int leds_b[NUM_LEDS];

int red;
int green;
int blue;
int colourBoost = 150;
int maxDev = 16;
int loopsFromLastLoudSound = 0;
float stdDev = 1;
int loopNr = 0;
int loopLastLoudSound = -999;


int selectedPattern = 0;

void printColours(int ledId) {  
  Serial.print("LED: ");
  Serial.print(ledId);
  Serial.print(" RED: ");
  Serial.print(leds_r[ledId]);
  Serial.print(" GREEN: ");
  Serial.print(leds_g[ledId]);
  Serial.print(" BLUE: ");
  Serial.println(leds_b[ledId]);
}

void randomPattern() {
  stdDev = getStdDev();
  bool change = false;
  for (int i=0; i<pow(stdDev, 3); i++) {
    if (random(100) == 4) {
      change = true;
    }
  }
  //Serial.print(" DEV: ");
  //Serial.println(stdDev);
  if (!change) {
    return;
  } else {
  }

  red = 0;
  green = 0;
  blue = 0;

  int dominantColour = random(3);
  if (dominantColour == 0) {
    red = colourBoost + 40;
  } else if (dominantColour == 1) {
    green = colourBoost;
  } else  {
    blue = colourBoost;
  } 
  
  int ledId = random(NUM_LEDS);

  leds_r[ledId] = red + random(10);
  leds_g[ledId] = green + random(10);
  leds_b[ledId] = blue + random(10);

  leds[ledId] = CRGB (leds_r[ledId] , leds_g[ledId], leds_b[ledId]);
  FastLED.show();

  printColours(ledId);
}

void rotatingPattern() {  
  for (int i=0; i<9; i++) {  
    
    stdDev = getStdDev();  
    incrementLoopNr();
    changePatternIfLoudSound();
    
    if (i == 0) {     
      for (int ii=0; ii<9; ii++) {          
        leds[ii] = CRGB (0 , 0, 255);
      }
      FastLED.show();
    } else if (i == 3) {  
      for (int ii=0; ii<9; ii++) {   
        leds[ii] = CRGB (0 , 255, 0);
      }
      FastLED.show();
    } else if (i == 6) { 
      for (int ii=0; ii<9; ii++) {   
        leds[ii] = CRGB (255 , 0, 0);
      }
      FastLED.show();
    }
  }
}

int currentRotatingColourVal = 0;
bool currentRotatingColourAscending = true;
void rotatingColourPattern(int rgb) {  
  
  stdDev = getStdDev();  
  incrementLoopNr();
  changePatternIfLoudSound();

  if (currentRotatingColourAscending) {
    currentRotatingColourVal += 5;
    
    if (currentRotatingColourVal >= 240) {
      currentRotatingColourAscending = false;
    }
  } else {    
    currentRotatingColourVal -= 5;    
    if (currentRotatingColourVal <= 20) {
      currentRotatingColourAscending = true;
    }
  }
  
  if (rgb == 0) {
    for (int i=0; i<9; i++) {   
      leds[i] = CRGB (currentRotatingColourVal , 0, 0);
    }
    FastLED.show();
  } else if (rgb == 1) {
    for (int i=0; i<9; i++) {   
      leds[i] = CRGB (0 , currentRotatingColourVal, 0);
    }
    FastLED.show();
  } else if (rgb == 2) {
    for (int i=0; i<9; i++) {   
      leds[i] = CRGB (0 , 0, currentRotatingColourVal);
    }
    FastLED.show();
  }  
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}


float getStdDev() {
  int numSamples = 500;
  int samples[numSamples];
  float avg = 0;
  float dev = 0;
  float stdDevAdjust = 2.7;

  for (int i=0; i<numSamples; i++) {
    samples[i] = analogRead(MIC_PIN);
    avg += samples[i];
  }  
  avg = avg/numSamples;
  
  for (int i=0; i<numSamples; i++) {
    dev += pow(samples[i]-avg, 2);
  }

  dev = sqrt(dev/numSamples)*3 - stdDevAdjust;
  if (dev < 1) {
    dev = 1;
  } else if (dev > maxDev) {
    dev = maxDev;
  }
  return dev;
}

void incrementLoopNr() {
  if (loopNr < 999) {
    loopNr += 1;
  } else {
    loopNr = 0;
  }
}

void changePatternIfLoudSound() {
  if (stdDev > 4) {
    Serial.print(loopNr);
    Serial.print(" - ");
    Serial.print(loopLastLoudSound);
    Serial.print(" ");
    Serial.println("loud!!!");
    if (loopNr - loopLastLoudSound > 2) {
      Serial.println("change!!!");
      selectedPattern = (selectedPattern+1) % 5;
      Serial.println(selectedPattern);
    }
    loopLastLoudSound = loopNr;   
  }
}

void loop() {

  incrementLoopNr();
  changePatternIfLoudSound();
  
  if (selectedPattern == 0) {
    randomPattern();
  } else if (selectedPattern == 1) {
    rotatingColourPattern(0);
  } else if (selectedPattern == 2) {
    rotatingColourPattern(1);
  } else if (selectedPattern == 3) {
    rotatingColourPattern(2);
  } else {
    rotatingPattern();
  }
}
