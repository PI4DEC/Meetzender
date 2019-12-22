#include <Encoder.h>                   // rotary encoder, wordt aan interupt gehangen voor snelle reactie
#include <si5351.h>                    // voor SI5351 - N.B. dit is versie 2.0.6 . Versie 1.2 werkt niet met deze sketch
#include <Wire.h>                      // voor I2C bus, is standaard voor Arduino
#include <LiquidCrystal.h>             // voor LCD display (is een 4 regel - 20 char display)
#include <EEPROM.h>                    // EEPROM gebruik (t.b.v. opslaan instelling bij uitschakelen)

#define Versienummer 3112              // zichtbaar tijdens opstarten

Si5351 si5351;                         // initialiseer de Si5351 routine
LiquidCrystal lcd(46,44,39,38,37,36);  // LiquidCrystal(rs, e, d4, d5, d6, d7) , display pootjes: 4,6,11,12,13,14 
Encoder FreqEnc(18,17);                // Rotary Encoder Freq aansluiting, midden aan massa (interupt op pin 18)
Encoder LeveldBmEnc(19,16);            // Rotary Encoder Level aansluiting, midden aan massa (interupt op pin 19)
//  ---------------------------------einde library zaken --------------------------------------------------------


// -----------------------------------------------------------------------------------------------------------------------------
// ---------------------------------- Variabelen voor Frequentie ----------------------------------------------------------------
//
// variabelen voor de Synth
//long long Freq ; // freq voor VFO, unsigned 64 bit getal ( =ULL ), 15.000.000.000 ( te lang voor unsigned long))
long long FStepArray[9] = {1,10,100,1000,10000,100000,1000000,10000000,100000000} ;
long long FStep;              // stapgrootte om te veranderen 1 Hz tot 100 MHz
#define HBIndexMax 19                  // maximum aantal elementen van de HamBand Array
long long HamBandArray[HBIndexMax] = {32768,137000,475000,1860000,3560000,5360000,7060000,10120000,14060000,
18080000,21060000,24920000,28060000, 50060000,70060000,144060000,220060000,432260000,433920000}; // 1296000000 inclusief 70cm

String HamBandStringArray[HBIndexMax]= {
"   Clock 2^15 Hz "," 2200 meter Band ","  630 meter Band ","  160 meter Band ","   80 meter Band ","   60 meter Band ","   40 meter Band ",
"   30 meter Band ","   20 meter Band ","   17 meter Band ","   15 meter Band ","   12 meter Band ","   10 meter Band ","    6 meter Band ",
"    4 meter Band ","    2 meter Band  "," 1.4 meter Band ","  70 centimeter  ","  70 cm ISM Band ", };

int HamBandIndex;
int FStepIndex ;
int teller;         // teller van tijd dat freqknop wordt INGEDRUKT ivm schakelen FStepIndex
long long Schermwaarde; // wat is de gewenste freq die ook op het scherm staat
long long MaxOscFreq;// = 150000000; // Maximale frequentie waar Si5351 nog stabiel werkt 260*5 = 1300
int8_t MaxHarmonic = 1;    
uint8_t CalIndex; // 
int Cursorpos[9] =   {14,13,12,  10,9,8,   6,5,4}; // 3xMHz, 3xkHz,3xHz cursor positie in freq uitlezing
uint8_t Sw;                                        // Variabelen voor druktoets uitlezing -------------------------------------------
int delaywaarde = 1500;                // millies tijdsduur dat schermen zichtbaar zijn
// ------------------------------------------------------------------------------------------------
int8_t DriveMA;
String EEpromString = "                    ";
String uVArray[151]=   // 151 dBm waarden omgezet naar spanning incl eenheid. Simpel maar doeltreffend.
{"224  mV",                                                                                    // 0dBm
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
 "2,0  uV","1,8  uV","1,6  uV","1,4  uV","1,3  uV","1,1  uV","1,0  uV","890  nV","790  nV","710  nV",
 "630  nV","560  nV","500  nV","450  nV","400  nV","350  nV","320  nV","282  nV","251  nV","224  nV",
 "199  nV","178  nV","158  nV","141  nV","126  nV","112  nV","100  nV"," 89  nV"," 79  nV"," 71  nV",
 " 63  nV"," 56  nV"," 50  nV"," 45  nV"," 40  nV"," 35  nV"," 32  nV"," 28  nV"," 25  nV"," 22  nV",
 " 20  nV"," 18  nV"," 16  nV"," 14  nV"," 13  nV"," 11  nV"," 10  nV","8,9  nV","7,9  nV","7,1  nV", 
};

// variabelen voor Encoders
long oldPosition_f;  // hoort bij freq verandering
long newPosition_f;
long newPos2_f ;

long oldPosition_L;  // hoort bij LeveldBm verandering
long newPosition_L;
long newPos2_L ;

byte oldPosition_M;  // hoort bij Menu verandering
byte newPosition_M; 
byte newPos2_M; 

byte oldPosition_C;  // hoort bij Calibratie Menu verandering
byte newPosition_C; 
byte newPos2_C;   
// variabelen voor de verzwakker
int LeveldBm;
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
