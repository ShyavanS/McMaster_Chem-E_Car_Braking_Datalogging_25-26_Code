// McMaster Chem-E Car Team
// Datalogging Code - 24/25

/*
Some datalogging code for the Braking sub-team to run on the previous year's car.
Uses the new Kalman filtering algorithm to reduce noise in data implemented on the
newer car alongside several other features, but adapted to run on the Arduino UNO
from last year. It uses the old car as a stationary platform so that they can
complete their testing of the reaction temporarily while the Mechanical sub-team
continues to work on getting this year's car chassis fabricated. The code is taken
piece-meal from the current code, but adapted to work with the UNO and an L298N
motor driver for the stirring mechanism. It also uses a micro-servo for the reactant
dumping mechanism, 8 AA batteries to provide a 12 V power source to the L298N and
for the stirring motor, a potentiometer to vary the stirring speed, and a DS18B20
temperature sensor. All data is output via serial to Excel Data Streamer.
*/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2    // Pin for the DS18B20 data line
#define TURBIDITY_SENS A0 // Pin for the turbidity sensor

OneWire one_wire(ONE_WIRE_BUS);            // create a OneWire instance to communicate with the sensor
DallasTemperature temp_sensors(&one_wire); // pass oneWire reference to Dallas Temperature sensor

// variables to store temperature
double temperature_c; // Current temperature
double init_temp;     // Initial temperature for differential calculation

// Last temperature fetched flag
bool last_fetch;

// Variables to store turbidity information
double turbidity_v;
double init_turbidity;

// KALMAN FILTER variables
double x_temp; // Filtered temperature
double p_temp; // Initial error covariance

// Process noise and measurement noise
double q_temp; // Process noise covariance
double r_temp; // Measurement noise covariance

/*
Description: Subroutine to implement Kalman filtering on temperature sensor data.
Inputs:      void
Outputs:     (double)x_temp, (double)p_temp
Parameters:  (double)x_k, (double)p_k, (double)q, (double)r, (double)input
Returns:     void
*/
void kalman_filter(double x_k, double p_k, double q, double r, double input) // Kalman filtering algorithm
{
  // Kalman filter prediction
  double x_k_minus = x_k;     // Predicted next state estimate
  double p_k_minus = p_k + q; // Predicted error covariance for the next state

  // Kalman filter update

  /* Kalman gain: Calculated based on the predicted error covariance
  and the measurement noise covariance, used to update the
  state estimate (x_k) and error covariance (p_k). */
  double k = p_k_minus / (p_k_minus + r); // Kalman gain

  // Comparison with actual sensor reading
  x_k = x_k_minus + k * (input - x_k_minus); // Updated state estimate
  p_k = (1 - k) * p_k_minus;                 // Updated error covariance

  // Output results and update global variables
  x_temp = x_k;
  p_temp = p_k;
}

void fetch_temp(void)
{
  temperature_c = temp_sensors.getTempCByIndex(0); // Get temperature in Celsius

  // Update temperature kalman filter
  kalman_filter(x_temp, p_temp, q_temp, r_temp, temperature_c);

  last_fetch = true; // Raise fetch flag to signal ready
}

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

  temp_sensors.begin();                     // Initialize the DS18B20 sensor
  temp_sensors.setResolution(11);           // Reduce resolution for faster polling
  temp_sensors.requestTemperatures();       // Request temperature from all devices on the bus
  temp_sensors.setWaitForConversion(false); // Disable blocking to allow multitasking

  init_temp = temp_sensors.getTempCByIndex(0); // Get temperature in Celsius
  temperature_c = init_temp;                   // Initialize temperature variable
  last_fetch = true;                           // Raise fetch flag to signal ready

  // Turbidity readings as V
  init_turbidity = (double)analogRead(TURBIDITY_SENS) * 3.3f / 1024.0f;

  // Initialize Kalman filter parameters
  x_temp = init_temp; // Initial state estimate
  p_temp = 0.1;       // Initial error covariance
  q_temp = 0.01;      // Process noise covariance
  r_temp = 0.5;       // Measurement noise covariance
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
  // Only poll if flag indicates ready state
  if (last_fetch)
  {
    temp_sensors.requestTemperatures(); // Request temperature from all devices on the bus

    last_fetch = false; // System is no longer ready, lower flag
  }
  else if (temp_sensors.isConversionComplete())
  {
    fetch_temp(); // Fetch temperature after conversion, otherwise continue loop
  }

  // Turbidity readings as V
  turbidity_v = (double)analogRead(TURBIDITY_SENS) * 3.3f / 1024.0f;

  // Output all necessary data for datalogging in CSV format for Excel Data Streamer
  Serial.print((double)micros() * 1e-6, 6);
  Serial.print(",");
  Serial.print(temperature_c, 6);
  Serial.print(",");
  Serial.print(x_temp, 6);
  Serial.print(",");
  Serial.println(turbidity_v, 6);

  delayMicroseconds(150000);  
}
