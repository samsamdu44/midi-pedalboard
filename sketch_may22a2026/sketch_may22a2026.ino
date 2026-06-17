#include <Control_Surface.h>

USBMIDI_Interface midi;


//Mux control pins
int s0 = 5;
int s1 = 4;
int s2 = 3;
int s3 = 2;

//Mux in "SIG" pin
int SIGA_pin = 8;
int SIGB_pin = 9;


int keyBoard[32] = {
  67, 66, 65, 64, 
  63, 62, 61, 60,
  59, 58, 57, 56,
  55, 54, 53, 52,
  51, 50, 49, 48,
  47, 46, 45, 44,
  43, 42, 41, 40,
  39, 38, 37, 36
};

int readMux(int channel);

int capteursA_LASTSTATE[16] = {
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023
};

int capteursB_LASTSTATE[16] = {
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023,
  1023, 1023, 1023, 1023
};

int capteursA[16];
int capteursB[16];

void setup(){

  midi.begin();
 
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  pinMode(SIGA_pin, INPUT);
  pinMode(SIGB_pin, INPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  Control_Surface.begin();
  Serial.begin(9600);

}

void loop(){

   midi.update();
   Control_Surface.loop();

    // Read current state for Mux A buttons
    for (int i = 0; i < 16; i++) {
        capteursA[i] = readMux(i, SIGA_pin);
        capteursB[i] = readMux(i, SIGB_pin);
    }
    
    // Send MIDI messages on button interaction
    // Mux A
    for (int i = 0; i < 16; i++) {
        handleMidi(keyBoard[i], capteursA[i], capteursA_LASTSTATE[i]);
    }
    
    // Mux B
    for (int i = 0; i < 16; i++) {
        handleMidi(keyBoard[i + 16], capteursB[i], capteursB_LASTSTATE[i]);
    }


    delay(20);
}


void handleMidi(int midiValue, int currentState, int lastState) {
  if (currentState != lastState) {
    if (currentState <= 400) {
      
      Serial.print(currentState);
      Serial.print(" - ");
      Serial.println(midiValue);
      
      midi.sendNoteOn({midiValue, CHANNEL_1}, 0x7F);

    } else {
       
      midi.sendNoteOff({midiValue, CHANNEL_1}, 0x7F);

    }
    lastState = currentState;
  }
}


int readMux(int channel, int mux) {
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0 
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4  
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8 
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12 
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(mux);

  //return the value
  return val;
}
