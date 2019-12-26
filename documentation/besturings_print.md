[//]: # (Table with 3-columns/7-lines)

## BESTURINGS PRINT :

De Besturingsprint is een 4-laags PCB. 
De signaal verbindingen liggen opgesloten tussen een massa vlak en een Vcc vlak, zodat stoorstraling geminimaliseerd wordt.

[![](besturing_print/DisplayArduinoMega1TN.jpg)](besturing_print/DisplayArduinoMega1.pdf) | **+** |  [![](besturing_print/DisplayArduinoMega2TN.jpg)](besturing_print/DisplayArduinoMega2.pdf)
--------------------------------------------- | --- |  -----------------------------------------------
**ArduinoMega2560 + Display + I2C-connector** |     | **Drukknoppen + Rotary schakelaars + connectors**
  |     |  
  |     | 
  |     | 
[![](besturing_print/DisplayArduinoBrdTN.jpg)](besturing_print/DisplayArduinoBoard.pdf) |**-** | [![](besturing_print/DisplayArduinoTN.jpg)]("besturing_print/DisplayArduino.jpg)
  **PCB layout** *( inner layers )* |     |   **module's v/d besturingsprint**
  
**Headers :** voor een full-size Arduino Mega 2560

**Header  :** voor een Display LCD2004

**Boxed Headers :**

 -  SV1 :  6-polig connector (sda-scl) + voeding voor dual I2C isolator board
 -  SV2 : 10-polig connector + voeding van SPI-bus
 -  SV3 : 10-polig data stuursignaal verzwakker print
 -  SV4 : 10 polig latch stuursignaal verzwakker print


**Schakelaars :**

 -  tact-switches:  S6, S7, S8 en S9  met Led's
 -  rotary-swiches: SW1 en SW2  met drukschakelaar S1 en S2

