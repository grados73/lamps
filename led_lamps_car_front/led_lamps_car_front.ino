/*
 * Author: Grad
 * Date: 16.09.2020r
 * ZADANIA: 
 * CO JESLI JEST W JEDNYM MOMENCIE STOP, COFANIA I KIERUNEK -> WYLACZAMY STOP?
 * CO JESLI JEST KIERUNEK ZWYKŁY I WCISNIEMY STOP 
 * CO JESLI JEST KIERUNEK Z STOP/COFANIE I WYLACZYMY STOP -> DODAC FLAGE OD KIERUNKU I JAK WYLACZAMY STOP TO WCHODZI DO FUNKCJI ZWYKLEGO KIERUNKU
 * 
 *
 */

#include <Adafruit_NeoPixel.h>

#define ledCount 30 //ilosc diod w pasku
#define ledStripPin1 10 // podlaczenie lewego paska przod
#define ledStripPin2 9 //podlaczenie prawego paska przod
#define ledStripPin1t 12 //podlaczenie lewego paska tyl
#define ledStripPin2t 11 //podlaczenie prawego paska tyl
#define Brightness 5 // max 255 - jasnosc
#define dlugoscMigacza 7  //dlugosc weza migacza
#define opoznienieZmianyMigacza 20 //czas w [ms] po jakim ma sie zmieniac stan diod przy kierunkowskazach

//Sygnaly sterujace
#define sstop 1  // swiatlo stopu
#define swiatlaDzienTyl 5 
#define swiatloDzienPrzod 3
#define swiatlaCofania 7
#define awaryjne 2 //swiatla awaryjne
#define kierunekPrawy 9
#define kierunekLewy 6
#define wlaczenieSystemu 4
#define wylaczanieSystemu 14
#define wylaczKierunki 12
#define wylaczStop 11
#define wylaczSwiatlaCofania 17
#define wylaczLampaPrzod 13
#define wylaczDzienTyl 15



Adafruit_NeoPixel strip1(ledCount, ledStripPin1, NEO_GRB + NEO_KHZ800); //przod lewy
Adafruit_NeoPixel strip2(ledCount, ledStripPin2, NEO_GRB + NEO_KHZ800); // przod prawy
Adafruit_NeoPixel strip1t(ledCount, ledStripPin1t, NEO_GRB + NEO_KHZ800); // tyl lewy
Adafruit_NeoPixel strip2t(ledCount, ledStripPin2t, NEO_GRB + NEO_KHZ800); //tyl prawy


String odebraneDane = ""; //Pusty ciąg odebranych danych
int aktualneZadanieKierunki = 0;
int aktualneDaneLampaTyl = 0;
int aktualneDaneLampaPrzod = 0;
int odebraneDaneInt = 0;
int flagaTylDzien = 0;
int flagaTylStop = 0;
int flagaTylCofanie = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //inicjalizacji transmisji szeregowej - ustawic predkosc na !!!115200!!!
  strip1.begin(); // INICJALIZACJA PASKOW
  strip2.begin();
  strip1.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  strip2.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  strip1t.begin();
  strip2t.begin();
  strip1t.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  strip2t.setBrightness(Brightness); //poziom intenyswnosci swiecenia
  clearLed12();
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
            swiatlo_stop();
            aktualneDaneLampaTyl = 1;
          break;

          case swiatloDzienPrzod:
            swiatlo_dzien_przod();  //SWIATLA DO JAZDY DZIENNEJ - LAMPA PRZOD
            aktualneDaneLampaPrzod = 3;   
          break;  

          case awaryjne: // SWIATLA AWARYJNE - OBA KIERUNKI SWIECA
            swiatla_awaryjne();
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH
            aktualneZadanieKierunki = 2;
          break;
          
          case kierunekLewy:  // KIERUNEK LEWY
            kierunkowskazy(6);
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH
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
            kierunkowskazy(9);
            aktualneDaneLampaPrzod = 0; //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH
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
              inicjalizacja_systemu();
 //             aktualneDaneLampaPrzod = 3; - AUTOMATYCZNE SWIATLA DZIENNE, W CELU WLACZENIA PO INICJALIZACJI ODKOMENTOWAC!
          break;

          case wylaczanieSystemu: // ANIMACJA WYLACZANIA SYSTEMU
              wylaczanie_systemu();
              aktualneDaneLampaPrzod = 0;
          break;
 
          case wylaczKierunki: //WYLACZ AWARYJNE / WYLACZ KIERUNKI
            clearLed12();
            aktualneZadanieKierunki = 12;
            if(flagaTylStop == 1) aktualneDaneLampaTyl= 1;
            if( flagaTylCofanie == 1) aktualneDaneLampaTyl= 7;
          break;

          case wylaczStop: // WYLACZANIE STOPU - LAMPA TYL
            clearLed12t();
            flagaTylStop = 0;
            aktualneDaneLampaTyl = 0;    //WERSJA Z WYLACZANIEM SWIATEL DZIENNYCH5
            
          break;

          case wylaczSwiatlaCofania: //  WYLACZ SWIATLA COFANIA - LAMPA TYL - ZOSTAJE POZYCJA
            clearLed12t();
            flagaTylCofanie = 0;
            if(flagaTylDzien == 0) aktualneDaneLampaTyl = 0;
            else aktualneDaneLampaTyl = 5; 
          break;

          case wylaczLampaPrzod:  // WYLACZ SWIATLA DZIENNE - LAMPA PRZOD
            clearLed12();
            aktualneDaneLampaPrzod = 13;   
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
  if(aktualneZadanieKierunki == 6) kierunkowskazy(6), kierunkowskazy_tyl(6);
  if(aktualneZadanieKierunki == 9) kierunkowskazy(9), kierunkowskazy_tyl(9);    
  if(aktualneDaneLampaTyl == 1) swiatlo_stop();
    else if(aktualneDaneLampaTyl == 5) swiatlo_dzien_tyl();
  if(aktualneDaneLampaTyl == 7) swiatlo_cofania(flagaTylDzien);
  if(aktualneDaneLampaPrzod == 3) swiatlo_dzien_przod();
  if( aktualneDaneLampaTyl == 91)kierunkowskazy_tyl_stop_cofanie(9);
  if( aktualneDaneLampaTyl == 61)kierunkowskazy_tyl_stop_cofanie(6);
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
