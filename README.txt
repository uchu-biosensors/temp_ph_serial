This repository stores an Arduino program to sample data for ISFET testing. Data is transmitted to a Raspberry Pi or ser    ial monitor using serial communication. The program accepts user input to set sampling rate and form of data to transmit as ADC output, millivolt results, or both. Data is transmitted in .csv format.

*** Digital pin 5 is used for reading temperature values in this revision ***
*** Analog pin 5 is used for reading pH values in this revision ***

***** Temperature and pH values will print '0' when ADC option is selected *****
***** Temperature will print as degrees celsius in the 4th column in mV option *****
***** pH will be printed in pH units in the 5th column in mV option *****
  
Data is reported in the following format for ADC or millivolt results:
  
          vsd,vplus,vout,isd,temp,gnd
          
Data is reported in the following format when the "both" option is selected:
  
          vsd_adc,vplus_adc,vout_adc,isd_adc,temp_adc,gnd_adc,vsd_mv,vplus_mv,vout_mv,_isd_mv,temp_mv,gnd_mv
          
When connected to a Raspberry Pi, the Pi python script appends a time stamp in milliseconds:
  
          timestamp,vsd,vplus,vout,isd,temp,gnd 

