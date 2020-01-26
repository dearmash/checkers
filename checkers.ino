#include "Adafruit_NeoTrellis.h"

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys

Adafruit_NeoTrellis t_array[Y_DIM / 4][X_DIM / 4] = {
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F) },
  { Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x31) },
};

Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM / 4, X_DIM / 4);

enum CellType {
  CT_RED, CT_BLUE, CT_EMPTY, CT_NA
};
struct Cell {
  CellType type;
};
struct Board {
  Cell cell[Y_DIM][X_DIM];
};

Board board = {
  {
    {{CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}},
    {{CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}},
    {{CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}, {CT_NA}, {CT_RED}},
    {{CT_EMPTY}, {CT_NA}, {CT_EMPTY}, {CT_NA}, {CT_EMPTY}, {CT_NA}, {CT_EMPTY}, {CT_NA}},
    {{CT_NA}, {CT_EMPTY}, {CT_NA}, {CT_EMPTY}, {CT_NA}, {CT_EMPTY}, {CT_NA}, {CT_EMPTY}},
    {{CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}},
    {{CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}},
    {{CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}, {CT_BLUE}, {CT_NA}},
  }
};

uint32_t getCellColor(Cell c) {
  switch (c.type) {
    case CT_RED: return 0x990000;
    case CT_BLUE: return 0x000099;
    case CT_EMPTY: return 0x111111;
  }
  return 0;
}

void setup() {
  Serial.begin(9600);

  if (!trellis.begin()) {
    Serial.println("failed to begin trellis");
    sos();
  }

  for (int y = 0; y < Y_DIM; y++) {
    for (int x = 0; x < X_DIM; x++) {
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, blink);
      trellis.setPixelColor(x, y, getCellColor(board.cell[y][x])); //addressed with x,y
    }
  }
  trellis.show(); //show all LEDs

}

void loop() {
  trellis.read();
  delay(20);
}

void sos() {
  uint8_t unit = 100;
  uint8_t led = 13;
  pinMode(led, OUTPUT);
  while (1) {
    for (int i = 0; i < 3; i++) {
      if (i > 0) delay(unit);
      digitalWrite(led, HIGH);
      delay(unit);
      digitalWrite(led, LOW);
    }
    delay(unit * 3);
    for (int i = 0; i < 3; i++) {
      if (i > 0) delay(unit);
      digitalWrite(led, HIGH);
      delay(unit * 3);
      digitalWrite(led, LOW);
    }
    delay(unit * 3);
    for (int i = 0; i < 3; i++) {
      if (i > 0) delay(unit);
      digitalWrite(led, HIGH);
      delay(unit);
      digitalWrite(led, LOW);
    }
    delay(unit * 7);
  }
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

TrellisCallback blink(keyEvent evt) {
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    trellis.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, X_DIM * Y_DIM, 0, 255))); //on rising
  else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
    trellis.setPixelColor(evt.bit.NUM, 0); //off falling

  trellis.show();
  return 0;
}
