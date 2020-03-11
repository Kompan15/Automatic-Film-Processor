#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
using namespace std;
//stale obslugi sterownika silnika
const int Enable_B = 3;
const int Wejscie_Sterownika_3 = 5;
const int Wejscie_Sterownika_4 = 4;
//stałe przyciskow
const int Przycisk_1 = 8;
const int Przycisk_2 = 9;
const int Przycisk_3 = 10;
signed int stanSwitch = 0; //signed, bo nie przyjmuje wartości ujemnych
//zmienna czasowa.
signed int czas;
//Deklaracje funkcji, inaczej tablica wskaznikow nie zadziala, kompilator musi wiedziec wczesniej.
//deklaracja tablicy programów
String programy[] = {"Wywolywanie", "Przerywanie", "Utrwalanie", "Plukanie", "Uzytkownika"}; //tablica Stringów, słowo to jeden obiekt
//definicja tablicy wskaznikow do programow.
void setup()
{
  /*Deklaracja przycisków*/
  pinMode(Przycisk_1, INPUT_PULLUP);
  pinMode(Przycisk_2, INPUT_PULLUP);
  pinMode(Przycisk_3, INPUT_PULLUP);
  //Ustawienie charakteru Pinow.
  pinMode(Enable_B, OUTPUT);
  pinMode(Wejscie_Sterownika_3, OUTPUT);
  pinMode(Wejscie_Sterownika_4, OUTPUT);
  //wylacz silnik.
  digitalWrite(Wejscie_Sterownika_3, LOW);
  digitalWrite(Wejscie_Sterownika_4, LOW);
  lcd.begin(16,2); //defincja 16 segmentów w dwóch rzędach.
  Serial.begin(9600); //rozpocznij komunikację na porcie szeregowym
}
void loop() //pętla główna.
{
  Menu();
  Init();
}
void Menu(){
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0;
  int StanPrzycisku_3 = 0;
  lcd.clear();
  lcd.print("Wybierz");
  lcd.setCursor(0,1);
  lcd.print("Program");
  
  delay(1500); //zapobiega wywołaniu programu.
  do{
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    
    if (StanPrzycisku_1) {stanSwitch--;}
    if (StanPrzycisku_3) {stanSwitch++;}
    //warunek kręcący.
    if (stanSwitch == 5){stanSwitch = 0;}
    if (stanSwitch == -1){stanSwitch = 4;}
    
    lcd.clear();
    lcd.setCursor((16- programy[stanSwitch].length())/2,0);
    lcd.print(programy[stanSwitch]);
    lcd.setCursor(0,1);
    lcd.print("<<     OK     >>"); delay(140);
  }while(!StanPrzycisku_2); //dopóki stan przycisku nie jest HIGH
}
void Init(){
  int StanPrzycisku_2 = 0; //inizjalizacja wartości zmiennej przechowującej stan przycisku na False.
  bool znacznik_przerwania = 0; 
  unsigned long time1;
  String Stme; //sekundy w stringu.
  lcd.clear();
  time1 = millis();
  while((millis()-time1)<2000){ //pętla Menu głównego.
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    Stme = String((millis() - time1)/1000);
    lcd.setCursor(0,0);
    lcd.print(programy[stanSwitch] + " za:" + Stme);
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("||  Przerwij  ||");
    if (StanPrzycisku_2) {
      znacznik_przerwania = 1;
      break;
    }
  } /*czytaj: jezeli nie przerwano - kontynuuj*/
  if(!znacznik_przerwania) Program(programy[stanSwitch]); //argument funkcji programy to string z nazwa wybranego programu.
}
void Program(String funkcja){ //string zostaje przejety tylko i wylacznie po to by wyswietlic dobry komunikat na ekran.
  /****************************************************************/
  /*zerujemy stany*/
  
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0;
  int StanPrzycisku_3 = 0;
  /****************************************************************/
  unsigned int znacznik_opcji = 0; //liczymy do 2 (3 - 0,1,2)
  // bool znacznik_przerwania = 0; //w tym podprogramie nie można przerywać - ograniczenie sprzętowe.
  bool znacznik_zatwierdzenia =0;
  bool K1 = 0,K2 = 1; //inicjalnie: false
  unsigned int PWM = 40;
  unsigned long BreakTime = 0, WTime = 0, time1, time2; //lokalny czas funkcji wywolanie.
  String SPWM, Stme; //PWM,Sekundy w string.
  lcd.clear(); //czysc ekran przed wejsciem do petli, inaczej wystapia artefakty.
  while(znacznik_opcji != 2){ //pętla przyjmujaca czas wykonywania wywolywania
    //przyciski, bez nich nie wyjdziesz z petli, nie zmienisz czasu.
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    /******************************************************************************************************/      
    switch(znacznik_opcji){ //switch case
    case 0:
      if (StanPrzycisku_1 && WTime <16000) { //jeżeli obydwie wartości są True.
        WTime = WTime + 1000; //1000 to jedna sec
        delay(100);
        lcd.clear();
      }
      
      if (StanPrzycisku_3 && WTime != 0) {
        WTime = WTime - 1000;
        delay(100);
        lcd.clear();
      }
      
      Stme = String((WTime)/1000);
      
      /* wyswietl panel */
      
      lcd.setCursor(0,0);
      lcd.print("Wykonuj : ");
      lcd.setCursor(0,1);
      lcd.print("<- OK +> "+ Stme + "[min]"); delay(140);
      if (StanPrzycisku_2){delay(140); znacznik_opcji = 1;delay(140);}break;
      /******************************************************************************************************/
    case 1:
      if (StanPrzycisku_1 && BreakTime <10000) { //jeżeli obydwie wartości są True.
        BreakTime = BreakTime + 1000; //1000 to jedna sec
        delay(100);
        lcd.clear();
      }
      
      if (StanPrzycisku_3 && BreakTime != 0) {
        BreakTime = BreakTime - 1000;
        delay(100);
        lcd.clear();
      }
      
      Stme = String((BreakTime)/1000);
      
      /* wyswietl panel */
      
      lcd.setCursor(0,0);
      lcd.print("Zm.kier.obr.co:");
      lcd.setCursor(0,1);
      lcd.print("<-  OK  +> " + Stme + "[s]"); delay(140);
      if (StanPrzycisku_2){
        delay(140); 
        znacznik_opcji = 2;
        delay(140);}break;
      
    case 2:
      lcd.setCursor(4,0);
      
      lcd.print("funkcja");
      lcd.setCursor(0,1);
      lcd.print("<-   Wykonaj    +>"); delay(140);
      if (StanPrzycisku_2){
        delay(140); 
        znacznik_opcji = 3; 
        delay(140);} break;
    }
  }
  
  
  
  //sprawdzamy stany przycisków  dekrementujemy bądź inkrementujemy.
  
  /********************************************************************/
  delay(100); //program czeka 100 ms aby nie 'złapać' stanu przycisku.
  /********************************************************************/
  //odebranie czasu do zmiennej, funkcja zwraca naliczone milisekundy od momentu uruchomienia ukladu
  time1 = millis();//czas wykorzystywany do obliczania czasu trwania programu.
  time2 = millis();//czas wykorzystywany do obliczania czasu trwania obrotu
  while(((millis()-time1) < WTime*60)){ //pętla WYKONANIA FUNKCJI WYWOLANIE przez okreslony czas
    
    analogWrite(Enable_B, PWM); //uruchom silnik.
    //czytaj stany przycisków
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    SPWM = String(PWM); //czytaj od tyłu (prawej) Konwersja na string wartosc PWM i przypisanie do SPWM
    Stme = String((millis() - time1) / 1000);
    
    //Wykonuj co okreslony czas
    if ((millis()-time2) >= BreakTime) {
      //inwersja obrotów
      K1 = !K1;
      K2 = !K2;
      digitalWrite(Wejscie_Sterownika_3, K1);
      digitalWrite(Wejscie_Sterownika_4, K2);
      time2 = millis();
    }
    if (StanPrzycisku_3 && PWM != 0)   { PWM--; delay(10); }
    if (StanPrzycisku_1 && PWM <= 254) { PWM++; delay(10); }
    
    if (StanPrzycisku_2) {break;}
    delay(100); //bez opoznienia nie ma wyswietlania bo tekst nawet nie zdazy sie pojawic na ekranie.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(funkcja +" "+ Stme + "s ");
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("<-  Przerwij  +>");
    
  }analogWrite(Enable_B,PWM=0); //zerujemy PWM dla bezpieczenstwa.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zakonczono");
  lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
  lcd.print(funkcja);
  delay(1000);
}
/********************************************************************/