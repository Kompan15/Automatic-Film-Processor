#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
using namespace std;
//stałe obslugi sterownika silnika
const int Enable_B = 3;
const int Wejscie_Sterownika_3 = 5;
const int Wejscie_Sterownika_4 = 4;
//stałe przyciskow
const int Przycisk_1 = 8;
const int Przycisk_2 = 9;
const int Przycisk_3 = 10;
unsigned int stanSwitch = 0; //unsigned, bo nie przyjmuje wartości ujemnych
//zmienna czasowa.
unsigned int czas;
//Deklaracje funkcji, inaczej tablica wskaznikow nie zadziala, kompilator musi wiedziec wczesniej.
void Wywolywanie();
void Przerywanie();
void Utrwalanie();
void Plukanie();
void Uzytkownika();
void Predefined_D74();
/*deklaracja tablicy programów - jeden string to jeden obiekt, planowałem zrobić to na wektorach, ale nie są wskazane
z tak małą ilością pamięci wewnętrznej*/
String programy[] = {"Wywolywanie", "Przerywanie", "Utrwalanie", "Plukanie", "Predefined D-74"}; 
//definicja tablicy wskaznikow do programow.
void (*programy_wskazniki[])(void) = {Wywolywanie, Przerywanie, Utrwalanie, Plukanie, Predefined_D74};
/*ważna informacja: chcesz dodać nową funkcję, dodajesz ją na tym samym indeksie w obydwóch powyzszych tablicach!*/
void setup()
{
  /*Deklaracja pinów przycisków w charakterze INPUT_PULLUP*/
  pinMode(Przycisk_1, INPUT_PULLUP);
  pinMode(Przycisk_2, INPUT_PULLUP);
  pinMode(Przycisk_3, INPUT_PULLUP);
 //deklaracja pinów kolejno: EnableB (PWM), i odpowiadających za kierunek krecenia.
  pinMode(Enable_B, OUTPUT);
  pinMode(Wejscie_Sterownika_3, OUTPUT);
  pinMode(Wejscie_Sterownika_4, OUTPUT);
  //upewnij się że silnik nie działa.
  digitalWrite(Wejscie_Sterownika_3, LOW);
  digitalWrite(Wejscie_Sterownika_4, LOW);
  lcd.begin(16,2); //defincja 16 segmentów w dwóch rzędach.
  Serial.begin(9600); //rozpocznij komunikację na porcie szeregowym (9600Hz)
}
void loop() /***********************************************************************************************pętla główna*/
{
  Menu(); //menu generuje ekran powitalny wraz z menu 'karuzelowym'.
  Init(); 
}
/**********************************************************************************************************koniec pętli głównej*/
/**********************************************************************************************************Menu główne*/
void Menu(){
  int StanPrzycisku_1 = 1;
  int StanPrzycisku_2 = 1;
  int StanPrzycisku_3 = 1;
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Wybierz");
  lcd.setCursor(4,1);
  lcd.print("Program");
  
  delay(1800); //zapobiega wywołaniu programu (ms)
  do{
    //czytaj stany przycisków a następnie przypisz je do zmiennych.
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    
    //zwiększaj/zmniejszaj indeks tablic.
    if (!StanPrzycisku_1) {stanSwitch--;}
    if (!StanPrzycisku_3) {stanSwitch++;}
    //warunek kręcący.
    if (stanSwitch == 6){stanSwitch = 0;}
    if (stanSwitch == -1){stanSwitch = 5;}
    
    lcd.clear();
    lcd.setCursor((16- programy[stanSwitch].length())/2,0);
    lcd.print(programy[stanSwitch]);
    lcd.setCursor(0,1);
    lcd.print("<<     OK     >>"); delay(140);
  }while(StanPrzycisku_2); //dopóki stan przycisku jest HIGH
}
/**********************************************************************************************************koniec menu głównego*/
/**********************************************************************************************************funkcja inicjalizująca*/
void Init(){
  int StanPrzycisku_2 = 0; //inicjalizacja wartości zmiennej przechowującej stan przycisku na False(0)
  bool znacznik_przerwania = 0; 
  unsigned long czas_na_przerwanie;
  String Stme; //czas w stringu.
  lcd.clear();
  czas_na_przerwanie = millis();
  while((millis()-czas_na_przerwanie)<2000){ //pętla Menu głównego.
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    Stme = String((millis() - czas_na_przerwanie)/1000);
    lcd.setCursor(0,0);
    lcd.print(programy[stanSwitch] + " za:" + Stme);
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("||  Przerwij  ||");
    if (!StanPrzycisku_2){
      znacznik_przerwania = 1;
      break;
    }
  } /*czytaj: jezeli nie przerwano - kontynuuj*/
   if(!znacznik_przerwania) programy_wskazniki[stanSwitch](); //argument funkcji programy to string z nazwa wybranego programu.
}
/**********************************************************************************************************koniec inicjalizatora*/
/**********************************************************************************************************Program wykonawczy*/
//następująca funkcja posiada dwa argumenty domniemane.
//string zostaje przejety tylko i wylacznie po to by wyswietlic dobry komunikat na ekran.
void Program(String funkcja, unsigned long BreakTime = 0, unsigned long WTime = 0){ 
//jedynki, PULLUP!  
  int StanPrzycisku_1 = 1;
  int StanPrzycisku_2 = 1;
  int StanPrzycisku_3 = 1;
  String SPWM, Stme, StmeB, Secs, Mins; //PWM,Sekundy w string.
  unsigned int PWM = 150;
  unsigned long czas_ref_trwania_programu, czas_ref_trwania_kierunku_obrotu, minuty = 0; //zmienne czasowe.
  unsigned long konwersja;
  bool K1 = 0,K2 = 1; //inicjalnie: false
  if (!BreakTime && !WTime){ //warunek sprawdzający czy argumenty zostały domniemane czy nie.
  unsigned int znacznik_opcji = 0; //liczymy do 2 (3 - 0,1,2)
  bool znacznik_zatwierdzenia = 0;
  lcd.clear(); //czysc ekran przed wejsciem do petli, inaczej wystapia artefakty.
  while(znacznik_opcji != 2){ //pętla przyjmujaca czas wykonywania wywolywania
    //przyciski, bez nich nie wyjdziesz z petli.
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    /******************************************************************************************************/      
    switch(znacznik_opcji){ //switch case
    case 0:
      if (!StanPrzycisku_1 && WTime <960) { //jeżeli obydwie wartości są True.
        WTime = WTime + 60; //1000 to jedna sec
        delay(100);
        lcd.clear();
      }
      
      if (!StanPrzycisku_3 && WTime != 0) {
        WTime = WTime - 60;
        delay(100);
        lcd.clear();
      }
      
      Stme = String((WTime)/60);
      
      /* wyswietl panel */
      
      lcd.setCursor(0,0);
      lcd.print("Czas trw.prg:");
      lcd.setCursor(0,1);
      lcd.print("<- OK +> "+ Stme + "[min]"); delay(140);
      if (!StanPrzycisku_2){delay(140); znacznik_opcji = 1; delay(140);}break;
      /******************************************************************************************************/
    case 1:
      if (!StanPrzycisku_1 && BreakTime < 20) { //jeżeli obydwie wartości są True.
        BreakTime = BreakTime + 1;
        delay(100);
        lcd.clear();
      }
      
      if (!StanPrzycisku_3 && BreakTime != 0) {
        BreakTime = BreakTime - 1;
        delay(100);
        lcd.clear();
      }
      
      Stme = String((BreakTime));
      
      /* wyswietl panel */
      
      lcd.setCursor(0,0);
      lcd.print("Zm.kier.obr.co:");
      lcd.setCursor(0,1);
      lcd.print("<-  OK  +> " + Stme + "[s]"); delay(140);
      if (!StanPrzycisku_2){
        delay(140); 
        znacznik_opcji = 2;
        delay(140);}break;
      
    case 2:
      lcd.setCursor(4,0);
      lcd.print("funkcja");
      lcd.setCursor(0,1);
      lcd.print("<-   Wykonaj    +>"); delay(140);
      if (!StanPrzycisku_2){
        delay(140); 
        znacznik_opcji = 3; 
        delay(140);} break;
    }
    }
  }
  
  /********************************************************************/
  delay(100); //program czeka 100 ms aby nie 'złapać' stanu przycisku.
  /********************************************************************/
  
  //odebranie czasu do zmiennej, funkcja zwraca naliczone milisekundy od momentu uruchomienia ukladu
  czas_ref_trwania_programu = millis();//czas wykorzystywany do obliczania czasu trwania programu.
  czas_ref_trwania_kierunku_obrotu = millis();//czas wykorzystywany do obliczania czasu trwania obrotu
  analogWrite(Enable_B, PWM);
  digitalWrite(Wejscie_Sterownika_3, K1);
  digitalWrite(Wejscie_Sterownika_4, K2);
  while(((millis()-czas_ref_trwania_programu) < WTime*1000)){ //pętla WYKONANIA FUNKCJI przez okreslony czas
    
    //czytaj stany przycisków
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    SPWM = String(PWM); //czytaj od tyłu (prawej) Konwersja na string wartosc PWM i przypisanie do SPWM
    konwersja = (abs(((millis() - czas_ref_trwania_programu) / 1000) - 60 * minuty));
    if (minuty < 10){
      Mins = String("0") + String(minuty) + String(":");
    }else Mins = String(minuty) + String(":");
    
    if (konwersja < 10){
      Secs = String("0") + String(konwersja);
    }else Secs = String(konwersja);
    Stme = Mins+Secs;
    if (konwersja >= 60){minuty++;}
    
    //Wykonuj co okreslony czas
    if ((millis()-czas_ref_trwania_kierunku_obrotu) >= BreakTime*1000) {
      analogWrite(Enable_B, PWM); //uruchom silnik.
      //inwersja zmiennych
      K1 = !K1;
      K2 = !K2;
      //faktyczna inwersja na wyjściach
      digitalWrite(Wejscie_Sterownika_3, K1);
      digitalWrite(Wejscie_Sterownika_4, K2);
      czas_ref_trwania_kierunku_obrotu = millis();
    }
    //kontrola szybkości
    if (!StanPrzycisku_3 && PWM != 0)   { PWM--; delay(10); }
    if (!StanPrzycisku_1 && PWM <= 254) { PWM++; delay(10); }
    
    if (!StanPrzycisku_2) {break;}
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(funkcja + Stme);
    lcd.leftToRight();
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("<-  Przerwij  +>");
    
  }analogWrite(Enable_B,PWM=0); //zerujemy PWM dla bezpieczeństwa.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zakonczono prog.");
  lcd.setCursor((16 - funkcja.length())/2,1); //drugi wiersz, pierwsza kolumna.
  lcd.print(funkcja);
  delay(2000);
}
//definicje programów, wcześniej były tylko deklaracje.
void Wywolywanie(){
    Program("Wywolywanie");
}
void Przerywanie(){
    Program("Przerywanie");
}
void Utrwalanie(){
    Program("Utrwalanie");
}
void Plukanie(){
    Program("Plukanie");
}
void Uzytkownika(){
    Program("Uzytkownika");
}
void Czekaj(){
  int StanPrzycisku_2 = 1;
  do{
    //czytaj stany przycisków a następnie przypisz je do zmiennych.
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Czekam...");
    lcd.setCursor(0,1);
    lcd.print("<<   Dalej    >>"); delay(140);
  }while(StanPrzycisku_2); //dopóki stan przycisku jest HIGH
  delay(100);
}
//argumenty podawać w kolejności: Czas po którym zostanie zmieniony kierunek obrotów, czas trwania programu.
void Predefined_D74(){
  Program("Wywolywanie", 5, 780);
  Czekaj();
  Program("Przerywanie", 5, 30);
  Czekaj();
  Program("Utrwalanie", 5, 420);
}
/******************************************************************************************************************************************/
