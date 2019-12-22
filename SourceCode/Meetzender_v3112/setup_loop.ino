//************************************ Setup ****************************************
void setup() {
  Serial.begin (115200);               // Seriele verbinding in 115200 baud
  //-----------------------------------------------------------------------------------
  // Configuratie waarden uit EEPROM lezen en toepassen
  //-----------------------------------------------------------------------------------
  EEPROM.get(10,Correctie); Serial.println(Correctie); // in ppm, in library moet ppb
  // EEPROM.get(11,SiMaxFreq); Serial.println(SiMaxFreq);
  SiMaxFreq = 150;                     // Default 150MHz 'beperking' Si5351 library >150MHz irrationeel gedrag
  EEPROM.get(13,MaxHarmonic); Serial.println(MaxHarmonic);
  MaxOscFreq = SiMaxFreq*1000000ULL;   // SiMaxFreq =MHz in EEPROM gezet dus omrekenen naar Hz
  //------------------------------------------------------------------------------------------
  pinMode(45, OUTPUT);                 // Dit is de R/W pin van LCD display, moet op LOW
  digitalWrite(45, LOW);               // nu kan er naar display geschreven worden
  lcd.begin(20, 4);                    // declareer LCD als 4 regels van 20 karakters 
  lcd.clear();
  // --------- Start I2C voor Si5351, deze loopt over de I2C verbinding ----------------------
  Wire.begin();                        // initialisatie I2C bus voor Wire library
  TWBR = 10;                           // maat voor snelheid I2C bus -> dit is erg snel

  // ------------- SI5351 setup --------------------
  // INIT string: load capacitance(pF), Xa Input of Referentie Freferentie (Hz), Correction (ppb)
  //si5351.init(SI5351_CRYSTAL_LOAD_0PF, 13000000, 0);  // for VCTCXO  0pf/13MHz
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000, Correctie*1000); // load capacitance, Xa input Freferentie, Correction in ppb
  DriveMA = 2;                                   // Default Drive_2MA , na soft restart Drive = 2 mA
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA); // kan 2,4,6,8 mA zijn. Ongeveer 2mA = 1dBm, 8mA = 11 dBm 
  si5351.output_enable(SI5351_CLK0, 0);                 // zet clk0 aan, door 1 te vervangen door 0 zet je hem uit.
  si5351.output_enable(SI5351_CLK2, 0);                 // andere twee clks staan uit
  si5351.output_enable(SI5351_CLK1, 1);                 // zet clk1 aan


  // ---------- Definieer A0/A3 als analoog in, A4/A7 als digitale output voor LED sturing
  pinMode(A0,INPUT);                   // navigatie switch
  pinMode(A1,INPUT);                   // digitaster switch S6,S7,S8,S9
  pinMode(A2,INPUT);                   // rotary switch S1,S2,S3
  pinMode(A3,INPUT);                   // mux input vanaf ADF435x lock detect
  pinMode(A4,OUTPUT);                  // led on AN4
  pinMode(A5,OUTPUT);                  // led on AN5
  pinMode(A6,OUTPUT);                  // led on AN6
  pinMode(A7,OUTPUT);                  // led on AN7
  
  // ---------- rotary encoders 19/16/A2 en 18/17/A2 A2 leest schakelaar uit
  pinMode(19, INPUT_PULLUP);       // pin D19 is input voor Level verandering encoder
  pinMode(16, INPUT_PULLUP);       // pin D16 is input voor Level verandering
  pinMode(18, INPUT_PULLUP);       // pin D18 is input voor Freq verandering encoder
  pinMode(17, INPUT_PULLUP);       // pin D17 is input voor freq verandering  
  pinMode(2,  INPUT_PULLUP);       // pin D2 is input voor Menu encoder
  pinMode(3,  INPUT_PULLUP);       // pin D3 is input voor Menu encoder
     
  // ---------- besturing van de verzwakker ------------------------
  pinMode(LE1, OUTPUT);
  pinMode(LE2, OUTPUT);
  pinMode(dB05,OUTPUT);
  pinMode(dB1, OUTPUT);
  pinMode(dB2, OUTPUT);
  pinMode(dB4, OUTPUT);
  pinMode(dB8, OUTPUT);
  pinMode(dB16,OUTPUT); 
  
  // -------------- set variabelen van rotary encoders ---------
  oldPosition_f  = -999;                        // Freq instelling (f)
  newPos2_f = oldPosition_f;
  
  oldPosition_L  = -999;                        // LveldBm instelling (L)
  newPos2_L = oldPosition_L;  


 
  // ---------- Level instelling -------------
  Level = 2;
  LeveldBm = LeveldBmArray[Level] - 1;          // start S9 = -73dBm = 50 uV (de -1 compenseert +1 opstart van encoder)

  // ---------- Freq instelling ---------------
  HamBandIndex = 6;                             // Is de HamBand keuze, we starten op 30m. 7.060.000
  lcd.clear();

  ExecFreq();
  ExecLeveldBm();

 // digitalWrite(A7,HIGH);                 // led AN7 ON omdat nu naar HF_mode
  lcd.clear();
  lcd.print(F("HF generator V."));       // toon Si startregel 1 op scherm 
  lcd.print(Versienummer);               // toon versienummer achter regel 1
  lcd.setCursor(3,2);                    // cursor 2 regels omlaag
  lcd.print(F("0.1 - 150 MHz"));         // toon Si startregel 2 op scherm 
  lcd.setCursor(0,3);                    // cursor regel omlaag
 
  //lcd.print(F("15e harm tot 2250MHz"));   // toon startregel 3 op scherm
    lcd.print(MaxHarmonic); 
    lcd.print(F("e harm tot "));
    lcd.print(MaxHarmonic*SiMaxFreq); 
    lcd.setCursor(17,3);
    lcd.print(F("MHz"));                    // toon startregel 3 op scherm
  
  delay(3*delaywaarde);                    // korte tijd blijft setup scherm zichtbaar
  lcd.clear();  
  FStepIndex = 3;                        // kies de Stapgrootte van Freq met FStepIndex
  FStep = FStepArray[FStepIndex] ;       // Initiele stapgrootte om weg te stappen van freq; 3 is per 1000Hz

  Schermwaarde = HamBandArray[HamBandIndex];  // Initiele startfreq uit array,
  Schermwaarde = Schermwaarde + FStep;        // compenseert voor offset opstart van encoder
}
// ----------------------------------- Einde Setup routine -----------------------------------------------------
