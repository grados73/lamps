/*
 * Author: Grad
 * Date: 16.09.2020r
 * Finish: 29.09.2020r
 * SWIATLA PRZEDNIE
 *
 */

#include <Adafruit_NeoPixel.h>

#define ledCount 30 //ilosc diod w pasku
#define ledStripPin1 10 // podlaczenie lewego paska przod
#define ledStripPin2 9 //podlaczenie prawego paska przod
#define Brightness 5 // max 255 - jasnosc
#define dlugoscMigacza 7  //dlugosc weza migacza
#define opoznienieZmianyMigacza 20 //czas w [ms] po jakim ma sie zmieniac stan diod przy kierunkowskazach

//Sygnaly sterujace
#define swiatloDzienPrzod 3
#define awaryjne 2 //swiatla awaryjne
#define kierunekPrawy 9
#define kierunekLewy 6
#define wlaczenieSystemu 4
#define wylaczanieSystemu 14
#define wylaczKierunki 12
#define wylaczLampaPrzod 13




Adafruit_NeoPixel strip1(ledCount, ledStripPin1, NEO_GRB + NEO_KHZ800); //przod lewy
Adafruit_NeoPixel strip2(ledCount, ledStripPin2, NEO_GRB + NEO_KHZ800); // przod prawy


String odebraneDane = ""; //Pusty ciąg odebranych danych
int aktualneZadanieKierunki = 0;
int aktualneDaneLampaPrzod = 0;
int odebraneDaneInt = 0;
int flagaDzien = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //inicjalizacji transmisji szeregowej - ustawic predkosc na !!!115200!!!
  strip1.begin(); // INICJALIZACJA PASKOW
  strip2.begin();
  strip1.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  strip2.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  clearLed12();

}

void loop() {
  //#################### OBSLUGA POLECEN ODEBRANYCH OD PORTU SZEREGOWEGO ##################
    if(Serial.available() > 0) { //Jesli Arduino odebrało dane  sterujace
        odebraneDane = Serial.readStringUntil('\n'); 
        odebraneDaneInt = StoI_f(odebraneDane); // konwersja string na int

        switch(odebraneDaneInt){

          case swiatloDzienPrzod:
            swiatlo_dzien_przod();  //SWIATLA DO JAZDY DZIENNEJ - LAMPA PRZOD
            aktualneDaneLampaPrzod = 3;  
            flagaDzien = 3; 
          break;  

          case awaryjne: // SWIATLA AWARYJNE - OBA KIERUNKI SWIECA
            swiatla_awaryjne();
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH
            aktualneZadanieKierunki = 2;
          break;
          
          case kierunekLewy:  // KIERUNEK LEWY
            kierunkowskazy(6);
            aktualneZadanieKierunki = 6;
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH  

          break;

          case kierunekPrawy: // KIERUNEK PRAWY
            kierunkowskazy(9);
            aktualneZadanieKierunki = 9;
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH
              
          break;
          
          case wlaczenieSystemu: //  ANIMACJA URUCHAMIANIA SYSTEMU
              inicjalizacja_systemu();
              aktualneDaneLampaPrzod = 3; // AUTOMATYCZNE SWIATLA DZIENNE, W CELU WLACZENIA PO INICJALIZACJI ZAKOMENTOWAC + FLAGE FLAGADZIEN!
              flagaDzien = 3;
          break;

          case wylaczanieSystemu: // ANIMACJA WYLACZANIA SYSTEMU
              wylaczanie_systemu();
              aktualneDaneLampaPrzod = 0;
              flagaDzien = 0;
          break;
 
          case wylaczKierunki: //WYLACZ AWARYJNE / WYLACZ KIERUNKI
            clearLed12();
            aktualneZadanieKierunki = 12;
            if(flagaDzien ==3) aktualneDaneLampaPrzod = 3;
          break;

          case wylaczLampaPrzod:  // WYLACZ SWIATLA DZIENNE - LAMPA PRZOD
            clearLed12();
            aktualneDaneLampaPrzod = 13;  
            flagaDzien = 0; 
          break;     
        }      

    }

// ################ AKTUALNY STAN LAMP #########################
  if(aktualneZadanieKierunki == 2) swiatla_awaryjne();
  if(aktualneZadanieKierunki == 6) kierunkowskazy(6);
  if(aktualneZadanieKierunki == 9) kierunkowskazy(9);    
  if(aktualneDaneLampaPrzod == 3) swiatlo_dzien_przod();
}

// ################ FUNKCJE REALIZUJACE STEROWANIE SWIATLAMI ###################

/// ############### KIERUNKOWSKAZY ################################
void swiatla_awaryjne() // OBYDWA KIERUNKOWSKAZY
{
  for( int i = ledCount-1; i >= 0; i--){  
     strip1.setPixelColor(i, strip1.Color(255, 215, 0));
     strip2.setPixelColor(i, strip2.Color(255, 215, 0));
     delay(opoznienieZmianyMigacza);
   
       if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
        strip1.setPixelColor(i+dlugoscMigacza, strip1.Color(0, 0, 0));
        strip2.setPixelColor(i+dlugoscMigacza, strip2.Color(0, 0, 0));
       }
     strip1.show();
     strip2.show();
     }
        for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
        strip1.setPixelColor(k, strip1.Color(0, 0, 0));
        strip2.setPixelColor(k, strip2.Color(0, 0, 0));
        strip1.show();
        strip2.show();
        delay(opoznienieZmianyMigacza);   
        }
}

void kierunkowskazy(int ktory){ // JEDEN KIERUNKOWSKAZ, PRZYJMUJE ZMIENNA WSKAZUJACA KTORY - 6 -LEWY, 9 -PRAWY
    if(ktory == 6){ //lewy
      clearLed1();
       for( int i = ledCount-1; i >= 0; i--){  
         strip1.setPixelColor(i, strip1.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza);
         if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
                strip1.setPixelColor(i+dlugoscMigacza, strip1.Color(0, 0, 0));
         }
          strip1.show();
          }
       for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip1.setPixelColor(k, strip1.Color(0, 0, 0));
           strip1.show();
           delay(opoznienieZmianyMigacza);   
        }
  }
    else if(ktory == 9){ //prawy
       for( int i = ledCount-1; i >= 0; i--){  
         strip2.setPixelColor(i, strip2.Color(255, 215, 0));
         delay(opoznienieZmianyMigacza);
         if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
                strip2.setPixelColor(i+dlugoscMigacza, strip1.Color(0, 0, 0));
         }
          strip2.show();
          }
       for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
           strip2.setPixelColor(k, strip2.Color(0, 0, 0));
           strip2.show();
           delay(opoznienieZmianyMigacza);   
        }
  }
}

//################## LAMPA PRZOD FUNKCJE ################################
  
void swiatlo_dzien_przod(){
  for (int i = 0; i < int(ledCount/3); i++) {
    strip1.setPixelColor(i, strip1.Color(255, 255, 255));
    strip2.setPixelColor(i, strip2.Color(255, 255, 255));
  }
  strip1.show();
  strip2.show();
}

void inicjalizacja_systemu(){
  //// PASEK RUSZA W PRAWO
  for( int i = 0; i < ledCount; i++){  
     strip1.setPixelColor(i, strip1.Color(255, 255, 255));
     strip2.setPixelColor(i, strip2.Color(255, 255, 255));
     delay(opoznienieZmianyMigacza);
   
       if( i >= dlugoscMigacza){ //gaszenie paska od poczatku
        strip1.setPixelColor(i-dlugoscMigacza, strip1.Color(0, 0, 0));
        strip2.setPixelColor(i-dlugoscMigacza, strip2.Color(0, 0, 0));
       }
     strip1.show();
     strip2.show();
     } /// MA NIE ZNIKAC CALKIEM DO SRODKA, JAKBY MIAL ZNIKNAC TO ODKOMENTOWAC TO PODSPODEM
//        for(int k = (ledCount-dlugoscMigacza); k <= ledCount ; k++){  //gaszenie koncowki paska (znikanie)
//        strip1.setPixelColor(k, strip1.Color(0, 0, 0));
//        strip2.setPixelColor(k, strip2.Color(0, 0, 0));
//        strip1.show();
//        strip2.show();
//        delay(opoznienieZmianyMigacza);   
//        }

  for( int i = ledCount-(dlugoscMigacza-1); i >= 0; i--){  // pasek rusza w lewo
     strip1.setPixelColor(i, strip1.Color(255, 255, 255));
     strip2.setPixelColor(i, strip2.Color(255, 255, 255));
     delay(opoznienieZmianyMigacza);

     if( i< ledCount-dlugoscMigacza){ 
        strip1.setPixelColor(i+dlugoscMigacza+2, strip1.Color(0, 0, 0));
        strip2.setPixelColor(i+dlugoscMigacza+2, strip2.Color(0, 0, 0));
       }
     strip1.show();
     strip2.show();
     } // WERSJA BEZ AUTOMATYCZNYCH SWIATEL DZIENNYCH USUNAC FOR PONIZEJ ZEBY ZOSTALY DZIENNE
            for(int k = dlugoscMigacza+1; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
                strip1.setPixelColor(k, strip1.Color(0, 0, 0));
                strip2.setPixelColor(k, strip2.Color(0, 0, 0));
                strip1.show();
                strip2.show();
                delay(opoznienieZmianyMigacza);   
        }
     
}

void wylaczanie_systemu(){
     for( int i = (0+dlugoscMigacza); i < ledCount; i++){  // pasek rusza w prawo
         strip1.setPixelColor(i, strip1.Color(255, 255, 255));
         strip2.setPixelColor(i, strip2.Color(255, 255, 255));
         delay(opoznienieZmianyMigacza);
       
             if( i >= dlugoscMigacza){ //gaszenie paska od poczatku
              strip1.setPixelColor(i-dlugoscMigacza, strip1.Color(0, 0, 0));
              strip2.setPixelColor(i-dlugoscMigacza, strip2.Color(0, 0, 0));
             }
         strip1.show();
         strip2.show();
     }
     
     for( int i = ledCount-dlugoscMigacza; i >= 0; i--){  // pasek rusza w lewo
           strip1.setPixelColor(i, strip1.Color(255, 255, 255));
           strip2.setPixelColor(i, strip2.Color(255, 255, 255));
           delay(opoznienieZmianyMigacza);
         
               if( i< ledCount-dlugoscMigacza){ //gaszenie paska od poczatku
                  strip1.setPixelColor(i+dlugoscMigacza, strip1.Color(0, 0, 0));
                  strip2.setPixelColor(i+dlugoscMigacza, strip2.Color(0, 0, 0));
               }
           strip1.show();
           strip2.show();
           }
              for(int k = dlugoscMigacza; k >= 0 ; k--){  //gaszenie koncowki paska (znikanie)
                    strip1.setPixelColor(k, strip1.Color(0, 0, 0));
                    strip2.setPixelColor(k, strip2.Color(0, 0, 0));
                    strip1.show();
                    strip2.show();
                    delay(opoznienieZmianyMigacza);   
              }
}


//################## LAMPA TYL FUNKCJE ################################

// Funkcja Macka
void testSwiatel() {
  for (int i = 0; i < ledCount; i++) {
    strip1.setPixelColor(i, strip1.Color(255, 215, 0));
    strip2.setPixelColor(i, strip2.Color(127, 105, 0));

    // delay(10);
  }
  strip1.show();
  strip2.show();
}

//######################## FUNKCJE CZYSZCZENIA PASKOW ############################
void clearLed1() {
  for (int i = 0; i < ledCount; i++) {
    strip1.setPixelColor(i, 0, 0, 0);
  }
  strip1.show();  
}

void clearLed2() {
  for (int i = 0; i < ledCount; i++) {
    strip2.setPixelColor(i, 0, 0, 0);
  }
  strip2.show();  
}

void clearLed12() {
  for (int i = 0; i < ledCount; i++) {
    strip1.setPixelColor(i, 0, 0, 0);
    strip2.setPixelColor(i, 0, 0, 0);
    
  } 
  strip1.show();
  strip2.show();
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
