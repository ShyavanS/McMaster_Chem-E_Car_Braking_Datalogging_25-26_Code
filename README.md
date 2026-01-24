# McMaster_Chem-E_Car_Braking_Datalogging_25-26_Code
A temporary datalogging codebase for the McMaster Chem-E Car Team Braking Sub-Team.

This project uses an Arduino UNO microcontroller programmed in the Arduino programming language. A simple setup for them to gather data for their reactions while the current car is still being created. This facilitates the creation of a linear regression model on both reactions' progression whose coefficient of determination is used to identify the reaction that the team proceeds with for the competition. All data is output via serial to Excel Data Streamer.

The major components used are as follows:<br />
1 - GL5516 LDR (for monitoring stopping reaction progress)<br />
1 - 5mm White LED (light source for LDR)<br />
1 - 150Ω resistor (current limiting for LED)<br />
3 - 5kΩ resistors (for constructing wheatstone bridge)<br />
1 - Arduino UNO Microcontroller<br />

The main objective of the competition is to design a small car to carry a certain amount of weight a set distance while powering and stopping the vehicle with a chemical reaction. The distance and weight vary each year. The compectition is hosted by the American Institute of Chemical Engineers (AIChE). See the corresponding repository for the current year's car codebase for more information on this year's setup.

Makes use of [timonbraun02's digital low-pass filter library](https://github.com/timonbraun02/digital_filter_arduino/tree/main) to remove noise due to stir bar oscillations at a frequency of 20-25 Hz.
