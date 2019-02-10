#include <ResponsiveAnalogRead.h>
#include <Bounce.h>
ResponsiveAnalogRead crossfader(A9, true);
ResponsiveAnalogRead leftVolume(A8, true);
ResponsiveAnalogRead rightVolume(A7, true);
ResponsiveAnalogRead leftFilter(A6, true);
ResponsiveAnalogRead rightFilter(A5, true);
ResponsiveAnalogRead leftEffect1(A4, true);
int values[21];

//The number of push buttons
const int NUM_OF_BUTTONS = 6;
const int DEBOUNCE_TIME = 5;
const int MIDI_NOTE_NUMS[NUM_OF_BUTTONS] = {33, 34, 35, 36, 37, 38};
Bounce buttons[NUM_OF_BUTTONS] =
{
  Bounce (0, DEBOUNCE_TIME),
  Bounce (1, DEBOUNCE_TIME),
  Bounce (2, DEBOUNCE_TIME),
  Bounce (3, DEBOUNCE_TIME),
  Bounce (4, DEBOUNCE_TIME),
  Bounce (5, DEBOUNCE_TIME)
};

void setup() {
  pinMode(13, OUTPUT);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  pinMode(36, OUTPUT);
  pinMode(35, OUTPUT);

  leftFilter.setActivityThreshold(15.0f);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  if (channel == 1) {
    switch (note) {
      case 1:
        analogWrite(6, 50);
        break;
      case 2:
        analogWrite(7, 3);
        break;
      case 3:
        analogWrite(8, 50);
        break;
      case 4:
        analogWrite(9, 50);
        break;

      case 11:
        analogWrite(36, 50);
        break;
      case 12:
        analogWrite(35, 3);
        break;
      case 13:
        analogWrite(10, 50);
        break;
      case 14:
        analogWrite(11, 3);
        break;
    }
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  if (channel == 1) {
    switch (note) {
      case 1:
        analogWrite(6, 0);
        break;
      case 2:
        analogWrite(7, 0);
        break;
      case 3:
        analogWrite(8, 0);
        break;
      case 4:
        analogWrite(9, 0);
        break;

      case 11:
        analogWrite(36, 0);
        break;
      case 12:
        analogWrite(35, 0);
        break;
      case 13:
        analogWrite(10, 0);
        break;
      case 14:
        analogWrite(11, 0);
        break;
    }
  }
}

void OnControlChange(byte channel, byte control, byte value) {
  if (channel == 1) {
    if (control == 10) {
      //analogWrite(6, value);
    }
  }
}


void loop() {

  faders();

  potis();

  effects();

  buttonsHandler();

  while (usbMIDI.read())
  {

  }
}

void faders() {
  crossfader.update();
  if (crossfader.hasChanged()) {
    midiSend(24, map(crossfader.getValue(), 1023, 0, 0, 127));
  }

  leftVolume.update();
  if (leftVolume.hasChanged()) {
    midiSend(26, map(leftVolume.getValue(), 1023, 0, 0, 127));
  }

  rightVolume.update();
  if (rightVolume.hasChanged()) {
    midiSend(25, map(rightVolume.getValue(), 1023, 0, 0, 127));
  }
}

void potis() {
  leftFilter.update();
  if (leftFilter.hasChanged()) {
    midiSend(27, map(leftFilter.getValue(), 0, 1023, 0, 127));
  }

  rightFilter.update();
  if (rightFilter.hasChanged()) {
    midiSend(28, map(rightFilter.getValue(), 0, 1023, 0, 127));
  }
}

void effects() {
  int leftEffect1 = map(analogRead(A4), 0, 1023, 0, 1);
  if (leftEffect1 != values[0]) {
    values[0] = leftEffect1;
    digitalWrite(13, HIGH);
    if (leftEffect1 == 1) {
      usbMIDI.sendNoteOn(29, 127, 1);
      delayMicroseconds(100);
      usbMIDI.sendNoteOff(29, 0, 1);
    } else {
      usbMIDI.sendNoteOn(30, 127, 1);
      delayMicroseconds(100);
      usbMIDI.sendNoteOff(30, 0, 1);
    }
    digitalWrite(13, LOW);
  }

  int rightEffect1 = map(analogRead(A3), 0, 1023, 0, 1);
  if (rightEffect1 != values[1]) {
    values[1] = rightEffect1;
    digitalWrite(13, HIGH);
    if (rightEffect1 == 1) {
      usbMIDI.sendNoteOn(31, 127, 1);
      delayMicroseconds(1);
      usbMIDI.sendNoteOff(31, 0, 1);
    } else {
      usbMIDI.sendNoteOn(32, 127, 1);
      delayMicroseconds(1);
      usbMIDI.sendNoteOff(32, 0, 1);
    }
    digitalWrite(13, LOW);
  }
}

void buttonsHandler() {
  for (int i = 0; i < NUM_OF_BUTTONS + 1; i++)
  {
    buttons[i].update();
  }

  for (int i = 0; i < NUM_OF_BUTTONS; i++)
  {
    if (buttons[i].fallingEdge())
    {
      usbMIDI.sendNoteOn (MIDI_NOTE_NUMS[i], 127, 1);
    }
    else if (buttons[i].risingEdge())
    {
      usbMIDI.sendNoteOff (MIDI_NOTE_NUMS[i], 0, 1);
    }
  }
}

void midiSend(int device, int value) {
  digitalWrite(13, HIGH);
  Serial.println(device + " " + value);
  usbMIDI.sendControlChange(device, value, 1);
  delayMicroseconds(100);
  digitalWrite(13, LOW);
}



