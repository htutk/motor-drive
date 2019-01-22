#include <Arduino.h>
#include <EnableInterrupt.h>

int const PIN_HALL_A = 12;
int const PIN_HALL_B = 11;
int const PIN_HALL_C = 10;

/**
 * Forward commutation sequence.
 *
 * The commutation sequence is encoded in an integer array. The array index is
 * a bitmask of the outputs of Hall effect sensors A, B, and C, respectively.
 * The value is a bitmask indicating which MOSFETs should be turned on. The
 * first array value (index 0) is filled with a dummy value.
 *
 *   |-----------|-----------------------------|---------------|
 *   | A | B | C | Q1 | Q2 | Q3 | Q4 | Q5 | Q6 | Index | Value |
 *   |---+---+---|----+----+----+----+----+----|-------+-------|
 *   | 1   0   0 |  1    0    0    0    0    1 |     4      33 |
 *   | 1   1   0 |  0    0    1    0    0    1 |     6       9 |
 *   | 0   1   0 |  0    1    1    0    0    0 |     2      24 |
 *   | 0   1   1 |  0    1    0    0    1    0 |     3      18 |
 *   | 0   0   1 |  0    0    0    1    1    0 |     1       6 |
 *   | 1   0   1 |  1    0    0    1    0    0 |     5      36 |
 *   |-----------|-----------------------------|---------------|
 */
int const MAP_COMMUTATION_FORWARD[7] = { 0b000000, 0b000110, 0b110000, 0b010010,
    0b100001, 0b100100, 0b001001 };

/**
 * Handles changes to the rotor state.
 *
 * The Hall effect sensor pins are bits 7, 6, and 5 of port B, corresponding to
 * sensors A, B, and C.
 */
void onRotorStateChange() {
  int index = (PINB & 0b01110000) >> 4;
  Serial.println(MAP_COMMUTATION_FORWARD[index]);
}

void setup() {
  Serial.begin(115200);

  // Setup Hall effect sensors.
  pinMode(PIN_HALL_A, INPUT_PULLUP);
  pinMode(PIN_HALL_B, INPUT_PULLUP);
  pinMode(PIN_HALL_C, INPUT_PULLUP);

  enableInterrupt(PIN_HALL_A, onRotorStateChange, CHANGE);
  enableInterrupt(PIN_HALL_B, onRotorStateChange, CHANGE);
  enableInterrupt(PIN_HALL_C, onRotorStateChange, CHANGE);
}

void loop() {
  delay(1000);
}
