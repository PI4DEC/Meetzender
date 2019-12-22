int SiMaxFreq = MaxOscFreq/1000000;                   // MaxcOscFreq is ULL kan niet geprint worden  
int8_t Correctie; //range -128=-12.7ppm / +127=12.7ppm -12800/+12700 ppb in si5351.init(load,referentie,correctie)
String CalArray[5] = 
  {"calibratie       ppm", "Si5351 fMax<     MHz", "Harmonic factor     ", " Drive Output    mA ", "Zet waarden in EPROM"};
 //"                    ", "                    ", "                    ", "                    ", "                    "
//EEPROM.get(15,DriveMA); Serial.println(DriveMA); 

void calibratieMenu()
{
  lcd.clear();                                          // clear
  lcd.noCursor();                                       // geen storende running cursor
  lcd.print(F("  Configuratie Menu "));

  int ConfIndex = 0;                                    //  

  while (Sw != 9) {                                     // while(Sw != 9) totdat 
    newPosition_M = FreqEnc.read();                     // lees de freq encoder rotatie
    
    if (newPosition_M != oldPosition_M) {               // Positie bepalen
      newPos2_M = oldPosition_M;
      oldPosition_M = newPosition_M;
 
      if (newPosition_M/4 > newPos2_M/4){               // draairichting < en >
        ConfIndex --;                                   // ConfIndex 1 omlaag
        if (ConfIndex < 0)  ConfIndex = 4;              // bewaking grens ConfIndex en loopt rond
      }

      if (newPosition_M/4 < newPos2_M/4){               // draairichting < en >
        ConfIndex ++;                                   // ConfIndex 1 omhoog
        if (ConfIndex > 4)  ConfIndex = 0;              // bewaking grens ConfIndex en loopt rond
      }
    }                                                   // end positie bepalen
    lcd.setCursor(0,2);                                 // cursor op regel 3
    lcd.print (CalArray[ConfIndex]);                    // print een regel uit CalArray

  // ***********************************************************************************************************
    if (ConfIndex == 0)                                 // frekwentie correctie in ppm
    {
      lcd.setCursor(11,2);          
      if (Correctie<0) lcd.print("-    "); //('-');                    // plaats het - teken voor negatieve ppm
      else             lcd.print("+    ");                    // plaats het + teken voor positieve ppm 
      lcd.setCursor(12,2);
      // keuze correctie in ppm of in 0.1ppm calibratie *1000 voor ppb of * 10000 voor ppm
      lcd.print (abs(Correctie)); //10));
    
      // Xtal correctie in 0.1ppm
      // lcd.print (abs(Correctie10));
      // lcd.print ('.');
      // lcd.print (abs(Correctie%10));
      // ---------------------------------
      lcd.setCursor(12,2);  
  
      SwitchValue();
      if (Sw == 1){
        Sw=0;
        ConfigFreqCorrection();
      }
    }
   // ************************************************************************************************************
    if (ConfIndex == 1)                                 // max grond frekwentie
    {
      lcd.setCursor(13,2);
      lcd.print (SiMaxFreq);
      SwitchValue();
      if (Sw == 1){
        Sw=0;
        ConfigFundamental();
      }
    }

  // *************************************************************************************************************
      if (ConfIndex == 2)                               // max harmonische
      {
        lcd.setCursor(17,2);
        lcd.print (MaxHarmonic);
        lcd.print(' ');
        SwitchValue();
        if (Sw == 1){
          Sw=0;
          ConfigHarmonic();
        }
      }
  // *************************************************************************************************************
      if (ConfIndex == 3)                               // Drive in mA
      {
        lcd.setCursor(15,2);
        lcd.print (DriveMA);
        lcd.print(' ');
        SwitchValue();
        if (Sw == 1){
          Sw=0;
          ConfigDrive();
        }
      }      
  // ******************************************************************************************************************
      if (ConfIndex == 4)                               // zet waarden in EEPROM
      {
        SwitchValue();
        if (Sw ==1){
          Sw=0;
          StoreConfiguration();
        }
      }
      SwitchValue();                                    // als toets 9 weer ingedrukt dan end loop
    }  // ----------------------------- end while loop ------------------------------------
    lcd.clear();
    ExecFreq();
    ExecLeveldBm();
    lcd.cursor();
    Sw = 0;                             
    //delay (20);     
  //}
}

void StoreConfiguration()
{
  while (Sw == 0){
    //Sw=0;
    //lcd.setCursor(1,1);
    //lcd.print("   Zeker  Weten ?   ");
    lcd.setCursor(0,2);        
    lcd.print(" Level knop opslaan ");
    lcd.setCursor(0,3);        
    lcd.print("annuleren ander knop");
    SwitchValue();
    if (Sw == 1) {                         // als level knop gedrukt
      EEPROM.put(10,Correctie);
      // EEPROM.put(11,SiMaxFreq);
      EEPROM.put(13,MaxHarmonic);  
      lcd.setCursor(0,3);        
      lcd.print("                    ");
      lcd.setCursor(0,2);        
      lcd.print("     OPGESLAGEN     "); 
      delay(2000);  
    }
    if (Sw>1){                               // als andere dan level knop gedrukt
      lcd.setCursor(0,3);        
      lcd.print("                    ");
      lcd.setCursor(0,2);        
      lcd.print("    GEANNULEERD     ");   
      delay(2000);
    }
    //lcd.setCursor(0,2);        
    //lcd.print("                    ");
  }
  Sw=0;
}
// ---------------------------------------------------------------------------------------------
void ConfigFreqCorrection()
//   si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000, 0);    // load capacitance, Xa input Freferentie, Correction in ppb
{
  //int8_t Correctie; //range -128=-12.7ppm / +127=12.7ppm -12800/+12700 ppb in si5351.init(load,referentie,correctie)
  while (Sw!=1)
  {
    newPosition_C = LeveldBmEnc.read();                 // lees de level encoder rotatie
    if (newPosition_C != oldPosition_C) {               // Positie verandering verwerken
      newPos2_C = oldPosition_C;
      oldPosition_C = newPosition_C;
      if (newPosition_C/4 > newPos2_C/4){               // draairichting < en >
        Correctie -- ;                                  // CorrectieFreq 0.1ppm omlaag 
      }
      if (newPosition_C/4 < newPos2_C/4){               // draairichting < en >
        Correctie ++ ;                                  // CorrectieFreq 0.1ppm omhoog
      }
    }
   
        lcd.cursor();                                   // zet underscore aan 
    //lcd.blink();   
      lcd.setCursor(11,2);          
    if (Correctie<0) lcd.print("-    "); //('-');                    // plaats het - teken voor negatieve ppm
    else             lcd.print("+    ");                    // plaats het + teken voor positieve ppm 
    lcd.setCursor(12,2);
    lcd.print (abs(Correctie)); ///10));
    //lcd.print ('.');
    //lcd.print (abs(Correctie%10));
    lcd.setCursor(12,2);  
    Serial.println(Correctie);
    SwitchValue(); 
    delay(10);    
  }
  Sw=0;
  // Dit is een correctie_verandering na initialisatie, daarom is een set_correction vereist
  si5351.set_correction(Correctie*1000,SI5351_PLL_INPUT_XO); // Correctie in ppb
  lcd.noBlink();                                        // 
  lcd.noCursor();                                       // underscore op positie waar de wijzigingen worden gemaakt  
}
// -----------------------------------------------------------------------------------------
void ConfigFundamental()
{
  int Fund = 150;
  int SiMax; // = MaxOscFreq/1000000;  Fund - 300
  while (Sw!=1)
  {
    newPosition_C = LeveldBmEnc.read();                 // lees de level encoder rotatie
    if (newPosition_C != oldPosition_C) {               // Positie verandering verwerken
      newPos2_C = oldPosition_C;
      oldPosition_C = newPosition_C;
      if (newPosition_C/4 > newPos2_C/4){               // draairichting < en >
        SiMax = SiMax-25;                                // SiMaxFreq 1 omlaag
        if (SiMax < 0) SiMax = Fund;                     // bewaking grens SiMaxFreq en loopt rond
      }
      if (newPosition_C/4 < newPos2_C/4){               // draairichting < en >
        SiMax = SiMax+25;                                // SiMaxFreq 1 omhoog
        if (SiMax > Fund) SiMax = 0;                     // bewaking grens SiMaxFreq en loopt rond
      }
    }
    lcd.cursor();                                   // zet underscore aan 
    //lcd.blink();   
    lcd.setCursor(13,2);
    SiMaxFreq = SiMax+Fund;
    lcd.print (SiMaxFreq);
    lcd.print (' ');
    lcd.setCursor(13,2);  
    SwitchValue(); 
    delay(100);    
  }
  Sw=0;
  MaxOscFreq = (SiMaxFreq)*1000000ULL;                  // Frequentie in Hertz
  LL2Serial(MaxOscFreq);
  lcd.noBlink();                                        // 
  lcd.noCursor();                                       // underscore op positie waar de wijzigingen worden gemaakt
}

// ---------------------------------------------------------------------------------------
void ConfigHarmonic()                                  
{                                
while (Sw!=1)                                           // zolang levelknop (1) niet ingedrukt
  {
    newPosition_C = LeveldBmEnc.read();                 // lees de level encoder rotatie
    if (newPosition_C != oldPosition_C) {               // Positie verandering verwerken
      newPos2_C = oldPosition_C;
      oldPosition_C = newPosition_C;
      if (newPosition_C/4 > newPos2_C/4){               // draairichting < en >
        MaxHarmonic = MaxHarmonic-2;                    // MaxHarmonic 1 omlaag
        if (MaxHarmonic < 1) MaxHarmonic = 15;          // bewaking grens MaxHarmonic en loopt rond
      }
      if (newPosition_C/4 < newPos2_C/4){               // draairichting < en >
        MaxHarmonic = MaxHarmonic+2;                                 // MaxHarmonic 1 omhoog
        if (MaxHarmonic > 15) MaxHarmonic = 1;          // bewaking grens MaxHarmonic en loopt rond
      }
    }
    //lcd.setCursor(17,2);                                // laat karakter op cursorplaats knipperen,
    lcd.cursor();                                       // zet underscore aan 
    //lcd.blink();   
    lcd.setCursor(17,2);
    lcd.print (MaxHarmonic);
    lcd.print (' ');
    lcd.setCursor(17,2);  
    SwitchValue(); 
    delay(100);   
  } //-------------------------------);                 // als Sw is 1 dan exit loop
  Sw=0;
  lcd.noBlink();                                        // 
  lcd.noCursor();                                       // underscore op positie waar de wijzigingen worden gemaakt
}
// ---------------------------------------------------------------------------------------
void ConfigDrive()                                      // 2 , 4 , 6 or 8 mA Drive
{                                
while (Sw!=1)                                           // zolang levelknop (1) niet ingedrukt
  {
    newPosition_C = LeveldBmEnc.read();                 // lees de level encoder rotatie
    if (newPosition_C != oldPosition_C) {               // Positie verandering verwerken
      newPos2_C = oldPosition_C;
      oldPosition_C = newPosition_C;
      if (newPosition_C/4 > newPos2_C/4){               // draairichting < en >
        DriveMA = DriveMA -2;                    // MaxHarmonic 1 omlaag
        if (DriveMA < 2) DriveMA = 8;          // bewaking grens MaxHarmonic en loopt rond
      }
      if (newPosition_C/4 < newPos2_C/4){               // draairichting < en >
        DriveMA = DriveMA+2;                                 // MaxHarmonic 1 omhoog
        if (DriveMA > 8) DriveMA = 2;          // bewaking grens MaxHarmonic en loopt rond
      }
    }
    //lcd.setCursor(17,2);                                // laat karakter op cursorplaats knipperen,
    lcd.cursor();                                       // zet underscore aan 
    //lcd.blink();   
    lcd.setCursor(15,2);
    lcd.print (DriveMA);
    lcd.print (' ');
    lcd.setCursor(15,2);  
    SwitchValue(); 
    delay(100);   
  } //-------------------------------);                 // als Sw is 1 dan exit loop
  Sw=0;
  
  if (DriveMA == 2)  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA); 
  if (DriveMA == 4)  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_4MA);   
  if (DriveMA == 6)  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_6MA); 
  if (DriveMA == 8)  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA); 
  //PrintLevel();  //Print L waarde op display
  lcd.noBlink();                                        // 
  lcd.noCursor();                                       // underscore op positie waar de wijzigingen worden gemaakt
}
