void loop()
{
  // ----------------------
  // Frequency Rotary encoder algorithme ---
  newPosition_f = FreqEnc.read();               // lees de freq encoder rotatie veranderd op interrupt
  newPosition_L = LeveldBmEnc.read();           // lees de Level encoder rotatie veranderd op interrupt
  //------------------------------------------------------------------------------------------------------
  // Freq verandering van Si5351 verwerken
  if (newPosition_f != oldPosition_f) {         // er is een rotatie verandering gezien
    newPos2_f = oldPosition_f;                  // sla nieuwe positie_waarde op
    oldPosition_f = newPosition_f;              // oude positie_waarde == nieuwe positie_waarde

    if (newPosition_f/4 < newPos2_f/4){         // als een volledige stap (4 veranderingen) dan 
      VFO_UP();
    }
    if (newPosition_f/4 > newPos2_f/4){         // < en > verwisselen = draairichting
      VFO_DOWN();
    }
  }    

  // ---------------------------------------------------------------------------------------------------
  // LeveldBm verandering verwerken
  if (newPosition_L != oldPosition_L) {         // er is een rotatie verandering gezien
    newPos2_L = oldPosition_L;                  // sla nieuwe positie_waarde op
    oldPosition_L = newPosition_L;              // oude positie_waarde == nieuwe positie_waarde

    if (newPosition_L/4 > newPos2_L/4){         // als een volledige stap (4 veranderingen) dan 
      Level_UP();                               //
    }
     if (newPosition_L/4 < newPos2_L/4){        //
      Level_DOWN();                             //
    }    
  }
  //-------------------------------------------------------------------------------------------------
  // Rotary-encoder pushbuttons Level / FStep 
 
  SwitchValue();                                     // Read Sw 1,2,3,6,7,8,9
  // --- Lees Encoder druktoets LeveldBmEnc om Level te veranderen (schakelt o.a. S9, S9+40, enz)  
  if (Sw == 1) {                                     // toetsWaarde = 1 .. Level toets
    Level++;                                         // LevelIndex een stap omhoog
    if (Level>5) {                                   // eind LevelArray
        Level = 0;                                   // terug naar start LevelArray
    }
    LeveldBm = LeveldBmArray[Level];                 // haal waarde uit LevelArray
    ExecLeveldBm();                                  // routine die verzwakker aanstuurt
    delay(100);                                      // settling time
    Sw = 0;                                          // toetsWaarde wordt 0
  }

  // Fstep 1/10/100/1k/10k/100k/1M/10M/100M/1G veranderen
  if (Sw == 3) {                                     // toetsWaarde = 3 .. Frequentie toets
    Sw = 0;                                          // toetsWaarde wordt 0
    FStepFunctie();                                  // Fstep instellen
  }
  //----------------------------------------------------------------------------------------------------

    if (Sw == 6){                                     // Store this freq
     Sw = 0;
     digitalWrite(A4,HIGH);                           // Led S6 aan
     EEpromString = "Store this frequency";
     lcd.setCursor(0,1);
     lcd.print(EEpromString);
     PUTEEPROM();
     delay(2500);
     lcd.setCursor(0,1);
     lcd.print(F("                    "));
     digitalWrite(A4,LOW);                            // Led S6 weer uit
  }


  if (Sw == 7){                                       // Get Stored freq
     Sw = 0;
     digitalWrite(A5,HIGH);                           // Led S7 aan
     EEpromString = "Get stored frequency";
     lcd.setCursor(0,1);
     lcd.print(EEpromString);
     //if (Sw==7) 
     GETEEPROM();
     delay(2500);
     ExecFreq();
     ExecLeveldBm();
     lcd.setCursor(0,1);
     lcd.print(F("                    "));
     digitalWrite(A5,LOW);                            // Led S7 weer uit
  }

    // ************* hamband ****************************************
  if (Sw == 8){                                       // Ham Band menu
     Sw = 0;                                          // geen toets 
     digitalWrite(A6,HIGH);                           // Led S8 aan
     HamBandFunctie();                                //
     digitalWrite(A6,LOW);                            // Led S8 weer uit
  }
  
  if (Sw==9 ) { //&& resettijd < 1500) {  // als S9 <3 seconden ingedrukt dan calibratie menu
     //if (resettijd > 1) Serial.println (resettijd);
     Sw = 0;
     digitalWrite(A7,HIGH);                           // HF ledje weer aan
     calibratieMenu();
     digitalWrite(A7,LOW);                            // HF led weer uit
  } 
  
  lcd.setCursor(Cursorpos[FStepIndex],0);             // plaats cursor onder aktieve frequentie digit
  lcd.cursor();                                       // toon underscore cursor
}
