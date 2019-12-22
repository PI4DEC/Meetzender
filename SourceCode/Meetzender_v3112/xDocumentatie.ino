/*
Version 3112
  Configuratie menu uitgebreid met Drive mA 2,4,6 of 8 mA
  SI5351MaxFreq default 150 MHz.  Instelbaar tot 300 MHz wordt niet in EEPROM geschreven.
  Harmonic factor instelbaar 3e, 5e, 7e, 9e, 11e, 13e, 15e. tot max 2250MHz

Version 3110
  Met drukknop S7 naar calibratie menu, zelfde drukknop >5 sec een soft reset.
  Library beperking max Freq tot 150MHz daarna PLL uit lock niet doorgaand afstembaar in 3e harmonische
  max si5351 waarop deze nog stabiel werkt instelbaar van 150-300 MHz
  wordt opgeslagen als SiMaxFreq=150 MHz en omgezet naar MaxOscFreq (ULL) in Hz
  maximale harmonische die gebruikt wordt voor de frequentie instelling. , 
  Calibratie correctie voor Xtal variabel gemaakt int8_t -128/127
  in ppm van -128ppm tot +127ppm of in 0.1ppm van -12.8ppm tot +12.7 ppm
  voor goedkope Xtal neem ppm, voor betere xtallen in 0.1 ppm
  
Version 310
  Extra lcd.clear() in setup
  frequentie basis tot 250 MHz
  3e harmonic tot 750 MHz  1/3 = -9.54dB  ~ -10dB
  5e harmonic tot 1250MHz  1/5 =  -14dB

Version308HF2 meld zich als V.30802
  Soft Restart toegevoegd, omdat soms de display rotzooi toont.
  Mode switch S9 (rechtonder) >3 seconden ingedrukt dan naar 'void setup()'

     
Version308HF meldt zich als V.3080
  Alles voor ADF435x eruit

Version 3.08
  Sweep menu uitgeschakeld
  S6 Store Frequency in EEprom
  S7 Get Frequency from EEprom
   
Version 307
  For digitasters S9 = HF / V-UHF  S8 = HambandMenu , S7 = SweepMenu , S6 = MemoryMenu    
  S9 Mode omschakeling geimplementeerd
  S8 HamBand omschakeling geimplementeerd    

Version 306 (zoals 206 dan voor nieuwe print)
  For testboard and new controlboard HF on CLK_0 , Fref on CLK_1
  LiquidCrystal lcd(46,44,39,38,37,36); // LiquidCrystal(rs, e, d4, d5, d6, d7) , display pootjes: 4,6,11,12,13,14 
  
Vanaf Version 300 nieuwe  besturingsprint
  Display op pin 
  SPI bus enabled op 10 polige plug
                 
Version 203
  without UBlox module
  New pushbutton routine for 3 rotary switches
  Menu button switches bands

Version 124
  ADFregisters worden alleen geschreven als INTA veranderd of indien een unlkocked situatie optreedt
  als basisVCO buiten de subbandgrenzen valt doordat NewReference is veranderd
  Variabelen voor: ADFreference en PfdRFout
                    
Version 121
  Verwisseld  CLK0 en CLK2 outputs, in software alle aanroepen naar CLK0 en CLK2 aangepast.
  Oude versies werken daardoor niet meer met de testhardware
  
Version 120
  Werkt alleen met SI5351 etherkit library versie 2.0 en hoger
  PPS menu om UBlox en CLK2 variabel te maken of op 0 te zetten.
  PPS menu stuurt CLK2 uitgang enable/disable
  LL2Serial routine om long long in monitor te printen met LL2Serial(var) i.p.v. Serial.print(var)
  bitSet reg[2],8 , enabled integer-N digital lock detect
  
Version 118
  Removed MOD and FRAC as only integer INT is used Integer synthesiser
  ADF4350 25MHz reference is tuned up to make steps possible of 10Hz  Fref:25000000.00 - 25284089.09       

Version 117
  for 13Mhz VC-TCXO injected in Xa of Si5351. UBlox 1MHz reference for PLL

  
Version 116
  ADFreference = 25e6
  Stap grootte is variabel d.m.v. menu
  debug op regel 1 naar keuze 
  

Version 115
  Draairichting F encoder verwisseld VFO_UP en VFO_DOWN andersom, 
  Duidelijke verbetering van nabije spurs d.m.v. Algoritme van Euclides toegepast op MOD en FRAC
  Grootste Gemene deler toegepast zodat 500:4000 wordt 1:8 en 912:4000 wordt 57/250


Version 114
  Enkele variabel namen veranderd. Nieuwe variabelen rond analogRead(A0)
  Setup verbeterd, na opstart mode gedefenieerde initiele waarden voor ADF en Si
   
Version 113
  Minimum Frequentie(137.5), Stapgrootte, PfdRFout en MOD zijn variabel
  correctie voor Stapgrootte op hogere banden

Version 112
  SchermwaardeULL zodat frequenties in Hz,  hulpS nodig om ULL in long te converteren
  INTA en FRAC makkelijk te berekenen
  
  
Version 111
  Frequentie berekening veranderd, INTA en FRAC worden nu vanuit frequentie berekend
  Frequentie in 10Hz zodat F nog als long getal kan blijven // Long=4.294.967.296  *10=42.949.672.96


Version 110  //  
  Only ADF
  Freference =32MHz // R-deler=32 Fpd=1MHz // INTA 137-4400
  Fstep minimum= 1MHz/4000=250Hz
  calc OutputDivider aangepast INTA:10
  Zijband ruis op ca +-4.5kHz -50 dB
  Zijband spurs op elke 90Hz 20dB boven ruis tot op +-1.5kHz
  Zijband spurs op iedere 1.25kHz tot op +- 6.25kHz
  
Version 102
  Hamband frequenties in een nieuw scherm kiezen met Freq_rotary 
  in Init routine afhankelijk van mode VCO uitschakelen of VCO inschakelen. VCO power Down
  variabele delaywaarde gemaakt, zodat zichtbaarheidstijd van schermen is aan te passen.
                  
Version 101:
  frequentie staat nu op regel 0
  SPI op nieuwe manier,  parameters zoals snelheid makkelijk te wijzigen
  loop gesplitst in adfloop, siloop en normale loop
  ook initSi en iniyADF toegevoegd

ADFfunctions
void VFO_UP_ADF()
void VFO_DOWN_ADF()
void WriteRegister32(const uint32_t value)
void SetADF4351()
void CalculateOutputDivider()
void ExecFreqADF()
void FStepFunctieADF()
void PrintFreqADF()

SIfunctions
void PUTEEPROM()
void GETEEPROM()
void VFO_UP()
void VFO_DOWN()
void FStepFunctie()
void HamBandFunctie()
void ExecFreq()
void PrintFreq()
void Level_UP()
void Level_DOWN()
void ExecLeveldBm()
void ATTSet()
void SchrijfATT(int ATT)
void PrintLevel()


   STEP:2.2G-4.4G/1kHz;  1.1G-2.2G/0.5kHz;  550M-1.1G/0.25KHz;  275M-550/0.125KHz;  137.5M-275M/62.5Hz
   F-PhaseDetector  4000*1kHz = 4MHz

   STEP:2.2G-4.4G/1.6kHz;  1.1G-2.2G/800Hz;  550M-1.1G/400Hz;  275M-550/200Hz;  137.5M-275M/100Hz
   F-PhaseDetector  4000*1.6kHz = 6.4MHz
   
   Tijdens opstarten, in de setup A0 default met PULLUP start op in Si5351 mode, 
   als A0 aan GND start op in ADF435x mode
                   
    Frequentie aanduiding:  GHz // Mega Hertz   //  Kilo-Hertz     //100/25Hz      
    Cursorpostie ADF          3      4    5    6    8 //   9     10   12  13
    Stapgrootte FStepArray    10000, 1000, 100, 10,   1 // 10000, 1000, 100 25
    FStepIndex Array          8      7    6    5    4 //   3     2    1   0 

Arduino pin   ADF4351 pin   ADF4351 Mnemonic  Functionality
9   53   LE  Frequency device update
11  51   DATA  SPI Master Out Slave In (MOSI)
13  50   CLK   SPI Serial Clock (SCK)
--  --   CE  SPI Slave Select (SS), indien nodig, is al hoog op ADF Board

 
PfdRFout == Reference frequency, board Xtal = 25.000000 Mhz of extern een andere frequentie.
b.v. OCXO 10MHz , deeltal R aanpassen naar 100.
b.v. 12 MHz of 6 MHz van een UBlox NEO-7N module met TCXO en GPS deeltal R aanpassen naar 120/60

 analogRead(A0)  is button pin
 analogRead(A2)  is schakelaar detectie
 digitalRead(A3) is Lock detect pin
 
*/
