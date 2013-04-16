/*
DESCRIPTION:
This file is used to measure how quickly and with how much error the analogRead() can be measured at the top (ON cycle) of a digitalWrite() call.
*/

unsigned long pulsecycles = 1; // Number of times the "pulselengthon" and "pulselengthoff" cycle (on/off is 1 cycle) (maximum = 1000 until we can store the read data in an SD card or address the interrupts issue)
unsigned long pulselengthon = 50; // Pulse LED on length in uS (minimum = 5us based on a single ~4us analogRead - +5us for each additional analogRead measurement in the pulse).
unsigned long pulselengthoff = 9975; // Pulse LED off length in uS (minimum = 20us + any additional operations which you may want to call during that time).
int measuringlight1 = 5;
int detector1 = A0;
unsigned long start1,start1orig,end1;
unsigned long pulselengthoncheck, pulselengthoffcheck, pulsecyclescheck, totaltimecheck;
int data1, data2, data3, data4;
float data1f, data2f, data3f, data4f;
int i = 0; // This detects the number of analogReads() per pulse

void setup() {
  
Serial.begin(38400); // set baud rate for serial communication
pinMode(measuringlight1, OUTPUT); // set pin to output
analogReadAveraging(1); // set analog averaging to 0 (ie ADC takes only one signal, takes ~3us)]
start1 = 0;
start1orig = 0;
end1 = 0;
data1 = 0;
data2 = 0;
data3 = 0;
data4 = 0;
}
  
void loop() {

// METHOD 1: 
// Using a for loop to define ON/OFF cycles, and using an empty while loop to do the timing

// NOTES:
// interrupts (in the form of cli() and sei() OR the standard arduino interrupts / noInterrupts) seems to make no difference in the result... though the Interrupts call itself was better than sei()
// based on looking at the actual oscilliscope but not much
// Adding to a String variable using the =+ or the string.concat() version of the string function takes about 8us!
// BUT, setting a simple int data variable equal to our analog read, that takes no time at all.
// when concatenating a string, this is slowest -   data = data + data1; .  It's better to call the concat function once and concatenate multiple things at a time within it, rather than making multiple calls.
// Even for the very quick pulses when using WASP method or other really short methods, we should be able to use the ADC to get 1us timed flashes and then just run analogRead as fast as possible (~3us).  
// Then, we can drop any of the values which are clearly too low (this should be obvious on a graph)... or perhaps we can speed up our analogRead?

// IMPROVEMENTS:
// Find a better way to store data than the String function - it's really slow and causes our OFF time to be really slow also.
// Even just getting rid of second string concatenation to add the comma (data += ",") would allow us to shorten the OFF time minimum down to 10us.
// We need a better place to store the data (SD card, for example) data storage limitation is causing the maximum number of cycles to be ~1000.  SD card should take care of that problem.

// SPECS USING THIS METHOD: 
// Peak to Peak variability, ON and OFF length variability all is <1us.  Total measurement variability = 0 to +3us (regardless of how many cycles)

digitalWriteFast(measuringlight1, HIGH); // OPTIONAL: Used with the RC circuit to measuring analogRead() timing (also, switch LOW and HIGH on the digitalWrite's in the program)
delay(3000); // give operator time to turn on the serial monitor and the capacitor to charge
noInterrupts(); // turn off interrupts to reduce interference from other calls

// Flash the LED in a cycle with defined ON and OFF times, and take analogRead measurements as fast as possible on the ON cycle
start1orig = micros();
start1 = micros();
for (i=0;i<pulsecycles;i++) {
  digitalWriteFast(measuringlight1, LOW); // During the ON phase, keep things as fast as possible - so simple data1 and data2 - no strings or anything confusing!
  data1 = analogRead(detector1); // user needs to set the number of analogReads() per ON cycle - add more for more analogReads()
  data2 = analogRead(detector1);
  data3 = analogRead(detector1);
  data4 = analogRead(detector1);
  start1=start1+pulselengthon;
  while (micros()<start1) {}
//  digitalWriteFast(measuringlight1, LOW); // Now, during OFF phase, we can average that data and save it as a string!
  start1=start1+pulselengthoff;
  while (micros()<start1) {} 
}
end1 = micros();

interrupts();

// Measure the total time of the run, the time of one ON cycle, and the time of one OFF cycle 
totaltimecheck = end1 - start1orig;

data1f = data1*3.3/1023; // convert the analog read into a voltage reading to correspond to the oscilliscope
data2f = data2*3.3/1023; // convert the analog read into a voltage reading to correspond to the oscilliscope
data3f = data3*3.3/1023; // convert the analog read into a voltage reading to correspond to the oscilliscope
data4f = data4*3.3/1023; // convert the analog read into a voltage reading to correspond to the oscilliscope

// Print values:
delay(50);
Serial.println("check the length of one pulse (one on/off), and the total number of pulses in the run: expected versus actual:");
Serial.println("expected values");

Serial.print("# of pulse cycles:  ");
Serial.println(pulsecycles);

Serial.print("total run length:  ");
Serial.println(totaltimecheck);

Serial.print("value of i:  ");
Serial.println(i);

// You can try to add start and end variables to measure the pulse on/off time, but it's better and more accurate to use an oscilliscope
//Serial.print("pulse length on actual:  ");
//Serial.println(pulselengthoncheck);
//Serial.print("pulse length off actual:  ");
//Serial.println(pulselengthoffcheck);

Serial.println("averaged data points from each ON measurement cycle");
Serial.println(data1f);
Serial.println(data2f);
Serial.println(data3f);
Serial.println(data4f);

delay(50);

while (1) {};

}  

