// VFO_UP-------------------------------------------------------------------------------------------------------
// VFO_DOWN -----------------------------------------------------------------
// HamBandFunctie
// ---- freq functies ----------------------------------------------------------

// -------------- VFO + FStep functie ---
void VFO_UP() {               
  Schermwaarde = Schermwaarde + FStep;                    // Voor goede werking:
  if (Schermwaarde > (MaxHarmonic * MaxOscFreq-1ULL)) {   // Grondtoon moet < 150MHz blijven
    Schermwaarde = (MaxHarmonic * MaxOscFreq)-1ULL ;      // Is een beperking v/d etherkit library
  }
  ExecFreq();                           // stuur de freq naar VFO en print op LCD
}


// --------------- VFO - FStep functie ---
void VFO_DOWN() {
  if ((Schermwaarde - FStep > 100000000000ULL) || (Schermwaarde-FStep < 10000ULL)){     // Ondergrens bewaking, is 10kHz 
    Schermwaarde = 10000ULL;             // als toekomstige freq 'omklapt'(grote stap) of toekomstige freq onder 10 kHz komt
  }
  else{
  Schermwaarde = Schermwaarde - FStep;
  }
  ExecFreq();                          // stuur de freq naar VFO en print op LCD
}

//=========================================================================================================================
// ------------- HamBand schakelen met druk knop van freq encoder - functie ---------
void HamBandFunctie() {
  lcd.clear();
  lcd.noCursor();                                       // geen storende running cursor
  lcd.print(F(" HamBand keuze menu "));
  long S;
  // met Sw!=8 wordt de hamband freq ingelezen, elke andere toets is escape uit hamband manu                           
  while (Sw==0){                                      //(Sw != 8){
    SwitchValue();                                      // als toets weer ingedrukt Sw wordt 2
    newPosition_M = FreqEnc.read();                     // lees de freq encoder rotatie
    // Positie verandering verwerken
    if (newPosition_M != oldPosition_M) {
      newPos2_M = oldPosition_M;
      oldPosition_M = newPosition_M;

      if (newPosition_M/4 > newPos2_M/4){               // draairichting < en >
        HamBandIndex --;                                // HamBandIndex 1 omlaag
        if (HamBandIndex < 0) {                         // bewaking grens HamBandIndex en loopt rond
          HamBandIndex = HBIndexMax-1;
        }
      }
      if (newPosition_M/4 < newPos2_M/4){               // draairichting < en >
        HamBandIndex ++;                                // HamBandIndex 1 omhoog
          if (HamBandIndex > HBIndexMax-1) {            // bewaking grens HamBandIndex en loopt rond
            HamBandIndex = 0;
          }
      }
    }
    // N.B. hiervoor nog correctie toevoegen als element uit HamBandArray >1000MHz
    lcd.setCursor(3,3);           
    S = HamBandArray[HamBandIndex];
    if ((S/1000000)<10) 
        lcd.print(F(" "));
    if ((S/1000000)<100) 
        lcd.print(F(" "));
    lcd.print(S/1000000);
    lcd.print(F("."));
    if ((S%1000000/1000)<10) 
        lcd.print(F("0"));
    if ((S%1000000/1000)<100) 
        lcd.print(F("0"));
    lcd.print((S%1000000/1000));
    lcd.print(F(","));
 //   lcd.print(F("000 kHz"));

    if ((S%1000)<100)                // modulo-kHz restant Hz als getal < 100
        lcd.print(F("0"));           // print leading zero
    if ((S%1000)<10)                 // modulo-kHz restant Hz als getal < 10
        lcd.print(F("0"));           //   print 2e leading zero
    lcd.print(S%1000);               // print Hz

    lcd.print(F(" kHz"));
    
    lcd.setCursor(2,1);
    lcd.print(HamBandStringArray[HamBandIndex]);
  } // ----------------------------- end while loop ------------------------------------

  // Sw==8 zodat met elke andere toets geen hamband frequntie
  if (Sw==8) Schermwaarde = HamBandArray[HamBandIndex];  // escape hamband
  
  // correctie om frequentie gelijk te houden, 1 count uit encoder 
  newPosition_f = FreqEnc.read(); 
  oldPosition_f = newPosition_f;
  //  newPos2_M = oldPosition_M;  
  // ------- end correctie
  
  lcd.clear();
  ExecFreq();
  ExecLeveldBm();
  lcd.cursor();
  Sw = 0;                                               // zet toetswaarde terug                           
  delay (20);                                           // debounce
}
//--------------------------------------------------------------------------------------------------


// ------------------ FStep veranderen met rotary encoder - functie -----
void FStepFunctie() {                                   // Op basis van de encoder wordt de FStep veranderd
  while (Sw == 0){
    lcd.setCursor(Cursorpos[FStepIndex],0);             // zet cursor op positie waar de wijziging wordt gemaakt
    lcd.cursor();                                       // zet underscore aan 
    lcd.blink();                                        // laat karakter op cursorplaats knipperen,
    SwitchValue();                                      // is Freq toets opnieuw ingedrukt ?
    newPosition_f = FreqEnc.read();                     // lees de freq encoder rotatie
    // Freq verandering verwerken
    if (newPosition_f != oldPosition_f) {
      newPos2_f = oldPosition_f;
      oldPosition_f = newPosition_f;

      if (newPosition_f/4 > newPos2_f/4){               // draairichting < en >
        FStepIndex ++;                                  // FStepIndex 1 omhoog
        if (FStepIndex > 8) {                           // bewaking grens FStepIndex en loopt rond
          FStepIndex = 0;
        }
      }
      if (newPosition_f/4 < newPos2_f/4){               // draairichting < en >
        FStepIndex --;                                  // FStepIndex 1 omlaag
          if (FStepIndex < 0) {                         // bewaking grens FStepIndex en loopt rond 0-8
            FStepIndex = 8;
          }
      }
    }
  }// --------------------------------------------- einde while Sw==0 loop    

  lcd.noBlink();                                        // Hier kom je zodra de knop wordt ingedrukt, ga dan terug naar gewone cursor
  lcd.cursor();                                         // underscore op positie waar de wijzigingen worden gemaakt
  FStep = FStepArray[FStepIndex];                       // 
  Sw = 0;                                               // zet toetswaarde terug   
  delay (20);                                           // debounce    
}
// --------------EINDE  FStep veranderen met rotary encoder - functie -----

// ============================================================================================================
// ------------------Zet freq in VFO en roept ook de Print Frequentie functie aan - functie -------
void ExecFreq() {
  uint64_t Freq;
  long S = Schermwaarde;
    // De freq gaat naar de VFO in hondersten van een Hz, daarom Schermwaarde x 100   
    if ((Schermwaarde >= (0*MaxOscFreq)) && (Schermwaarde<(1*MaxOscFreq))){
      Freq = (100ULL * Schermwaarde);   
    Serial.print("Harm=1 - DisplayVal= "); Serial.print(S);     
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print(F("                    "));
    }
    // Schermwaarde hoger dan 250Mhz, deel door 3 en werk met 3e harmonische, dus Freq = 100/3 x Schermwaarde
    if ((Schermwaarde >= (1*MaxOscFreq)) && (Schermwaarde<(3*MaxOscFreq))) { 
      Freq = (100ULL * Schermwaarde) / 3;
    Serial.print("Harm=3 - DisplayVal= "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F(" 3e harmonic -9.5 dB"));
    }
    if ((Schermwaarde >= (3*MaxOscFreq)) && (Schermwaarde<(5*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 5;
    Serial.print("Harm=5 - DisplayVal= "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F(" 5e harmonic -14 dB ")); // 7e harm -17dB
    }
    if ((Schermwaarde >= (5*MaxOscFreq)) && (Schermwaarde<(7*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 7;
    Serial.print("Harm=7 - DisplayVal= "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F(" 7e harmonic -17 dB ")); // 7e harm -17dB
    }
    if ((Schermwaarde >= (7*MaxOscFreq)) && (Schermwaarde<(9*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 9;
    Serial.print("Harm=9 - DisplayVal= "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F(" 9e harmonic -19 dB ")); // 7e harm -17dB
    }    
    if ((Schermwaarde >= (9*MaxOscFreq)) && (Schermwaarde<(11*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 11;
    Serial.print("Harm=11 - DisplayVal= "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F("11e harmonic -21 dB ")); // 7e harm -17dB
    }  
    if ((Schermwaarde >= (11*MaxOscFreq)) && (Schermwaarde<(13*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 13;
    Serial.print("Harm=13 - DisplayVal=  "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F("13e harmonic -22.5dB")); // 7e harm -17dB
    } 
    if ((Schermwaarde >= (13*MaxOscFreq)) && (Schermwaarde<(15*MaxOscFreq))) {
      Freq = (100ULL * Schermwaarde) / 15;
    Serial.print("Harm=15 - DisplayVal=  "); Serial.print(S);
      lcd.setCursor(0, 1);             // plaats cursus aan begin van regel2
      lcd.print (F("15e harmonic -23.5dB")); // 7e harm -17dB
    }           
  Serial.print(" -  Schermw= "); LL2Serial(Schermwaarde);Serial.print(" -  SiFreq/100:  "); LL2Serial(Freq/100); Serial.println(' ');
  si5351.set_freq(Freq, SI5351_CLK1);        // Stuurt Freq naar VFO, CLock 1
  PrintFreq();                               // Zet freq op display, zo volgt de display altijd de VFO freq.
}
// ===================================================================================================================


// ---------------------- Print Freq op display voor Si5351---------------------
void PrintFreq() {                   // Zet de Freq waarde keurig neer zonder leading zero's en met punt en komma op juiste plek
  lcd.home();                        // positie(0,0)
  lcd.print(F("f= "));               // print startfreq oscillator 
 
  unsigned long S;
  S = Schermwaarde;                  // hulp variable die in MHz , kHz en Hz wordt ontleed
                                     // correctie voor f>1GHz invoegen
//    lcd.setCursor(4, 0);           // plaats cursus aan begin van getal
    lcd.setCursor(3, 0);             // plaats cursus aan begin van getal
    if ((S/1000000)<1000)            // als f<1GHz
        lcd.print(F(" "));
    if ((S/1000000)<100)             // delen MHz als getal < 100
        lcd.print(F(" "));           //   print leading zero
    if ((S/1000000)<10)              // delen MHz als getal < 10
        lcd.print(F(" "));           //   print 2e leading zero
    lcd.print(S/1000000);            // print MHz in 1,2 of 3 digits of 4

    lcd.print(F("."));               // print punt tussen MHz en kHz

    if ((S%1000000/1000)<100)        // modulo-MHz restant delen kHz als getal < 100
        lcd.print(F("0"));           //   print 1e leading zero
    if ((S%1000000/1000)<10)         // modulo-MHz restant delen kHz als getal < 10
        lcd.print(F("0"));           //   print 2e leading zero
    lcd.print((S%1000000/1000));     // print modulo-MHz delen kHz in 1,2 of 3 digits

    lcd.print(F(","));               // print komma tussen kHz en Hz

    if ((S%1000)<100)                // modulo-kHz restant Hz als getal < 100
        lcd.print(F("0"));           // print leading zero
    if ((S%1000)<10)                 // modulo-kHz restant Hz als getal < 10
        lcd.print(F("0"));           //   print 2e leading zero
    lcd.print(S%1000);               // print Hz

    lcd.print(F(" kHz"));
 
  lcd.setCursor(Cursorpos[FStepIndex],0);
  lcd.cursor();                      //zet de underscore (cursor) weer aan op positie waar de wijzigingen worden gemaakt
    //Serial.println(CursorposADF[FStepIndex]);
}


// ----------------------- LeveldBm functies -------------------------------------------------------

// --------- functie --- Level 1 dB omhoog ---------------
void Level_UP() {        
  LeveldBm++;
  if (LeveldBm > 0){
    LeveldBm = 0;
  }
  ExecLeveldBm();  // stuur de de ATT en display aan
}


// --------- functie --- Level 1 dB omlaag --------------
void Level_DOWN() {
  LeveldBm--;
  if (LeveldBm < -150){ // voor de test doeleinden moet dit -150 zijn zodat we de verzwakker kunnnen ijken
    LeveldBm = -150;    // bij gewoon gebruik staat hier -140.
  }
  ExecLeveldBm();
}
  

// ---------- functie ---- roept de ATTSet functie aan die de verzwakker inschakelt, en drukt gewenste Level af op display
void ExecLeveldBm() {      
  ATTSet();                // zet de juiste ATT aan in de verzwakker.
  PrintLevel();            // print dBm niveau op display
}


void ATTSet() {             // schakel de ATT in op bepaalde waarde voor de gewenste output
  // Dit is de versie voor de test: 
  // noteren: gemeten Output vs Freq en LeveldBm van de Arduino om transfer functie te maken.
  digitalWrite(LE1, LOW); // zet fijnlatch of 0
  digitalWrite(LE2, LOW); // zet grote stappen latch op 0
  
  int total;
  int trapatt;
  int rest;
  int grof; // stuurt de grote stappen (= 4 chips tegelijk) minimale stapgrootte= 4 x 0,5 = 2 dB
  int fijn;  // stuurt de fijne stap = per dB

  total = -1* LeveldBm ;     // werk met positieve getallen
  trapatt = total / 5; // hoeveel verzwakking per trap zou er moeten zijn? Er zijn 5 trappen
  rest = total - (5 * trapatt);
  grof = 2 * trapatt;  // schuif bitjes 1x op voor hele dBs
  fijn = 2 * trapatt;  //  schuif bitjes 1x op voor hele dBs
  if (rest == 0){   //iedere trap evenveel verzwakking en hele getallen
  ; // doe niks
  }
  if (rest == 1) { 
    fijn = fijn+2 ;  // fijn is 1 dB hoger want rest is 1, bitjes zijn 1x opgeschoven  vandaar +2
  }
  if (rest == 2) {
    grof = grof +1 ; // er komt zo 4 x 0,5 dB bij  (bij grof gaat alles in 4-voud)
  }
  if (rest == 3) {
    grof = grof + 1;  // 4 x 0,5 dB erbij in grof 
    fijn = fijn + 2;  // 1 dB erbij voor fijn
  }
  if (rest == 4) {
    grof = grof + 2;  // 4 x 1 dB erbij voor grof
  }
  
 
  SchrijfATT(grof);        // zet de data op de pinnen
  digitalWrite(LE2, HIGH); // Met de latch clock van 1ms wordt de waarde ingeklokt
  delay(1);
  digitalWrite(LE2, LOW);
//  lcd.setCursor(7,0);      // schrijf de grof waarde op display = 2x de echte waarde
//  lcd.print(F("  "));
//  lcd.setCursor(7,0);  
//  lcd.print(grof);
  
  SchrijfATT(fijn);        // zet de data op de pinnen
  digitalWrite(LE1, HIGH); // Met de latch clock van 1ms wordt de waarde ingeklokt
  delay(1);
  digitalWrite(LE1, LOW);
//  lcd.setCursor(13,0);      // schrijf de fijn waarde op display = 2x de echte waarde
//  lcd.print(F("  "));
//  lcd.setCursor(13,0);  
//  lcd.print(fijn);  
 
  //  transferfunctie wordt per apparaat gemaakt bij de ijking.
}


void SchrijfATT(int ATT) {
  digitalWrite(dB16, HIGH && (ATT & B00100000));
  digitalWrite(dB8,  HIGH && (ATT & B00010000));
  digitalWrite(dB4,  HIGH && (ATT & B00001000));
  digitalWrite(dB2,  HIGH && (ATT & B00000100));
  digitalWrite(dB1,  HIGH && (ATT & B00000010));
  digitalWrite(dB05, HIGH && (ATT & B00000001));
   
}


// -------------- Schrijft LeveldBm, spanning en S-punt op het display ------ functie --------------------------------
void PrintLevel() {            // Zet alle dBmm waarden keurig neer
  lcd.setCursor(0, 2);                 // positie0, line 2  = 1e positie op regel 3
  lcd.print(F("L= "));                 // print startlevel oscillator   
  lcd.setCursor(3, 2);
  int L1 = -1*LeveldBm;            // L1 is LeveldBm zonder min teken. rekent makkelijker in de software.
  if(L1 == 0){
    lcd.print(F("   0"));         // als LeveldBm =0 dan gewoon 0 schrijven.
  }
  else{                        // als leveldBm oftewel L1 is niet 0
  lcd.print(F("-"));
  int L1a=L1/100;                  // L1a wordt L1/100 en verder nog vastgehouden
    if (L1/100 <1){            // onderdrukken leading zero
      lcd.print(F(" "));
    }
    else {
    lcd.print(L1/100);
    }
 
    L1 = L1 - ((L1/100)*100);
    if ((L1/10 <1) && (L1a<1)){    // onderdrukken tweede leading zero
    lcd.print(F(" "));
    }
  
    else {
      lcd.print(L1/10);
    }
    L1 = L1 - ((L1/10)*10);   
    lcd.print(L1);
  }
  lcd.print(F(" dBm"));

  // ---- zet nu de Level in uV of mV neer.
  lcd.setCursor(4,3);
  L1 = -1*LeveldBm;
  lcd.print(uVArray[L1]);          // print hier het aantal uV of mV, staan simpel opgeslagen in een Array 
  lcd.noCursor();                  // zet cursor uit.

  // ---- Ook nog een S-meter waarde afdrukken +/- zoveel dB
  lcd.setCursor(12,3);
  lcd.print(F("S"));
  int SdB;
  int dB;
  int S;
  SdB = LeveldBm+127;  // SdB is verschoven schaal: 0 = S0, +54 = S9, enz.
  S = SdB/6;            // berekend S punt
  if (S<0) {            // als S<0 dan S=0
    S=0;  
  }
  if (S>9) {            // als S>9 dan S=9
    S=9;
  }
  dB = SdB - S*6;
  lcd.print(S);
  if (dB>=0) {
    lcd.print(F(" +"));
    if (dB<10) {
      lcd.print(F(" "));
    }
    lcd.print(dB);
  }
  if (dB<-9) {
    lcd.print(F(" "));
    lcd.print(dB);
  }
  if (dB<0 && dB>-10) {
    lcd.print(F(" - "));
    lcd.print(-1*dB);
  }
  lcd.print (F("dB"));
 

  if (DriveMA == 2) {
    //lcd.setCursor(0,1); 
    //lcd.print(F("                    "));
    lcd.setCursor(11,2); 
    lcd.print(F("         "));    
  }
  if (DriveMA == 4) {
    //lcd.setCursor(0,1);
    //lcd.print(F("Drive 2 mA L=  +6 dB")); 
    lcd.setCursor(11,2); 
    lcd.print(F(" L= +6 dB"));    
  }
  if (DriveMA == 6) {
    //lcd.setCursor(0,1);
    //lcd.print(F("Drive 4 mA L= +9.5dB")); 
    lcd.setCursor(11,2); 
    lcd.print(F(" L=+9.5dB"));        
  }
  if (DriveMA == 8) {
    //lcd.setCursor(0,1); 
    //lcd.print(F("Drive 8 mA L= +12 dB"));
    lcd.setCursor(11,2); 
    lcd.print(F(" L= +12dB"));        
  }  
  lcd.noCursor();     
}


// leest schakelaar van druktoetsen en schakelaars op encoders
void SwitchValue()                     
{
  // ------------------------------    bepaal toets digitasters type S6-S7-S8-S9
  uint8_t valA1 = analogRead(A1)>>2;   // delen door 4 zodat waarde < 255
  delay(2);
  int resettijd = 0;

  do
  {
    if (valA1 < 250)                   // als een digitast ingedrukt
    {
      if (valA1 > 160) Sw=9;           // S9_Toets
      if (valA1 < 160) Sw=8;           // S8_Toets
      if (valA1 < 48)  Sw=7;           // S7_Toets
      if (valA1 < 4)   Sw=6;           // S6_Toets
      delay(2);                        // debounce waarde
    }
    valA1 = analogRead(A1)>>2;         // 
    // Serial.print(Sw);Serial.print(' ');Serial.println(valA1);
    if (Sw==9) { resettijd = resettijd+1;} // tel aantal loops, 1 loop ca. 2mS
  }
  while(valA1 < 250) ;                 // wacht tot toets losgelaten
  
  if (Sw==9 && resettijd > 1000) {  // als S9 ca. 3 seconden ingedrukt dan soft reset
     Sw = 0;                        // zodat na softreset Sw geen 9 is
     //Serial.println (resettijd);
     delay(5);
     setup();
  }

  // ------------------------------    bepaal toets pushbuttons rotary switch S1-S2-S3
  uint8_t valA2 = analogRead(A2)>>2;   // delen door 4 zodat waarde< 255
  delay(2);
  do
  {
    if (valA2 < 250)                   // als een rotary switch ingedrukt
    {
      if (valA2 > 64) Sw=2;            // Menu_Toets niet in gebruik
      if (valA2 < 48) Sw=3;            // Freq_Toets
      if (valA2 < 4)  Sw=1;            // Level_Toets
      delay(2);                        // debounce waarde
    }
    valA2 = analogRead(A2)>>2;
    //Serial.println(valA2);
  }
  while(valA2 < 250) ;                 // wacht tot toets losgelaten
}




// -------------------------------------------------------------------------------------------------------
// -------- EEPROM funties -----------------------------------------------------
// -------- Put EEPROM functie -  Schrijft Level en freq in de EEPROM ---------------
void PUTEEPROM() { 
  int eeAdres=0;
  EEPROM.put(eeAdres,LeveldBm);
  eeAdres += sizeof(int);
  EEPROM.put(eeAdres,Schermwaarde);
}

// -------- GET EEPROM functie -  Haal Level en freq op uit de EEPROM ---------------
void GETEEPROM() {
  int eeAdres=0;
  EEPROM.get(eeAdres,LeveldBm);
  eeAdres += sizeof(int);
  EEPROM.get(eeAdres,Schermwaarde);
}


// ------------------------------------------------------------------------------------------------------------
// PRINT Long Long integer on Serial (forum.arduino.cc/index.php?topic=298161.0 for function which includes a fault)
void LL2Serial(long long ll)
{
  uint64_t xx = ll/1000000000ULL;
  if (xx >0) Serial.print((long)xx);
  if((ll-xx*1000000000) <100000000 )Serial.print("0");
  if((ll-xx*1000000000) <10000000 )Serial.print("0");   
  if((ll-xx*1000000000) <1000000 )Serial.print("0");  
  if((ll-xx*1000000000) <100000 )Serial.print("0");  
  if((ll-xx*1000000000) <10000 )Serial.print("0");  
  if((ll-xx*1000000000) <1000 )Serial.print("0");  
  if((ll-xx*1000000000) <100 )Serial.print("0");
  if((ll-xx*1000000000) <10 )Serial.print("0"); 
  Serial.print((long)(ll-xx*1000000000));
}
// ****************************************
