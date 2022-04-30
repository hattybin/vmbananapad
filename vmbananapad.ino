#include <ResponsiveAnalogRead.h>
#include <Keypad.h>

/*
A 10
B 11
C 12
D 13
E 14
F 15
G 16
H 17
I 18
J 19
K 20
L 21
M 22
N 23
O 24
P 25
Q 26
R 27
S 28
T 29
U 30
V 31
W 32
X 33
 */

// rows and cols for the keypad matrix
const byte ROWS = 4;
const byte COLS = 6; 

// define how many pots and buttons are active up to number of available analog inputs
const int analogInputs = 10;

// define slider pins and cc codes
const int A_PINS = analogInputs;
const int ANALOG_PINS[A_PINS] = {A3, A4, A6, A7, A8, A9, A10, A11, A12, A13};
const int CCID[A_PINS] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45};

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

// the setup routine runs once when you press reset:
void setup() {  
  // initialize serial monitor output
  Serial.begin(9600);
}

void loop(){

  // buttons
  // make keypad object   
  char key = keypad.getKey();

  // if there's a value, send it to midi
  if (key != NO_KEY){
        
int pos = 0;  
  for(int i=0;i<ROWS;i++){
    for(int j=0;j<COLS;j++){
      if(key == keys[i][j]){
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

  // update the ResponsiveAnalogRead object every loop
  for (int i=0;i<A_PINS;i++){

    analog[i].update(); 

    // if the repsonsive value has change, print out 'changed'
    if(analog[i].hasChanged()) 
    {

      data[i] = analog[i].getValue()>>3;
      if (data[i] != dataLag[i]){
        dataLag[i] = data[i];
        usbMIDI.sendControlChange(CCID[i], -(data[i]), 1);
        Serial.print("SLIDER CCID: ");
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
