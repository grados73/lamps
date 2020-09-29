/*
 * Author: Grad
 * Date: 16.09.2020r
 * ZADANIA: 
 * CO JESLI JEST KIERUNEK ZWYKŁY I WCISNIEMY STOP 
 * CO JESLI JEST KIERUNEK Z STOP/COFANIE I WYLACZYMY STOP -> DODAC FLAGE OD KIERUNKU I JAK WYLACZAMY STOP TO WCHODZI DO FUNKCJI ZWYKLEGO KIERUNKU
 * OGARNAC CO JESLI STOP/ COFANIA I AWARYJNE, ZEBY POZYCJA GASLY
 *
 */

#include <Adafruit_NeoPixel.h>

#define ledCount 30 //ilosc diod w pasku
#define ledStripPin1t 12 //podlaczenie lewego paska tyl
#define ledStripPin2t 11 //podlaczenie prawego paska tyl
#define Brightness 5 // max 255 - jasnosc
#define dlugoscMigacza 7  //dlugosc weza migacza
#define opoznienieZmianyMigacza 20 //czas w [ms] po jakim ma sie zmieniac stan diod przy kierunkowskazach

//Sygnaly sterujace
#define sstop 1  // swiatlo stopu
#define swiatlaDzienTyl 5 
#define swiatlaCofania 7
#define awaryjne 2 //swiatla awaryjne
#define kierunekPrawy 9
#define kierunekLewy 6
#define wlaczenieSystemu 4
#define wylaczanieSystemu 14
#define wylaczKierunki 12
#define wylaczStop 11
#define wylaczSwiatlaCofania 17
#define wylaczDzienTyl 15


Adafruit_NeoPixel strip1t(ledCount, ledStripPin1t, NEO_GRB + NEO_KHZ800); // tyl lewy
Adafruit_NeoPixel strip2t(ledCount, ledStripPin2t, NEO_GRB + NEO_KHZ800); //tyl prawy


String odebraneDane = ""; //Pusty ciąg odebranych danych
int aktualneZadanieKierunki = 0;
int aktualneDaneLampaTyl = 0;
int odebraneDaneInt = 0;
int flagaTylDzien = 0;
int flagaTylStop = 0;
int flagaTylCofanie = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //inicjalizacji transmisji szeregowej - ustawic predkosc na !!!115200!!!
  strip1t.begin();
  strip2t.begin();
  strip1t.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  strip2t.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  clearLed12t();
}

void loop() {
  //#################### OBSLUGA POLECEN ODEBRANYCH OD PORTU SZEREGOWEGO ##################
    if(Serial.available() > 0) { //Jesli Arduino odebrało dane  sterujace
        odebraneDane = Serial.readStringUntil('\n'); 
        odebraneDaneInt = StoI_f(odebraneDane); // konwersja string na int

        switch(odebraneDaneInt){
          case sstop:  // SWIATLO STOP - LAMPA TYL
            flagaTylStop = 1;
            if(flagaTylCofanie == 0){ // wcisniety stop kiedy nie ma swiatla cofania
                swiatlo_stop();
                aktualneDaneLampaTyl = 1; 
            }
            else if(flagaTylCofanie == 1){
                swiatlo_stop();
                aktualneDaneLampaTyl = 1; 
            }
          break;

          case awaryjne: // SWIATLA AWARYJNE - OBA KIERUNKI SWIECA
            if(flagaTylStop or flagaTylCofanie){// JESLI JEST ZAPALONE SWIATLO COFANIA / SWIATLO STOP
              swiatla_awaryjne_tyl_stop_cofanie();
              aktualneZadanieKierunki = 3;
            }
            else {// JESLI NIE JEST WLACZONE SWIATLO COFANIA / SWIATLO STOP
                swiatla_awaryjne();
                aktualneZadanieKierunki = 2;
                aktualneDaneLampaTyl = 0;
            }
          break;
          
          case kierunekLewy:  // KIERUNEK LEWY
            if(flagaTylStop or flagaTylCofanie){ // JESLI JEST ZAPALONE SWIATLO COFANIA / SWIATLO STOP
                kierunkowskazy_tyl_stop_cofanie(6);
                aktualneDaneLampaTyl =61;
            }
            else{ // JESLI NIE JEST WLACZONE SWIATLO COFANIA / SWIATLO STOP
                kierunkowskazy_tyl(6);
                aktualneDaneLampaTyl = 6;
                aktualneZadanieKierunki = 6; 
            } 
          break;

          case kierunekPrawy: // KIERUNEK PRAWY
            if(flagaTylStop or flagaTylCofanie){ // JESLI JEST ZAPALONE SWIATLO COFANIA / SWIATLO STOP
                kierunkowskazy_tyl_stop_cofanie(9);
                aktualneDaneLampaTyl =91;
            }
            else{ // JESLI NIE JEST WLACZONE SWIATLO COFANIA / SWIATLO STOP
                kierunkowskazy_tyl(9);
                aktualneDaneLampaTyl = 9;
                aktualneZadanieKierunki = 9; 
            }
              
          break;

          case swiatlaDzienTyl: // SWIATLA DZIENNE - LAMPA TYL
            swiatlo_dzien_tyl();
            flagaTylDzien = 1;
            aktualneDaneLampaTyl = 5; 
          break;

          case swiatlaCofania:  // SWIATLA COFANIA -  LAMPA TYL           
            swiatlo_cofania(flagaTylDzien);
            flagaTylCofanie = 1;
            aktualneDaneLampaTyl = 7;    
          break;
          
          case wlaczenieSystemu: //  ANIMACJA URUCHAMIANIA SYSTEMU
                 swiatlo_dzien_tyl();
                 flagaTylDzien = 1;
          break;

          case wylaczanieSystemu: // ANIMACJA WYLACZANIA SYSTEMU
              flagaTylDzien = 0;
          break;
 
          case wylaczKierunki: //WYLACZ AWARYJNE / WYLACZ KIERUNKI
            clearLed12t();
            aktualneZadanieKierunki = 12;
            if(flagaTylStop == 1) aktualneDaneLampaTyl= 1;
            else if(flagaTylDzien == 1 && flagaTylStop == 0) aktualneDaneLampaTyl= 5; // jest pozycja dzien, ale nie ma stop
            else if(flagaTylStop == 1) aktualneDaneLampaTyl= 1; //
            if( flagaTylCofanie == 1) aktualneDaneLampaTyl= 7;
          break;

          case wylaczStop: // WYLACZANIE STOPU - LAMPA TYL
            clearLed12t();
            flagaTylStop = 0;
            if(flagaTylDzien = 0) aktualneDaneLampaTyl = 0;    //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH5
            if(flagaTylDzien = 1) aktualneDaneLampaTyl = 5;
            if(flagaTylCofanie == 1) aktualneDaneLampaTyl = 7;
            
          break;

          case wylaczSwiatlaCofania: //  WYLACZ SWIATLA COFANIA - LAMPA TYL - ZOSTAJE POZYCJA
            clearLed12t();
            flagaTylCofanie = 0;
            if(flagaTylDzien == 0) aktualneDaneLampaTyl = 0;
            else if(flagaTylStop == 1) aktualneDaneLampaTyl = 1;
            else aktualneDaneLampaTyl = 5; 
          break;  

          case wylaczDzienTyl: // WYLACZ SWIATLA DZIENNE - LAMPA TYL
            clearLed12t();
            flagaTylDzien = 0;
            aktualneDaneLampaTyl = 0;
          break;  
        }      

    }
// ################ AKTUALNY STAN LAMP #########################
  if(aktualneZadanieKierunki == 2) swiatla_awaryjne();
  if(aktualneZadanieKierunki == 3)  swiatla_awaryjne_tyl_stop_cofanie();
  if(aktualneZadanieKierunki == 6) kierunkowskazy_tyl(6);
  if(aktualneZadanieKierunki == 9) kierunkowskazy_tyl(9);    
  if(aktualneDaneLampaTyl == 1) {
    swiatlo_stop();
    if(flagaTylCofanie == 1) swiatlo_cofania(flagaTylDzien);
  }
    else if(aktualneDaneLampaTyl == 5) swiatlo_dzien_tyl();
  if(aktualneDaneLampaTyl == 15) ;
  if(aktualneDaneLampaTyl == 7) swiatlo_cofania(flagaTylDzien);
  if( aktualneDaneLampaTyl == 91)kierunkowskazy_tyl_stop_cofanie(9);
  if( aktualneDaneLampaTyl == 61)kierunkowskazy_tyl_stop_cofanie(6);
}

// ################ FUNKCJE REALIZUJACE STEROWANIE SWIATLAMI ###################

/// ############### KIERUNKOWSKAZY ################################
void swiatla_awaryjne() // OBYDWA KIERUNKOWSKAZY
{
  for( int i = ledCount-1; i >= 0; i--){  
     strip1t.setPixelColor(i, strip1t.Color(255, 215, 0));
     strip2t.setPixelColor(i, strip2t.Color(255, 215, 0));
     delay(opoznienieZmianyMigacza);
   
       if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
        strip1t.setPixelColor(i+dlugoscMigacza, strip1t.Color(0, 0, 0));
        strip2t.setPixelColor(i+dlugoscMigacza, strip2t.Color(0, 0, 0));
       }
     strip1t.show();
     strip2t.show();
     }
        for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
        strip1t.setPixelColor(k, strip1t.Color(0, 0, 0));
        strip2t.setPixelColor(k, strip2t.Color(0, 0, 0));
        strip1t.show();
        strip2t.show();
        delay(opoznienieZmianyMigacza);   
        }
}
void swiatla_awaryjne_tyl_stop_cofanie(){
         for( int i = ledCount/2; i >= 0; i--){  
         strip1t.setPixelColor(i, strip1t.Color(255, 215, 0));
         strip2t.setPixelColor(i, strip2t.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza+10);
         if( i< ledCount/2-dlugoscMigacza+1){ //gaszenie paska od poczatku
                strip1t.setPixelColor(i+dlugoscMigacza, strip1t.Color(0, 0, 0));
                strip2t.setPixelColor(i+dlugoscMigacza, strip2t.Color(0, 0, 0));
         }
          strip2t.show();
          strip1t.show();
          }
       for(int k = dlugoscMigacza-1; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip1t.setPixelColor(k, strip1t.Color(0, 0, 0));
           strip2t.setPixelColor(k, strip1t.Color(0, 0, 0));
           strip1t.show();
           strip2t.show();
           delay(opoznienieZmianyMigacza+10);   
        }
}

void kierunkowskazy_tyl(int ktory){ // JEDEN KIERUNKOWSKAZ, PRZYJMUJE ZMIENNA WSKAZUJACA KTORY - 6 -LEWY, 9 -PRAWY
    if(ktory == 6){ //lewy
      clearLed1t();
       for( int i = ledCount-1; i >= 0; i--){  
         strip1t.setPixelColor(i, strip1t.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza);
         if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
                strip1t.setPixelColor(i+dlugoscMigacza, strip1t.Color(0, 0, 0));
         }
          strip1t.show();
          }
       for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip1t.setPixelColor(k, strip1t.Color(0, 0, 0));
           strip1t.show();
           delay(opoznienieZmianyMigacza);   
        }
  }
    else if(ktory == 9){ //prawy
       for( int i = ledCount-1; i >= 0; i--){  
         strip2t.setPixelColor(i, strip2t.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza);
         if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
                strip2t.setPixelColor(i+dlugoscMigacza, strip1t.Color(0, 0, 0));
         }
          strip2t.show();
          }
       for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip2t.setPixelColor(k, strip2t.Color(0, 0, 0));
           strip2t.show();
           delay(opoznienieZmianyMigacza);   
        }
  }
}
void kierunkowskazy_tyl_stop_cofanie(int ktory){
   if(ktory == 6){ //lewy
       for( int i = ledCount/2; i >= 0; i--){  
         strip1t.setPixelColor(i, strip1t.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza+10);
         if( i< ledCount/2-dlugoscMigacza+1){ //gaszenie paska od poczatku
                strip1t.setPixelColor(i+dlugoscMigacza, strip1t.Color(0, 0, 0));
         }
          strip1t.show();
          }
       for(int k = dlugoscMigacza-1; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip1t.setPixelColor(k, strip1t.Color(0, 0, 0));
           strip1t.show();
           delay(opoznienieZmianyMigacza+10);   
        }
  }
    else if(ktory == 9){ //prawy 
       for( int i = ledCount/2; i >= 0; i--){  
         strip2t.setPixelColor(i, strip2t.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza+10);
         if( i< ledCount/2-dlugoscMigacza+1){ //gaszenie paska od poczatku
                strip2t.setPixelColor(i+dlugoscMigacza, strip2t.Color(0, 0, 0));
         }
          strip2t.show();
          }
       for(int k = dlugoscMigacza-1; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip2t.setPixelColor(k, strip2t.Color(0, 0, 0));
           strip2t.show();
           delay(opoznienieZmianyMigacza+10);   
        }
  }
}

//################## LAMPA PRZOD FUNKCJE ################################
  
//################## LAMPA TYL FUNKCJE ################################

void swiatlo_stop(){
  for (int i = 0; i < ledCount; i++) {
    strip1t.setPixelColor(i, strip1t.Color(255, 0, 0));
    strip2t.setPixelColor(i, strip2t.Color(255, 0, 0));
  }
  strip1t.show();
  strip2t.show();
}

void swiatlo_dzien_tyl(){
  for (int i = 0; i < (ledCount/3); i++) {
    strip1t.setPixelColor(i, strip1t.Color(255, 0, 0));
    strip2t.setPixelColor(i, strip2t.Color(255, 0, 0));
  }
  strip1t.show();
  strip2t.show();
}


void swiatlo_cofania(int flagaDzienTyl){
    for (int i = int((2*ledCount/3-2)); i < (ledCount-2); i++) {
    strip1t.setPixelColor(i, strip1t.Color(255, 255, 255));
    strip2t.setPixelColor(i, strip2t.Color(255, 255, 255));
  }
  if (flagaDzienTyl == 1) swiatlo_dzien_tyl();
  strip1t.show();
  strip2t.show();
}


// Funkcja Macka


//######################## FUNKCJE CZYSZCZENIA PASKOW ############################

void clearLed1t() {
  for (int i = 0; i < ledCount; i++) {
    strip1t.setPixelColor(i, 0, 0, 0);
  }
  strip1t.show();  
}

void clearLed2t() {
  for (int i = 0; i < ledCount; i++) {
    strip2t.setPixelColor(i, 0, 0, 0);
  }
  strip2t.show();  
}

void clearLed12t() {
  for (int i = 0; i < ledCount; i++) {
    strip1t.setPixelColor(i, 0, 0, 0);
    strip2t.setPixelColor(i, 0, 0, 0);
    
  } 
  strip1t.show();
  strip2t.show();
}
//############### FUNKCJA DO KONWERSJI ODEBRANEGO Z PORTU SZEREGOWEGO STRINGA NA WARTOSC INT DO POROWNANIA W SWITCH-CASE#####################
int StoI_f(String daneString){
  int odebraneDaneInt = 0;
  if( daneString == "1") odebraneDaneInt = 1;
  if( daneString == "2") odebraneDaneInt = 2;
  if( daneString == "3") odebraneDaneInt = 3;
  if( daneString == "4") odebraneDaneInt = 4;
  if( daneString == "5") odebraneDaneInt = 5;
  if( daneString == "6") odebraneDaneInt = 6;
  if( daneString == "7") odebraneDaneInt = 7;
  if( daneString == "8") odebraneDaneInt = 8;
  if( daneString == "9") odebraneDaneInt = 9;
  if( daneString == "10") odebraneDaneInt = 10;
  if( daneString == "11") odebraneDaneInt = 11;
  if( daneString == "12") odebraneDaneInt = 12;
  if( daneString == "13") odebraneDaneInt = 13;
  if( daneString == "14") odebraneDaneInt = 14;
  if( daneString == "15") odebraneDaneInt = 15;
  if( daneString == "16") odebraneDaneInt = 16;
  if( daneString == "17") odebraneDaneInt = 17;
  if( daneString == "18") odebraneDaneInt = 18;
  if( daneString == "19") odebraneDaneInt = 19;
  //else odebraneDaneInt = 0;
  return odebraneDaneInt;
}
