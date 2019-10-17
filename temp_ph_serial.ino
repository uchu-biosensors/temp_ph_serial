#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include "DFRobot_PH.h"

// Constant pins to measure appropriate inputs
#define VSD   A0
#define VPLUS A1
#define VOUT  A2
#define ISD   A3

/*
 * Temp and pH globals
 */
#define ONE_WIRE_BUS 5
#define PH_PIN A5
float voltage,phValue,temperature = 25;

float Celcius=0; 
float Fahrenheit=0;

DFRobot_PH ph;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables to store adc results in 
int vsd_adc, vplus_adc, vout_adc, isd_adc, temp_adc, ph_adc = 0;
// Variables to store millivolt results in 
int vsd_mv, vplus_mv, vout_mv, isd_mv = 0;
float temp_mv, ph_mv = 0;

// Frequency to record data 
int sampling_delay;

// Booleans to determine form of data
bool adc_values  = false;
bool mv_values   = false;
bool both_values = false;

// Ask user if they want ADC, mV, or both as outputs
void get_form_of_data();
// Get sampling delay in milliseconds
void get_sampling_delay();
// Read from all of the analog pins
void read_analog_vals();
// Send data to serial port
void send_vals_to_serial();
// Calculate millivolts from ADC output
void calc_mv_from_adc();
// Read pH value from pH probe
float get_ph_val();
float readTemperature();
// Print functions to send data in different format
void print_adc();
void print_mv();
void print_both();
// "Stops" the arduino until it is reset
void check_for_stop();
// Reset analog values to zero
void clear_analog_values();

/*
 *  Setup and Loop
 */
 
void setup() {
  analogReference(DEFAULT);
  sensors.begin();
  ph.begin();
  Serial.begin(9600); // begin transmission
  while(!Serial) {} // wait for serial
  get_sampling_delay();
  get_form_of_data();
}

void loop() {
  check_for_stop();
  read_analog_vals();
  send_vals_to_serial();
  delay(sampling_delay - 20);
}

/*
 * End Setup and Loop
 */

float get_ph_val() {
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
        phValue += 0.25;
    }
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
 }

float readTemperature() {
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  return Celcius;
}

void get_form_of_data() {
  String input = "";
  bool correct_input;
  do {
    correct_input = true;
    Serial.println("Would you like ADC, mV, or both forms data? (ADC/mV/both): ");
    while(Serial.available() == 0) {} // wait for user input
    while(Serial.available() > 0) {
      input = Serial.readString();
    }
    if(input.equals("ADC\n")) {adc_values = true;}
    else if (input.equals("mV\n")) {mv_values = true;}
    else if (input.equals("both\n")) {both_values = true;}
    else {
      Serial.println("That form of data is not recognized, please try again.");
      correct_input = false;
    }
  } while (correct_input != true);

}

void get_sampling_delay() {
  String sampling_delay_string = "";
  do {
    Serial.println("Enter delay between ISFET samples in ms (> 20 ms): ");
    while(Serial.available() == 0) {} // wait for user input
    while(Serial.available() > 0) {
       sampling_delay_string = Serial.readString();
    } 
    sampling_delay = sampling_delay_string.toInt();
    if (sampling_delay == 0) {
      Serial.println("You have entered an invalid delay. Please use milliseconds.");
    }
    else if (sampling_delay < 20) {
      Serial.println("Please enter a value equal to or greater than 20 ms.");
    }
  } while (sampling_delay == 0); 
}

void read_analog_vals() {
  // Average 10 analog readings
  for (int i = 0; i < 30; i++) {
    vsd_adc   += analogRead(VSD);
    vplus_adc += analogRead(VPLUS);
    vout_adc  += analogRead(VOUT);
    isd_adc   += analogRead(ISD);
    temp_adc  += 0;
    ph_adc    += 0;
  }
  vsd_adc   = vsd_adc / 30;
  vplus_adc = vplus_adc / 30;
  vout_adc  = vout_adc / 30;
  isd_adc   = isd_adc / 30;
  temp_adc  = 0;
  ph_adc    = 0;    
}

void send_vals_to_serial() {
  if (adc_values == true) {
    print_adc();
  }
  else if (mv_values == true) {
    print_mv();
  }
  else if (both_values == true) {
    print_both();
  }
}

void calc_mv_from_adc() {
  vsd_mv   = vsd_adc * (5.0/1023.0) * 1000;
  vplus_mv = vplus_adc * (5.0/1023.0) * 1000;
  vout_mv  = vout_adc * (5.0/1023.0) * 1000;
  isd_mv   = isd_adc * (5.0/1023.0) * 1000;
  temp_mv  = readTemperature();
  ph_mv    = get_ph_val();
}

void print_adc() {
    Serial.print(vsd_adc);
    Serial.print(",");
    Serial.print(vplus_adc);
    Serial.print(",");
    Serial.print(vout_adc);
    Serial.print(",");
    Serial.print(isd_adc);
    Serial.print(",");
    Serial.print(temp_adc);
    Serial.print(",");
    Serial.print(ph_adc);
    Serial.println();
}

void print_mv() {
    calc_mv_from_adc();
    Serial.print(vsd_mv);
    Serial.print(",");
    Serial.print(vplus_mv);
    Serial.print(",");
    Serial.print(vout_mv);
    Serial.print(",");
    Serial.print(isd_mv);
    Serial.print(",");
    Serial.print(temp_mv);
    Serial.print(",");
    Serial.print(ph_mv);
    Serial.println();  
}

void print_both() {
    Serial.print(vsd_adc);
    Serial.print(",");
    Serial.print(vplus_adc);
    Serial.print(",");
    Serial.print(vout_adc);
    Serial.print(",");
    Serial.print(isd_adc);
    Serial.print(",");
    Serial.print(temp_adc);
    Serial.print(",");
    Serial.print(ph_adc);  
    Serial.print(",");
    calc_mv_from_adc();
    Serial.print(vsd_mv);
    Serial.print(",");
    Serial.print(vplus_mv);
    Serial.print(",");
    Serial.print(vout_mv);
    Serial.print(",");
    Serial.print(isd_mv);
    Serial.print(",");
    Serial.print(temp_mv);
    Serial.print(",");
    Serial.print(ph_mv);
    Serial.println();  
}

void check_for_stop() {
    String input = "";
    while(Serial.available() > 0) {
      input = Serial.readString();
    }
    if(input.equals("stop\n")) {while (true) {}}  
}

void clear_analog_values() {
  vsd_adc   = 0;
  vplus_adc = 0;
  vout_adc  = 0;
  isd_adc   = 0;
  temp_adc  = 0;
  ph_adc   = 0;     
}
