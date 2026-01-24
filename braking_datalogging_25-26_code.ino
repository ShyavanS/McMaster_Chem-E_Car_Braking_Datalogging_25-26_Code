// McMaster Chem-E Car Team
// Braking Datalogging Code - 25/26

/*
Some datalogging code for the Braking sub-team to run on a temporary
datalogging setup. It uses an arduino UNO platform so that they can complete
their testing of the reactions temporarily while the Mechanical sub-team
continues to work on getting this year's car chassis fabricated. The reaction
is a cinnamaldehyde clock which varies in turbidity and is measured using a
photoresistor and LED light source across from each other with a beaker filled
with the reactants in the centre. All data is output via serial to Excel
Data Streamer.
*/

// Included libraries
#include "filter_lib.h"

// Pins for the turbidity sensor
#define TURBIDITY_SENS1 A1
#define TURBIDITY_SENS2 A2

lowpass_filter turbidity_filter(5); // low-pass filter to reduce oscillations from stir bar

// Variables to store turbidity information
double turbidity_raw;
double turbidity_filtered;
double turbidity_ldr;
double turbidity_ref;

/*
Description: Arduino setup subroutine.
Inputs:      void
Outputs:     void
Parameters:  void
Returns:     void
*/
void setup()
{
  Serial.begin(115200); // Start serial communication (adjust baud rate as needed)

  analogReference(EXTERNAL); // Use 3.3V reference instead of default 5V

  pinMode(TURBIDITY_SENS1, INPUT);
  pinMode(TURBIDITY_SENS2, INPUT);
}

/*
Description: Arduino loop subroutine.
Inputs:      void
Outputs:     void
Parameters:  void
Returns:     void
*/
void loop()
{
  turbidity_raw = 0.0f; // Reset turbidity reading

  // Loop for averaging to reduce random noise, differential signal from wheatstone bridge
  for (int i = 0; i < 10; i++)
  {
    turbidity_ldr = (double)analogRead(TURBIDITY_SENS1);
    turbidity_ref = (double)analogRead(TURBIDITY_SENS2);

    turbidity_raw += turbidity_ldr - turbidity_ref;
  }

  // Rescale to match range of RP2040's 12-bit ADC on final design
  // Gain of 1.5 and DC bias of 1.65V to be implemented in differential instrumentation amp in final version
  turbidity_raw = (turbidity_raw / 10.0f * 4095.0f / 1023.0f + 2047.0f) * 1.5f;
  turbidity_filtered = turbidity_filter.filter(turbidity_raw);

  Serial.print((double)micros() * 1e-6, 6);
  Serial.print(",");
  Serial.print(turbidity_raw);
  Serial.print(",");
  Serial.println(turbidity_filtered);

  delay(10); // Delay for data streamer
}
