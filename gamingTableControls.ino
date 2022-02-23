/*  
* This script was created by Bob Clagett for I Like To Make Stuff
* For more projects, check out iliketomakestuff.com
* 
* This code is for the project at https://iliketomakestuff.com/
* 
* 
* All of the components are list on the url above.
* 
* We offer NO SUPPORT for this code, and do not promise any updates/improvements.
* Please regard this as "unoptimized, prototype code".
* Feel free to branch and extend this code, but we will likely NOT merge in any changes.
*/
#include <ezButton.h>
#include "Adafruit_Keypad.h"
#include <FastLED.h>

#define LED_PIN     13
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    132

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 30

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

const int SEATS = 8;
int highlightCenter = 0;
int highlightSpread = 3;

int centerPixels[SEATS] = {11,22,33,53,77,88,99,118};//{10,30,50,70,90,110,130,150};
CRGB highlightCLR = CRGB::Blue;

CRGBPalette16 gPal;
//-------------------------------------
//keypad setup
//-------------------------------------
  const byte ROWS = 4; // rows
  const byte COLS = 4; // columns
  //define the symbols on the buttons of the keypads
  char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {11, 10, 9, 8}; //connect to the column pinouts of the keypad
  
  //initialize an instance of class NewKeypad
  Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int pendingPlayerCode;
int runningAnimation = 0; //(1-4)
//-------------------------------------
//button setup
//-------------------------------------
  
  const int BUTTON_NUM = 5;
  
  const int BUTTON_1_PIN = 2;
  const int BUTTON_2_PIN = 3;
  const int BUTTON_3_PIN = 4;
  const int BUTTON_4_PIN = 5;
  const int BUTTON_5_PIN = 6;
  const int BUTTON_6_PIN = 7;
  const int BUTTON_7_PIN = 8;
  const int BUTTON_8_PIN = 9;
  
  ezButton buttonArray[] = {
    ezButton(BUTTON_1_PIN), 
    ezButton(BUTTON_2_PIN), 
    ezButton(BUTTON_3_PIN), 
    ezButton(BUTTON_4_PIN), 
    ezButton(BUTTON_5_PIN)
  };

int gameMode = 0;
/*
0 = DM control only
1 = player advance control

*/


void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();
  FastLED.show();
  
  Serial.begin(9600);
  customKeypad.begin();
for(byte i = 0; i < BUTTON_NUM; i++){
    buttonArray[i].setDebounceTime(50); // set debounce time to 50 milliseconds
  }
  Serial.println("annnnnnnd GO!");
}

void loop() {
  random16_add_entropy( random());
  //run current animations
  switch (runningAnimation){
    case 0:
      FastLED.clear();
      //highlightCLR = CRGB::Blue;
        break;
    case 1:
      lightning();   
        break;
    case 2:
      gPal = HeatColors_p;
      Fireplace();
      highlightCLR = CRGB::Blue;
      break;
    case 3:
      gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::White,  CRGB::Green);
      Fireplace();
      highlightCLR = CRGB::Red;
      break;
    case 4:
    gPal = CRGBPalette16( CRGB::Black, CRGB::DarkGreen, CRGB::Black,  CRGB::Yellow);
      Fireplace();
      highlightCLR = CRGB::White;
        break;
  }

//handle selected player
      highlightPlayer();

      FastLED.show(); 
      FastLED.delay(1000 / FRAMES_PER_SECOND);

  
  //check keypad
    customKeypad.tick();
  
    while(customKeypad.available()){
      keypadEvent e = customKeypad.read();
      char keypressed = (char)e.bit.KEY;
      if(e.bit.EVENT == KEY_JUST_PRESSED) {
          switch (keypressed){
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            //case '9':
              pendingPlayerCode = keypressed-'0';
                //clear old selected player
                FastLED.clear();
                FastLED.show(); 
              Serial.print(keypressed);
                break;
            case '0':
//clear all lights
               runningAnimation = pendingPlayerCode = 0;
               FastLED.clear();
               FastLED.show(); 
                break;   
//animations       
            case 'A':
              if(runningAnimation!=1){
                runningAnimation = 1;
              } else {
                runningAnimation = 0;
                FastLED.clear();
                FastLED.show();
              }
                break;
            case 'B':
              if(runningAnimation!=2){
                runningAnimation = 2;
              } else {
                runningAnimation=0;
                FastLED.clear();
                FastLED.show();
              }
                break;
            case 'C':
              if(runningAnimation!=3){
                runningAnimation = 3;
              } else {
                runningAnimation = 0;
                FastLED.clear();
                FastLED.show();
              }
                break;
            case 'D':
              if(runningAnimation!=4){
                runningAnimation = 4;
              } else {
                runningAnimation = 0;
                FastLED.clear();
                FastLED.show();            
              }
                break;
             
            case '*':
              //install SHIFT
                break;
            case '#':
              //execute player select
                break;
          }
      }
   

     
    }
    
  
    delay(10);

  //check buttons
    for(byte i = 0; i < BUTTON_NUM; i++)
        buttonArray[i].loop(); // MUST call the loop() function first
    
      for(byte i = 0; i < BUTTON_NUM; i++) {
        if(buttonArray[i].isPressed()) {
          Serial.print("The button ");
          Serial.print(i + 1);
          Serial.println(" is pressed");

          switch(gameMode){
            
            case 0:
            //if the selected player presses their button, clear it
            if(i==pendingPlayerCode){
              pendingPlayerCode=0;
            }
              break;
            case 1:
            //advance to next, with loop to beginning
              pendingPlayerCode++;
              if(pendingPlayerCode>SEATS){
                pendingPlayerCode=0;  
              }
              FastLED.clear();
              FastLED.show(); 
              break;
          }
        }
    
        /*if(buttonArray[i].isReleased()) {
          Serial.print("The button ");
          Serial.print(i + 1);
          Serial.println(" is released");
        }*/
      }
}


#define FLASHES 8
#define FREQUENCY 20 // delay between strikes

unsigned int dimmer = 1;

void lightning()
{
    for (int flashCounter = 0; flashCounter < random8(3,FLASHES); flashCounter++)
  {
    if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);           // return strokes are brighter than the leader
    
    FastLED.showColor(CHSV(255, 0, 255/dimmer));
    delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
    FastLED.showColor(CHSV(255, 0, 0));
    
    if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
    delay(50+random8(100));               // shorter delay between strikes  
  }
  delay(random8(FREQUENCY)*100);          // delay between strikes
  highlightPlayer();
}



//from https://codebender.cc/sketch:190691
/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) 55 */
#define COOLING 55

/* How hot is "hot"? Increase for brighter fire 180 */
#define HOT 120
#define MAXHOT HOT*HEIGHT
#define HEIGHT 1


void Fireplace () {
  static unsigned int spark[NUM_LEDS]; // base heat
  CRGB stack[NUM_LEDS][HEIGHT];        // stacks that are cooler

  // 1. Generate sparks to re-heat
  for( int i = 0; i < NUM_LEDS; i++) {
    if (spark[i] < HOT ) {
      int base = HOT * 2;
      spark[i] = random16( base, MAXHOT );
    }
  }

  // 2. Cool all the sparks
  for( int i = 0; i < NUM_LEDS; i++) {
    spark[i] = qsub8( spark[i], random8(0, COOLING) );
  }

  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for( int i = 0; i < NUM_LEDS; i++) {
    unsigned int heat = constrain(spark[i], HOT / 2, MAXHOT);
    for( int j = HEIGHT - 1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette( gPal, index );

      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0, HOT);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop;

      heat = constrain(heat, 0, MAXHOT);
    }
  }

  // 4. map stacks to led array
  for( int i = 0; i < NUM_LEDS; i++) {
    for( int j = 0; j < HEIGHT; j++) {
      leds[(j * NUM_LEDS) + i] = stack[i][j];
    }
  }

}
void highlightPlayer(){
  if(pendingPlayerCode != 0){
        highlightCenter = centerPixels[pendingPlayerCode-1];
        int k = highlightCenter-highlightSpread;
        int m = k+highlightSpread*2+1;
        /*Serial.print("highlightCenter=");
        Serial.println(highlightCenter);
        //Serial.println(k+"----------"+m);*/
         for( int j = k; j < m; j++) {
          Serial.println(j);
          leds[j] = highlightCLR;
        }
      }  
  
}
