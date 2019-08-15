
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <string.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

int fsrPin [4] = {0,1,2,3}; // the FSR and 10K Pulldown pins that are connected to a0 - a3
int fsrReading [4];
int r = -1;
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN_A       13
#define PIN_B       11
#define PIN_C       9
#define PIN_D       7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      12

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels [4];


int delayval = 500; // delay for half a second
const long respInterval = 2000;

//digital pins that vibrating motors are connected to 
const int motorPin[4] = {12, 10, 8, 6};

int max = 950; 

//Set all curr weights to 0
int curr [4] = {0,0,0,0};

//Timers to use instead of delay
unsigned long timers [4];

bool signals [4]; //Array of 4 false - if true, light up and vibrate motor

void on(int part)
{
    //Random colors for now
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;

    for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels[part].setPixelColor(i, pixels[part].Color(r,g ,b)); // Moderately bright green color.
    pixels[part].show(); // This sends the updated pixel color to the hardware.
  
    }

    digitalWrite(motorPin[part], HIGH);

}

void off(int part)
{
   for(int i=0;i<NUMPIXELS;i++)
    {
      pixels[part].setPixelColor(i, pixels[part].Color(0,0,0));
      pixels[part].show(); // This sends the updated pixel color to the hardware.
    }
    //Sets the motor to low
    digitalWrite(motorPin[part], LOW);
}


void setup() {
    // We'll send debugging information via the Serial monitor
  Serial.begin(1200);   

  pixels[0] = Adafruit_NeoPixel(NUMPIXELS, PIN_A, NEO_GRB + NEO_KHZ800);
  pixels[1] = Adafruit_NeoPixel(NUMPIXELS, PIN_B, NEO_GRB + NEO_KHZ800);
  pixels[2] = Adafruit_NeoPixel(NUMPIXELS, PIN_C, NEO_GRB + NEO_KHZ800);
  pixels[3] = Adafruit_NeoPixel(NUMPIXELS, PIN_D, NEO_GRB + NEO_KHZ800);
  
  for (int i = 0; i < 4; i++)
  {
   fsrReading[i] = 0; //Initialize all to 0 
   signals[i] = false; //Initalize all signals to false
   pixels[i].begin(); // This initializes the NeoPixel library.
   pinMode(motorPin[i], OUTPUT);
   timers[i] = 0;
   //off(i); Switch off by default
  }

  //Initialize all timers as 0

}

void loop() {
  r = -1;
  String val; //Value that is received from the Pi
  unsigned long currTimer = millis(); //Timer for the current time after the prog ran
  //Serial.println(currTimer);
  //Recevies signals from the Pi
  
  if(Serial.available()){         //From RPi to Arduino
    r = Serial.read() - '0' ;  //conveting the value of chars to integer
    //Serial.println(r);
  }

  if (r != -1)
  {
    on(r);
    
  }
  //Get all the readings from the fsr pins
  for (int i = 0; i < 4; i++)
  {
      fsrReading[i] = analogRead(fsrPin[i]); 
        //Serial.print(i);
        //Serial.print(" | ");
        //Serial.println(fsrReading[i]);
        //Check weights for all and set signals array if necessary

        //Switch off the lights if they are switched on
         if (signals[i] == true && (currTimer - timers[i] > respInterval))
         {
          off(i); //Switch off the lights and the motors
          signals[i] = false; //Signals are off
         }
        
        if (abs(fsrReading[i] - curr[i]) > 150) //big change detected
        {

          if (fsrReading[i] < curr[i])
          {
            Serial.print("Remove ");
            Serial.println(i);
          }
          else if (curr[i] < fsrReading[i])
          {
            Serial.print("Add ");
            Serial.println(i);
          }
          curr[i] = fsrReading[i];

          //Only can turn on the light every 5 seconds
          if (signals[i] == false && (currTimer - timers[i] > respInterval)) //Only turn on if signal is false
           {
            signals[i] = true;
            timers[i] = currTimer;
            on(i); //Switch on the lights and the motors
           }
        }

        }
    
  }
