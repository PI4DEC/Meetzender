__[SourceCode folder-view](/SourceCode)__

    Eenvoudigste is om de (meest recente) zipfile hieronder aan te klikken. 
    De [download] knop staat dan aan de rechter-zijde, naast de [history] knop.
    Plaats de uitgepakte zip-file in de Arduino sketch folder, waarna het compileren
    en uploaden naar een Mega2560 eenvoudig moet lukken.

    Zorg er wel voor dat een recente versie van de "Etherkit Si5351 by Jason Milldrum"  geinstalleerd is.


__[**Versie 3112.zip** : ](/SourceCode/Meetzender_v3112.zip)__ (december 2019)

   [Handleiding v3112](Handleiding3112.txt)
   
    Deze versie ondersteund diverse menu's, waardoor een verhoogd gebruiksgemak.
    Via menu kan een correctiefactor worden ingevoerd waarmee de afwijking van het 
    referentie Xtal op de oscillator module kan worden gecorrigeerd.
    Instelbaar Hamband menu, keuze van voorgeprogrammeerde frekwenties/golflengtes.
    Instelbare harmonische factor die de maximale aangegeven frekwentie bepaalt.
    Voor het geval dat 0dBm uitgangsvermogen te laag is kan via een menu tijdelijk 
    het uitgangsvermogen verhoogd worden tot ca +12 dBm. (Drive 2, 4, 6 of 8 mA)


__[Versie 085 CLK1 dec.zip : ](/SourceCode/Meetzender_0_85_CLK1_dec.zip)__(2018)

    >Deze versie is versie 085 aangepast aan de recente besturingsprint.
    Het signaal staat op CLK1, de middelste SMA connector.
    LCD display gebruikt pin 44 en 46, daardoor is pin 50 t/m 53 bruikbaar als SPI bus.


__[Versie 085.zip : ](/SourceCode/Meetzender_0_85.zip)__(2017)

    Dit is de oorspronkelijke versie geschreven door Arie PA3A.
    Werkt niet meer met de meest recente besturingsprint.
    LCD display gebruikt Arduino pin 50 en 52, waardoor de SPI bus geblokkeerd is.

