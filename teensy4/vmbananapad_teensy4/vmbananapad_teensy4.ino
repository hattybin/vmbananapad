#include <ResponsiveAnalogRead.h>
#include <Keypad.h>

/*
Version of my vmbananapad using a teensy 4 instead of the teensy 4.1

I mapped the 24 keys to letters for the keypad char values. These are the letter to MIDI CCID mappings I'm using. 
To get the CCID value for the key, I just add 10 to the key's position in the matrix. So when the key is actuated, 
the program will send 10 + the pos variable as the MIDI CCID. This is a shitty hack that might bite me in the future 
but it works for now. 

Pins: 
  Cols: 2, 3, 4, 5, 6, 7
  Rows: 8, 9, 10, 11

char  index ccid
A     0     10
B     1     11
C     2     12
D     3     13
E     4     14
F     5     15
G     6     16
H     7     17
I     8     18
J     9     19
K     10    20
L     11    21
M     12    22
N     13    23
O     14    24
P     15    25
Q     16    26
R     17    27
S     18    28
T     19    29
U     20    30
V     21    31
W     22    32
X     23    33

The potentiometer start at CCID 40. I wanted the sliders to have  different range 
so I can recognise them easier while debugging. This may not be good practice for 
MIDI but I don't know any better so lets see if it works...

Pins: A0, A1, A2, A3, A4, A5, A6, A7, A8, A9 
      (teensy 4 has 10 analog inputs so it is perfect for this project)
  pot       pin   CCID 
  Rotary 1  A0     40
  Rotary 2  A1     41
  Slider 1  A2     42
  Slider 2  A3     43
  Slider 3  A4     44
  Slider 4  A5     45
  Slider 5  A6     46
  Slider 6  A7     47
  Slider 7  A8     48
  Slider 8  A9     49
 */

// rows and cols for the keypad matrix
const byte ROWS = 4;
const byte COLS = 6; 

// define how many pots and buttons are active up to number of available analog inputs
const int analogInputs = 10;

// define slider pin assignments and cc codes
const int A_PINS = analogInputs;
const int ANALOG_PINS[A_PINS] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
const int CCID[A_PINS] = {40, 41, 42, 43, 44, 45, 46, 47, 48, 49};

// keypad pin assignments
// I'm changing the matrix for the teensy 4. I messed up the teensy 4.1 version while 
// debugging the keypad matrix and never cleaned it up. I'm restructuring the matrix 
// to be accurate to the wiring and layout now that I understand it better
// const byte colPins[COLS] = {24, 25, 26, 27};
// const byte rowPins[ROWS] = {2, 3, 4, 5, 6, 7}; 

const byte colPins[COLS] = {2, 3, 4, 5, 6, 7};
const byte rowPins[ROWS] = {8, 9, 10, 11};

// number of buttons for the keypad matrix
const int NUM_OF_BUTTONS = 24;

// the MIDI channel number to send messages
const int MIDI_CHAN = 1;

// Create Bounce objects for each button and switch. The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
// 5 = 5 ms debounce time which is appropriate for good quality mechanical push buttons.
// If a button is too "sensitive" to rapid touch, you can increase this tim
// button debounce time
const int DEBOUNCE_TIME = 5;

// set midi mode
const int MIDI_MODE_CCS = 1;

// Arrays the store the CC message push button will send.
const int MIDI_CC_VAL = 127;

// keypad matrix ccid values for keys
// the keypad library keypad object takes char datatypes for the key matrix value
// I am a simple man and the simplest solution I could figure out to make keys send
// MIDI CCID values is to add 10 to the index of the key in the matrix array. This 
// method produces 24 unqiue values for the MIDI CCID codes we need to send with each 
// key press of: 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
// 27, 28, 29, 30, 31, 32, 33. 
// I did a small amount of research into what MIDI CCID codes are and my only take away
// from the information was the range of valid values. I suspect the values have designations
// for specific applications or some kind of meaning, but I'll have to deal with that
// in a later version once I am happy with the basic functionality.

// todo:
// I would like to implement nkro for the keypad matrix so multiple mute buttons or routing
// buttons can be pressed at the same time or within the same loop.
// I have no idea how.
char keys[COLS][ROWS] = {
  {'A','B','C','D'},
  {'E','F','G','H'},
  {'I','J','K','L'},
  {'M','N','O','P'},
  {'Q','R','S','T'},
  {'U','V','W','X'}
};

// initialize keypad object
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// define arrays for input values and lagged input values
int inputAnalog[analogInputs];

// a data array and a lagged copy to tell when MIDI changes are required
byte data[A_PINS];
byte dataLag[A_PINS];

// initialize the ReponsiveAnalogRead objects
ResponsiveAnalogRead analog[]{
  {ANALOG_PINS[0],true},
  {ANALOG_PINS[1],true},
  {ANALOG_PINS[2],true},
  {ANALOG_PINS[3],true},
  {ANALOG_PINS[4],true},
  {ANALOG_PINS[5],true},
  {ANALOG_PINS[6],true},
  {ANALOG_PINS[7],true},
  {ANALOG_PINS[8],true},
  {ANALOG_PINS[9],true}
}; 

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned long loopCount;
unsigned long startTime;

String msg;

// the setup routine runs once when you press reset:
void setup() 
{  
  // initialize serial monitor output
    Serial.begin(9600);
    loopCount = 0;
    startTime = millis();
    msg = "";
}

void loop()
{
    loopCount++;
    if ( (millis()-startTime)>5000 ) {
        Serial.print("Average loops per second = ");
        Serial.println(loopCount/5);
        startTime = millis();
        loopCount = 0;
    }

  /* non nkro keypad code
  // buttons
  // make keypad object   
  char key = keypad.getKey();

  // if keypad has a key value, send it to midi
  if (key != NO_KEY)
  {
    int pos = 0;  
    for(int i=0;i<ROWS;i++)
    {
      for(int j=0;j<COLS;j++)
      {
        if(key == keys[i][j])
        {
          int CCID = pos+10;
          usbMIDI.sendControlChange (CCID, MIDI_CC_VAL, MIDI_CHAN); 
          Serial.print("Sent: ");
          Serial.print(CCID);
          Serial.print(" for key: ");
          Serial.print(key);
          Serial.print(" with data: ");
          Serial.print(MIDI_CC_VAL);
          Serial.println("");
        }
        pos++;
      }
    }
  } 
*/
// new nkro keypad code
    if (keypad.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( keypad.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (keypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    int CCID = i+10;
                    usbMIDI.sendControlChange (CCID, MIDI_CC_VAL, MIDI_CHAN); 
                    msg = " RELEASED.";
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print("Key ");
                Serial.print(keypad.key[i].kchar);
                Serial.println(msg);
            }
        }
    }


  // update the ResponsiveAnalogRead object every loop
  for (int i=0;i<A_PINS;i++)
  {    
    analog[i].update(); 
    // if the repsonsive value has change, print out 'changed'
    if(analog[i].hasChanged()) 
    {
      data[i] = map(analog[i].getValue()>>3,128,0,0,128);
      if (data[i] != dataLag[i])
        {
          dataLag[i] = data[i];          
          usbMIDI.sendControlChange(CCID[i], data[i], 1);
          if(i == 2 || i == 4)
            { 
              Serial.print("KNOB CCID: ");
            } 
            else 
            {
              Serial.print("SLIDER CCID: ");
            }                      
          Serial.print(CCID[i]);
          Serial.print(" , Data: ");
          Serial.print(data[i]);      
          Serial.println("");
        }
    }
  }
  
  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read())
  {
    // ignoring incoming messages, so don't do anything here.
  }
  
}  
