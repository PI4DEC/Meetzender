/*
    Deze versie werkt met de verbeterde besturingsprint van de Meetzender en gebruikt CLK1 van de SI5351 als output
    LiquidCrystal lcd(46, 44, 39,38,37,36)
    LCD aan pins 44,45 en 46, zodat pins 50,51,52 en 53 vrij blijven als SPI bus. 
*/
// ------------------------------------------------------------------------------------------------------------
//---------------------------------VERSIE 0.85 ----------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------


/* Arduino Mega 2560 kern met Encoder en Etherkit 5351 libraries
Versie 0.6 heeft een encoder voor de frequentie en een up en down toets.
Er zijn diverse funties gedefinieerd.
Versie 0.61 heeft een variabele FStep die uit de FStepArray komt. 
Versie 0.62 heeft grenzen 100 kHz - 159.999,000 MHz en hambands
Versie 0.63 heeft Hambands drukschakelaar  
Versie 0.64 met twee encoders en dBm instelling op diversie start niveaus met druk schakelaar.
Versie 0.65 schrijft netjes de SchermwaardeString (=schermwaarde) en leveldBm met punten, komma's, min teken enz.
Versie 0.66 bevat een dBm to uV functie en drukt deze af
Versie 0.67 zijn pinnen van encoder verplaatst om 2+3 vrij te maken, evenzo de display naar de dubbele rij aansluitingen,
            encoders naar: 18-17 en 19-16, displaypootjes 4,6,11,12,13,14 aan arduino 42,40,32,34,36,38
Versie 0.68 freq knop lang indrukken --> stapgrootte instellen, waren voorheen twee knoppen. die zijn nu weg. 
            S-meter waarden op de display     

Versie 0.7  Bevat een routine om de output te ijken.
            Lang en kort drukken op frq encoder: kort = verander stapgrootte, lang = verander hamband               
Versie 0.71 De 70 cm band toegevoegd als 3e harmonische
            Schermwaarde is leidend in freq verwerking, waar mogelijk is de ULL verkleind naar UL, (bv Schermwaarde is nu UL ipv ULL)
Versie 0.72 ULL variabelen hersteld. Boven een bepaalde frequentie anders geen goede data naar de VFO. Verder output power op 4mA gezet ipv 2mA
Versie 0.73 Nieuwe ATT verdeling over de verschillende chips.
Versie 0.74 Wegschrijven en ophalen LeveldBm en Schermwaarde in EEPROM, wegschrijven gebeurt met kort naar 0 halen van pin A7 (momory toets).
Versie 0.8  pins aangepast aan bedienprint versie 1
Versie 0.84 werkende versie, debugged
Versie 0.85 vertaal polynoom plaats aangegeven in funvtie en gebruikte de nieuwste 5351 bibliotheek 
*/

// ------------------------------------------------------------------------------------------------------------  
//--------------------------- Libraries -----------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#include <Encoder.h>                   // rotary encoder, wordt aan interupt gehangen voor snelle reactie
#include <si5351.h>                    // voor SI5351 - dit is de etherkit library
#include <Wire.h>                      // voor I2C bus, is standaard voor Arduino
#include <LiquidCrystal.h>             // voor LCD display (is een 4 regel - 20 char display)
#include <EEPROM.h>                    // EEPROM gebruik (t.b.v. opslaan instelling bij uitschakelen)


Si5351 si5351;                         // initialiseer de Si5351 routine
                                       // LiquidCrystal(rs, e, d4, d5, d6, d7) instelling met gewone display 4x20, display pootjes: 4,6,11,12,13,14 
LiquidCrystal lcd(46, 44, 39,38,37,36); // 
Encoder FreqEnc(18,17);                // Rotary Encoder Freq aansluiting, midden aan massa (interupt op pin 18)
Encoder LeveldBmEnc(19,16);            // Rotary Encoder Level aansluiting, midden aan massa (interupt op pin 19)

//  ---------------------------------einde library zaken --------------------------------------------------------



// -----------------------------------------------------------------------------------------------------------------------------
// ---------------------------------- Variabelen -------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------

String Versie = "Versie 0.85_CLK1_DEC";
String CALL = "PA3A";

// variabelen voor de Synth
//unsigned long long Freq;                // freq voor de VFO als unsigned 64 bit getal ( =ULL ), hoogste waarde Freq = 15.000.000.000 ( te lang voor gewone unsigned long))
unsigned long long FStepArray[9] = {1,10,100,1000,10000,100000,1000000,10000000,100000000} ;
unsigned long long FStep;               // stapgrootte om te veranderen 1 Hz tot 100 MHz
unsigned long long HamBandArray[16] = {137000,475000,1860000,3560000,5360000,7060000,10120000,14060000,18080000,21060000,24920000,28060000, 50060000,70060000,144060000,432260000}; // inclusief 70cm
int HamBandIndex;
int FStepIndex ;
signed int teller;         // teller van tijd dat freqknop wordt INGEDRUKT ivm schakelen FStepIndex
int val; // hulp variabele
int valF; // hulpvariabele in Frq deel
int i; // idem
unsigned long long Schermwaarde; // wat is de gewenste freq die ook op het scherm staat


int Cursorpos[9] = {14,13,12,  10,9,8,   6,5,4}; // cursor positie in Freq uitlezing
String uVArray[151]=                             // 141 dBm waarden omgezet naar spanning incl eenheid. Simpel maar doeltreffend.
{"224  mV",
 "199  mV","178  mV","158  mV","141  mV","126  mV","112  mV","100  mV"," 89  mV"," 79  mV"," 71  mV",
 " 63  mV"," 56  mV"," 50  mV"," 45  mV"," 40  mV"," 35  mV"," 32  mV"," 28  mV"," 25  mV"," 22  mV",
 " 20  mV"," 18  mV"," 16  mV"," 14  mV"," 13  mV"," 11  mV"," 10  mV","8,9  mV","7,9  mV","7,1  mV",
 "6,6  mV","5,6  mV","5,0  mV","4,5  mV","4,0  mV","3,5  mV","3,2  mV","2,8  mV","2,5  mV","2,2  mV",
 "2,0  mV","1,8  mV","1,6  mV","1,4  mV","1,3  mV","1,1  mV","1,0  mV","890  uV","790  uV","710  uV",
 "630  uV","560  uV","500  uV","450  uV","400  uV","350  uV","320  uV","282  uV","251  uV","224  uV",
 "199  uV","178  uV","158  uV","141  uV","126  uV","112  uV","100  uV"," 89  uV"," 79  uV"," 71  uV",
 " 63  uV"," 56  uV"," 50  uV"," 45  uV"," 40  uV"," 35  uV"," 32  uV"," 28  uV"," 25  uV"," 22  uV",
 " 20  uV"," 18  uV"," 16  uV"," 14  uV"," 13  uV"," 11  uV"," 10  uV","8,9  uV","7,9  uV","7,1  uV",
 "6,6  uV","5,6  uV","5,0  uV","4,5  uV","4,0  uV","3,5  uV","3,2  uV","2,8  uV","2,5  uV","2,2  uV", 
 "2,0  uV","1,8  uV","1,6  uV","1,4  uV","1,3  uV","1,1  uV","1,0  uV","0,89 uV","0,79 uV","0,71 uV",
 "630  nV","560  nV","500  nV","450  nV","400  nV","350  nV","320  nV","282  nV","251  nV","224  nV",
 "199  nV","178  nV","158  nV","141  nV","126  nV","112  nV","100  nV"," 89  nV"," 79  nV"," 71  nV",
 " 63  nV"," 56  nV"," 50  nV"," 45  nV"," 40  nV"," 35  nV"," 32  nV"," 28  nV"," 25  nV"," 22  nV",
 " 20  nV"," 18  nV"," 16  nV"," 14  nV"," 13  nV"," 11  nV"," 10  nV","8,9  nV","7,9  nV","7,1  nV",
// "0,66 uV","0,56 uV","0,50 uV","0,45 uV","0,40 uV","0,35 uV","0,32 uV","0,28 uV","0,25 uV","0,22 uV",
// "0,20 uV","0,18 uV","0,16 uV","0,14 uV","0,13 uV","0,11 uV","0,10 uV","0,09 uV","0,08 uV","0,07 uV",
// "0,063uV","0,056uV","0,050uV","0,045uV","0,040uV","0,035uV","0,032uV","0,028uV","0,025uV","0,022uV",
// "n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ","n.v.t. ", 
};


// variabelen voor Encoders
long oldPosition_f;  // hoort bij Freq verandering
long newPosition_f;
long newPos2_f ;

long oldPosition_L;  // hoort bij LeveldBm verandering
long newPosition_L;
long newPos2_L ;


#define UP 1
#define DOWN -1


// variabelen voor de verzwakker
signed int LeveldBm;
int Level;
int LeveldBmArray[6] = {0, -33, -73, -109, -127, -140};   // standaard levels voor S9+x, S3, S0 enz.

// aansturen van verzwakker
#define  LE1 14     // latch voor fijnregeling
#define  LE2   15   // latch voor grove 20dB stappen
#define  dB05  8    // bus pinnen 8 - 13 voor setting van verzwakkers
#define  dB1   9
#define  dB2   10
#define  dB4   11
#define  dB8   12
#define  dB16  13



// -------------------------------------------------------------------------------------------------------
// -------------------------------------------  Functies -------------------------------------------------
// -------------------------------------------------------------------------------------------------------


// -------- EEPROM funties -----------------------------------------------------

// -------- Put EEPROM functie -  Schrijft Level en Freq in de EEPROM ---------------
void PUTEEPROM(){ 
  int eeAdres=0;
  EEPROM.put(eeAdres,LeveldBm);
  eeAdres += sizeof(int);
  EEPROM.put(eeAdres,Schermwaarde);
  
}

// -------- GET EEPROM functie -  Haal Level en Freq op uit de EEPROM ---------------
void GETEEPROM() {
  int eeAdres=0;
  EEPROM.get(eeAdres,LeveldBm);
  LeveldBm--; //compenseer de startpuls van de encoder
  eeAdres += sizeof(int);
  EEPROM.get(eeAdres,Schermwaarde);
  Schermwaarde = Schermwaarde - FStep; //compenseer de startpuls van de encoder
}




// ---- Freq functies ----------------------------------------------------------

// -------------- VFO + FStep functie ---
void VFO_UP() {               
  Schermwaarde = Schermwaarde + FStep;
  if (Schermwaarde > 440000000ULL){        // Bovengrens bewaking
    Schermwaarde = 440000000ULL;
  }
  ExecFreq();                           // stuur de freq naar VFO en print op LCD
}


// --------------- VFO - FStep functie ---
void VFO_DOWN() {
  if ((Schermwaarde - FStep > 1000000000ULL) || (Schermwaarde-FStep < 100000ULL)){     // Ondergrens bewaking, is 100kHz 
    Schermwaarde = 100000ULL;             // als toekomstige Freq 'omklapt'(grote stap) of toekomstige Freq onder 100 kHz komt
  }
  else{
  Schermwaarde = Schermwaarde - FStep;
  }
  ExecFreq();                          // stuur de freq naar VFO en print op LCD
}


// ------------------ FStep veranderen met rotary encoder - functie -----
void FStepFunctie(){     // Op basis van de encoder wordt de FStep veranderd
  int valFreq = analogRead(A2);
//  Serial.println("Fstep  ");   // debugging
//  Serial.println(valFreq);
  while (valFreq <=120 || valFreq >=165)  {                  // voorheen digitalRead(A0)!=LOW) zolang de knop NIET opnieuw wordt ingedrukt..
    lcd.setCursor(Cursorpos[FStepIndex],1);
    lcd.cursor();                                       //zet de underscore (cursor) aan op positie waar de wijzigingen worden gemaakt
    lcd.blink();                                        //laat karakter op cursorplaats knipperen,
    valFreq = analogRead(A2);                           // tijdens de while do blijven scannen
    
    newPosition_f = FreqEnc.read();                     // lees de freq encoder rotatie
    // Freq verandering verwerken
    if (newPosition_f != oldPosition_f) {
      newPos2_f = oldPosition_f;
      oldPosition_f = newPosition_f;

      if (newPosition_f/4 < newPos2_f/4){
        FStepIndex ++;                                  // FStepIndex 1 omhoog
        if (FStepIndex > 8) {                           // bewaking grens FStepIndex en loopt rond
          FStepIndex = 0;
        }
      }
      if (newPosition_f/4 > newPos2_f/4){
        FStepIndex --;                                  // FStepIndex 1 omlaag
          if (FStepIndex < 0) {                         // bewaking grens FStepIndex en loopt rond
            FStepIndex = 8;
          }
      }
    }
  }      
  lcd.noBlink();                              // Hier kom je zodra de knop wordt ingedrukt, ga dan terug naar gewone cursor
  lcd.cursor();
  ExecFreq();
  FStep = FStepArray[FStepIndex];
  
  valFreq = analogRead(A2);
//  Serial.print("Fstep2  ");  // debug
//  Serial.println(valFreq);   // debug
  while (valFreq >= 120 && valFreq <= 165) {              //houd het programma hier vast tot knop weer los is. Daarna pas weer verder. Anders wordt de HamBand veranderd.
    valFreq = analogRead(A2);                             // blijven scannen binnen de while do
    delay(1);
  }
  delay(5);                                   // debounce 5ms
}
// --------------EINDE  FStep veranderen met rotary encoder - functie -----



// ------------- HamBand schakelen met druk knop van freq encoder - functie ---------
void HamBandFunctie(){
  HamBandIndex++;
  if (HamBandIndex>15) {
    HamBandIndex = 0;
  }
  Schermwaarde = HamBandArray[HamBandIndex];
  ExecFreq();
  delay (200);
}



// ------------------Zet freq in VFO en roept ook de Print Frequentie functie aan - functie -------
void ExecFreq() {
uint64_t Freq;
 
    if (Schermwaarde >= 150000000ULL)           // Schermwaarde hoger dan 150Mhz, deel door 3 en werk met 3e harmonische, dus Freq = 100/3 x Schermwaarde
      Freq = (100ULL * (Schermwaarde / 3ULL));
    else
      Freq = (100ULL * Schermwaarde);                 // De freq gaat naar de VFO in hondersten van een Hz, daarom Schermwaarde x 100  
     
  si5351.set_freq(Freq, SI5351_CLK1);  // Stuurt Freq naar VFO, CLock 1
  PrintFreq();                               // Zet freq op display, zo volgt de display altijd de VFO freq.
}


// ---------------------- Print Freq op display ---------------------
void PrintFreq() {                   // Zet de Freq waarde keurig neer zonder leading zero's en met punt en komma op juiste plek
  unsigned long S;
  S = Schermwaarde;                  // hulp variable die stap voor stap wordt ontleed
  lcd.setCursor(4, 1);               // plaats cursus aan begin van getal

                                     // Op het scherm brengen gaat per 10-factor tegelijk, eenvoudig zichtbaar hieronder:
  int f1=S/100000000;                // eerst delen voor het aantal 100-den MHz en f1 vasthouden
  if (f1 <1){
    lcd.print(" ");                  // als onder de 100 MHz dan spatie afdrukken, onderdrukt dus leading zero.
  }
  else {
    lcd.print(S/100000000);          // er zijn een aantal 100-den MHz dus druk het aantal 100-den MHz af
  }
  S = S - ((S/100000000)*100000000); //trek de 100-den MHz van de waarde af
  
  if ((S/10000000 <1) && (f1<1)){    //zijn er geen 10-tallen MHz en was eerste getal een " "?
    lcd.print(" ");                  // druk dan weer een spatie een spatie. We onderdrukken zo
  }                                  // de tweede leading zero
  else {
    lcd.print(S/10000000);           // zo ja, druk het aantal 10-tallen Mhz af
  }
  S = S - ((S/10000000)*10000000);   // trek de 10-tallen MHz van de waarde af.
  
  lcd.print(S/1000000);              // druk enkele MHz af
  S = S - ((S/1000000)*1000000);     // trek de aantal MHz af van de waarde
  
  lcd.print(".");
  
  lcd.print(S/100000);               // 100-den kHz
  S = S - ((S/100000)*100000);      
  lcd.print(S/10000);                // 10 tallen kHz
  S = S - ((S/10000)*10000); 
  lcd.print(S/1000);                 // kHz 
  S = S - ((S/1000)*1000);        
  
  lcd.print(",");
  
  lcd.print(S/100);                  // 100-den Hz
  S = S - ((S/100)*100);
  lcd.print(S/10);                   // 10-tallen Hz
  S = S - ((S/10)*10); 
  lcd.print(S);                      // Hz-en
  lcd.print(" kHz");
 
 
  lcd.setCursor(Cursorpos[FStepIndex],1);
  lcd.cursor();                      //zet de underscore (cursor) weer aan op positie waar de wijzigingen worden gemaakt
  
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
  // ----------------- hier komt de vertaling naar juiste geijkte Niveau
  total = -1* LeveldBm ;     // werk met positieve !!! HIER KOMT OOK DE CONVERSIE VAN LEVELDBM NAAR DE GEWENSTE VERZWAKKING VOLGENS POLYNOOM
  // ----------------- einde vertaling
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
  lcd.setCursor(7,0);      // schrijf de grof waarde op display = 2x de echte waarde
  lcd.print("  ");
  lcd.setCursor(7,0);  
  lcd.print(grof);
  
  SchrijfATT(fijn);        // zet de data op de pinnen
  digitalWrite(LE1, HIGH); // Met de latch clock van 1ms wordt de waarde ingeklokt
  delay(1);
  digitalWrite(LE1, LOW);
  lcd.setCursor(13,0);      // schrijf de fijn waarde op display = 2x de echte waarde
  lcd.print("  ");
  lcd.setCursor(13,0);  
  lcd.print(fijn);  

  
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
  lcd.setCursor(3, 2);
  int L1 = -1*LeveldBm;            // L1 is LeveldBm zonder min teken. rekent makkelijker in de software.
  if(L1 == 0){
    lcd.print("   0");         // als LeveldBm =0 dan gewoon 0 schrijven.
  }
  else{                        // als leveldBm oftewel L1 is niet 0
  lcd.print("-");
  int L1a=L1/100;                  // L1a wordt L1/100 en verder nog vastgehouden
    if (L1/100 <1){            // onderdrukken leading zero
      lcd.print(" ");
    }
    else {
    lcd.print(L1/100);
    }
 
    L1 = L1 - ((L1/100)*100);
    if ((L1/10 <1) && (L1a<1)){    // onderdrukken tweede leading zero
    lcd.print(" ");
    }
  
    else {
      lcd.print(L1/10);
    }
    L1 = L1 - ((L1/10)*10);   
    lcd.print(L1);
  }
  lcd.print(" dBm");

  // ---- zet nu de Level in uV of mV neer.
  lcd.setCursor(4,3);
  L1 = -1*LeveldBm;
  lcd.print(uVArray[L1]);          // print hier het aantal uV of mV, staan simpel opgeslagen in een Array 
  lcd.noCursor();                  // zet cursor uit.

  // ---- Ook nog een S-meter waarde afdrukken +/- zoveel dB
  lcd.setCursor(12,3);
  lcd.print("S");
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
    lcd.print(" +");
    if (dB<10) {
      lcd.print(" ");
    }
    lcd.print(dB);
  }
  if (dB<-9) {
    lcd.print(" ");
    lcd.print(dB);
  }
  if (dB<0 && dB>-10) {
    lcd.print(" - ");
    lcd.print(-1*dB);
  }
  lcd.print ("dB");
  lcd.noCursor();    
}



// ---------------------------------- einde functies --------------------------------------------------  



// ------------------------------------------------------------------------------------------------------------
//------------------------------------------------------ Set Up -----------------------------------------------
// ------------------------------------------------------------------------------------------------------------
void setup() {
//  Serial.begin(4800);    // Debug
  // Set up van Display
  pinMode(45, OUTPUT);      // Dit is de R/W pin van het display, moet op LOW
  digitalWrite(45, LOW);
  
  lcd.begin(20,4);                 // 20 bij 4 display, lcd.begin(colums, rows)
  //lcd.print("V ");
  lcd.print(Versie);
  delay(3000);
  lcd.setCursor(0,0);
  lcd.print("ATT G=    F=    ");  // dient om de grove en fijne ATT te laten zien (test doeleinden)
  //lcd.print((char)244); // Ohm-teken

  lcd.setCursor(0, 1);             // positie 0, regel 1  = 1e plek op regel 2.
  lcd.print("f= ");                // print startfreq oscillator 
  
  lcd.setCursor(0, 2);
  lcd.print("L= ");                // print startlevel oscillator  
 
  
  // --------- Start I2C voor Si5351, deze loopt over de I2C verbinding
  Wire.begin();
  TWBR = 10;                       // maat voor snelheid I2C bus -> dit is erg snel



  // ---------- rotary encoders 19/16/A2 en 18/17/A2
  pinMode(19, INPUT_PULLUP);       // pin D19 is input voor Level verandering encoder
  pinMode(16, INPUT_PULLUP);       // pin D16 is input voor Level verandering
  // Level schakelaar knop wordt via A2 uitgelezen
  
  pinMode(18, INPUT_PULLUP);      // pin D18 is input voor Freq verandering encoder
  pinMode(17, INPUT_PULLUP);      // pin D17 is input voor freq verandering  
  // knop voor bandwijziging enz wordt via A2 uitgelezen

 // pinMode(A7, INPUT_PULLUP);      // write to EEPROM schakelaar, nu buiten werking


 
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
  
  oldPosition_f  = -999;                     // Freq instelling (f)
  newPos2_f = oldPosition_f;
  
  oldPosition_L  = -999;                     // LveldBm instelling (L)
  newPos2_L = oldPosition_L;  

  
  
  // ---------- Freq instelling ------------------
  
  FStepIndex = 3;                                      // kies de Stapgrootte van Freq met FStepIndex
  FStep = FStepArray[FStepIndex] ;                     // Initiele stapgrootte om weg te stappen van freq; 3 is per 1000Hz
  HamBandIndex = 6 ;                                   // Is de HamBand keuze, we starten op 30m.
  Schermwaarde = HamBandArray[HamBandIndex] - FStep;  // Initiele startfreq 10120  (de -FStep compenseert de +1 opstart van de encoder)

  
  // ----------- Level instelling ----------------

  Level = 2;
  LeveldBm = LeveldBmArray[Level] - 1;                 // starten met S9 = -73dBm = 50 uV (de -1 compenseert de +1 opstart van de encoder)

  // ---------- EEPROM lezen om weggeschreven waarden op te halen-----------------   staat nu uit
//  GETEEPROM();                                        // Leest LeveldBm en Schermwaarde uit EEPROM

  

// ------------- SI5351 setup --------------------

  si5351.init(SI5351_CRYSTAL_LOAD_8PF,0, 0);           //specify the load capacitance of the reference crystal, and to use the default reference oscillator frequency of 25 MHz

  si5351.set_correction(1,SI5351_PLL_INPUT_CLKIN);      // Is een eenmalige ijking om set op zijn freq goed te zetten. Is bepaald met IJK programma. Arie: 600
  
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA); // kan 2,4,6,8 mA zijn. Ongeveer 2mA = 1dBm, 8mA = 11 dBm 
  si5351.output_enable(SI5351_CLK0, 0);                 // zet clk0 aan, door 1 te vervangen door 0 zet je hem uit.
  si5351.output_enable(SI5351_CLK2, 0);                 // andere twee clks staan uit
  si5351.output_enable(SI5351_CLK1, 1);


// -------- zet de start Freq en start LeveldBm in VFO en ATT op het display ----
 
  ExecFreq();     // print de Freq meteen op het scherm voor de start en stuur de freq naar de VFO
  ExecLeveldBm(); // idem voor LeveldBm op het scherm en de benodigde ATT naar de verzwakker 
  
       

}



// ------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Loop --------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void loop() {

  // ----------------------Eerst Rotary encoder algorithme ---
      
  newPosition_f = FreqEnc.read();            // lees de freq encoder rotatie
  newPosition_L = LeveldBmEnc.read();        // lees de Level encoder rotatie

  // Eerst Freq verandering verwerken
  if (newPosition_f != oldPosition_f) {
    newPos2_f = oldPosition_f;
    oldPosition_f = newPosition_f;

    if (newPosition_f/4 > newPos2_f/4){
      VFO_UP();
    }
     if (newPosition_f/4 < newPos2_f/4){
      VFO_DOWN();
    }    
  }
  
  // Nu LeveldBm verandering verwerken
  if (newPosition_L != oldPosition_L) {
    newPos2_L = oldPosition_L;
    oldPosition_L = newPosition_L;

    if (newPosition_L/4 > newPos2_L/4){
      Level_UP();
    }
     if (newPosition_L/4 < newPos2_L/4){
      Level_DOWN();
    }    
  }


  
  // --------  Andere toetsen lezen en iets mee doen, in dit geval de FStep grootte veranderen. 
  

  teller = 0;
  valF = analogRead(A2);
 // Serial.print("teller =");    // debug
 // Serial.println(teller);
 // Serial.println(valF);
  
  while (valF >=120 && valF <= 165) {                  // waarde 125  als de knop wordt ingedrukt
    teller++ ;
    valF = analogRead(A2);                            // blijven scannen tijdens de while do
    delay(1);
  }
  if (teller >= 300){  // knop > 0,5 sec ingedrukt dan FStep instellen met de FStepFunctie
    HamBandFunctie();
  }
  if (teller > 1 && teller < 300) {   // knop <0,5 sec ingedrukt dan HamBand veranderen met HamBandFunctie
     FStepFunctie();
  }
  


// --- Lees Encoder druktoets LeveldBmEnc om Level te veranderen (schakelt o.a. S9, S9+40, enz)   
  if (analogRead(A2)<30) {
  //  Serial.println("level");  // debug
    Level++;
  
    if (Level>5) {
      Level = 0;
    }
    LeveldBm = LeveldBmArray[Level];
    ExecLeveldBm();
    delay (300);
  }


// ---------------- EEPROM SAVE ----------------
//  if (digitalRead(A7)==LOW) {
//    PUTEEPROM();
//    delay (300);
//  }

}
  


// ------------------------------------------------------------------------------------------------------------  
// -------------------------------------------- end loop ------------------------------------------------------    
// ------------------------------------------------------------------------------------------------------------


//--------------------------------------- FM en AM TRIALS -------------------------
/*
// FM deel: 6 stappen van 1 kHz omhoog en ook weer naar beneden 
  for (i=0; i <= 5; i++){
      Freq = Freq + 100000;
      si5351.set_freq(Freq, SI5351_PLL_FIXED, SI5351_CLK0);
  }
    for (i=0; i <= 5; i++){
      Freq = Freq - 100000;
      si5351.set_freq(Freq, SI5351_PLL_FIXED, SI5351_CLK0);
  }
*/

/*
  Amplitude variatie  -- AM trial  -- gebruiken we niet in deze FM trial.
  si5351.output_enable(SI5351_CLK0, 1); // zet clk0 aan ofuit, 0 = uit, 1 = aan
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
//  delayMicroseconds(90);  
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
 // delayMicroseconds(10);
 si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);
 // delayMicroseconds(20);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
//  delayMicroseconds(90);
 si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);
//  delayMicroseconds(10);
 si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
//  delayMicroseconds(10);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  //delayMicroseconds(20);
  si5351.output_enable(SI5351_CLK0, 0); // zet clk0 aan ofuit, 0 = uit, 1 = aan  
  //delayMicroseconds(20);
*/
