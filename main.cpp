/**
 * A motor drive controller for brushless DC motors.
 *
 * The controller reads inputs from Hall effect sensors to determine the rotor
 * state of a brushless DC motor. The controller determine which MOSFETS to
 * enable in the inverter based on the current rotor state. The MOSFETs are
 * driven by a HIP4086APZ three phase gate driver connected on port C.
 *
 * Pin assignments:
 *
 *   - PB4: Hall effect sensor A
 *   - PB5: Hall effect sensor B
 *   - PB6: Hall effect sensor C
 *   - PC0: AH inverting
 *   - PC1: AL
 *   - PC2: BH inverting
 *   - PC3: BL
 *   - PC4: CH inverting
 *   - PC5: CL
 */
#include <Arduino.h>

/**
 * Brushless DC motor commutation sequence.
 *
 * |-----------|   |-----------------------------|
 * |  Sensors  |   |        MOSFET States        |
 * |-----------|   |-----------------------------|
 * | C   B   A |   | CL   CH   BL   BH   AL   AH |
 * |---+---+---|   |----+----+----+----+----+----|
 * | 0 | 0 | 1 | 1 |  1 |  1 |  0 |  1 |  0 |  0 | AH and CL on
 * | 0 | 1 | 0 | 2 |  0 |  1 |  0 |  0 |  1 |  1 | BH and AL on
 * | 0 | 1 | 1 | 3 |  1 |  1 |  0 |  0 |  0 |  1 | BH and CL on
 * | 1 | 0 | 0 | 4 |  0 |  0 |  1 |  1 |  0 |  1 | CH and BL on
 * | 1 | 0 | 1 | 5 |  0 |  1 |  1 |  1 |  0 |  0 | AH and BL on
 * | 1 | 1 | 0 | 6 |  0 |  0 |  0 |  1 |  1 |  1 | CH and AL on
 * |-----------|   |-----------------------------|
 */
int const MAP_COMMUTATION[7] = { 0, 0b00110100, 0b00010011, 0b00110001,
    0b00001101, 0b00011100, 0b00000111 };

/**
 * Handles changes to the rotor state.
 *
 * Hall effect sensor values are read through port B and used as an index into
 * the commutation table. The commutation value is then used as output to the
 * gate driver, which is connected on port C.
 */
ISR(PCINT0_vect) {
  int index = (PINB & 0b01110000) >> 4;
  PORTC = MAP_COMMUTATION[index];
}

void setup() {
  // Set PORTC as an output. PC0-PC6 are used as inputs to the HIP4086APZ.
  DDRC |= (1 << PC0);  // CL
  DDRC |= (1 << PC1);  // CH (inverting)
  DDRC |= (1 << PC2);  // BL
  DDRC |= (1 << PC3);  // BH (inverting)
  DDRC |= (1 << PC4);  // AL
  DDRC |= (1 << PC5);  // AH (inverting)

  // Initially set all MOSFETs off.
  PORTC = 0b00010101;

  // Enable pullup resistors for the Hall effect sensors.
  PORTB |= (1 << PB4);  // Hall A
  PORTB |= (1 << PB5);  // Hall B
  PORTB |= (1 << PB6);  // Hall C

  // Enable pin change for the Hall effect sensors.
  PCMSK0 |= (1 << PCINT4);  // Hall A
  PCMSK0 |= (1 << PCINT5);  // Hall B
  PCMSK0 |= (1 << PCINT6);  // Hall C

  // Enable pin change interrupts.
  PCICR |= (1 << PCIE0);
}

void loop() {
  delay(1000);
}
