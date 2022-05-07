#include <ResponsiveAnalogRead.h>
#include <Keypad.h>

/*
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

Pins: A3, A4, A6, A7, A8, A9, A10, A11, A12, A13 
      (I skipped A5 for no particular reason but cleaner wiring in my prototype.)
  pot       pin   CCID 
  Rotary 1  A6    40
  Rotary 2  A8    41
  Slider 1  A7    42
  Slider 2  A9    43
  Slider 3  A4    44
  Slider 4  A3    45
  Slider 5  A13   46
  Slider 6  A12   47
  Slider 7  A11   48
  Slider 8  A10   49
 */

// rows and cols for the keypad matrix
const byte ROWS = 4;
const byte COLS = 6; 

// define how many pots and buttons are active up to number of available analog inputs
const int analogInputs = 10;

// define slider pins and cc codes
const int A_PINS = analogInputs;
const int ANALOG_PINS[A_PINS] = {A3, A4, A6, A7, A8, A9, A10, A11, A12, A13};
const int CCID[A_PINS] = {45, 44, 40, 42, 41, 43, 49, 48, 47, 46};

// define button pins
//const byte rowPins[ROWS] = {2, 3, 4, 5, 6, 7}; 
//const byte colPins[COLS] = {8, 9, 10, 11};

// testing updates
const byte rowPins[ROWS] = {8, 9, 10, 11}; 
const byte colPins[COLS] = {2, 3, 4, 5, 6, 7};

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
char keys[ROWS][COLS] = {
  {'A','B','C','D','E','F'},
  {'G','H','I','J','K','L'},
  {'M','N','O','P','Q','R'},
  {'S','T','U','V','W','X'}
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

// map function to convert values from a given range to the desired range
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// map function to convert the char value of the keymatrix to an int
// it uses the characters position in the alphabet for the MIDI CCID value
int charMap(char in_key) {
   char alphabet[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};   
   for (int i = 0; i < 26; i++) {
      if(in_key == alphabet[i]){
        return i;
      }
   }
   return -1;
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
    // new nkro keypad code
    if (keypad.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   
        {
            if ( keypad.key[i].stateChanged )   
            {
                switch (keypad.key[i].kstate) {  
                    case PRESSED:
                    msg = " PRESSED.";
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:
                    msg = " RELEASED.";
                    if(charMap(keypad.key[i].kchar) > 0) 
                    {
                      usbMIDI.sendControlChange(charMap(keypad.key[i].kchar), MIDI_CC_VAL, MIDI_CHAN);                       
                      msg = msg + "CCID sent: " + charMap(keypad.key[i].kchar) + " for key: ";
                    } else 
                    {
                      msg = msg + "CCID not sent. Error finding value.";
                    }
                    
                    
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
          // this is just debugging 
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
