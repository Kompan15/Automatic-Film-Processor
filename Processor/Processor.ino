#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
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
signed int stanSwitch = 0;
//zmienna czasowa.
signed int czas;
//Deklaracje funkcji, inaczej tablica wskaznikow nie zadziala, kompilator musi wiedziec wczesniej.
void Wywolywanie();
void Plukanie();
void Utrwalanie();
void Zmiekczanie();
void Uzytkownika();
//deklaracja tablicy programów
String programy[] = {"Wywolywanie", "Plukanie", "Utrwalanie", "Zmiekczanie", "Uzytkownika"}; //tablica Stringów, słowo to jeden obiekt
void (*programy_wskazniki[])(void) = {Wywolywanie, Plukanie, Utrwalanie, Zmiekczanie, Uzytkownika}; //tablica wskaznikow do funkcji;
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
  delay(200); //zapobiega wywołaniu programu.
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
    lcd.print(programy[stanSwitch]); 
    lcd.setCursor(0,1); 
    lcd.print("<<     OK     >>"); delay(140);
  }while(!StanPrzycisku_2); //dopóki stan przycisku nie jest HIGH
}
void Init(){
  int StanPrzycisku_2 = 0; 
  bool znacznik_przerwania = 1;
  unsigned long time1;
  String Stme; //sekundy w stringu.
  lcd.clear();
  time1 = millis();
  while((millis()-time1)<2000){ //pętla Menu głównego.
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    Stme = String((millis()-time1)/1000);
    lcd.setCursor(0,0);
    lcd.print(programy[stanSwitch] + " za:" + Stme); 
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("||  Przerwij  ||");
    if (StanPrzycisku_2) {
      znacznik_przerwania = 0;
      break;
    }
  }
  if(znacznik_przerwania) programy_wskazniki[stanSwitch]();
}
void Wywolywanie(){
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0; 
  int StanPrzycisku_3 = 0;
  bool znacznik_przerwania = 1;
  bool znacznik_zatwierdzenia =0; 
  bool K1 = 0,K2 = 1; //inicjalnie: false
  unsigned int PWM = 40;
  unsigned long WTime = 0, time1, time2; //lokalny czas funkcji wywolanie.
  String SPWM, Stme; //PWM,Sekundy w string.
  lcd.clear(); //czysc ekran przed wejsciem do petli, inaczej sa artefakty.
  while(!StanPrzycisku_2){ //pętla przyjmujaca czas wykonywania wywolywania
    //przyciski, bez nich nie wyjdziesz z petli, nie zmienisz czasu.
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    //sprawdzamy stany przycisków  dekrementujemy bądź inkrementujemy.
    if (StanPrzycisku_1 && WTime <16000) { 
      WTime = WTime + 1000; 
      delay(100); 
      lcd.clear();
    }          
    
    if (StanPrzycisku_3 && WTime != 0) { 
      WTime = WTime - 1000; 
      delay(100); 
      lcd.clear();
    }
    
    Stme = String((WTime)/1000);
    /*wyswietl panel*/
    lcd.setCursor(0,0);
    lcd.print("Czas : " + Stme + "[min]"); 
    lcd.setCursor(0,1); 
    lcd.print("<-     OK     +>"); delay(140);
    //breaker pętli odczytu czasu
    if (StanPrzycisku_2) {
      znacznik_zatwierdzenia = 1; //znacznik zatwierdzenia to nie znacznik przerwania.
      break;
    }   
  }/********************************************************************/
  delay(100); //program czeka 100 ms aby nie złapać stanu przycisku.
  /********************************************************************/
  //odebranie czasu do zmiennej, funkcja zwraca naliczone milisekundy od momentu uruchomienia ukladu
  time1 = millis();//czas wykorzystywany do obliczania czasu trwania programu.
  time2 = millis();//czas wykorzystywany do obliczania czasu trwania obrotu
  while(((millis()-time1) < WTime*60) && znacznik_zatwierdzenia){ //pętla WYKONANIA FUNKCJI WYWOLANIE przez okreslony czas
    
    analogWrite(Enable_B, PWM); //uruchom silnik.
    //czytaj stany przycisków
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    SPWM = String(PWM); //czytaj od tyłu (prawej) Konwersja na string wartosc PWM i przypisanie do SPWM
    Stme = String((millis()-time1)/1000);
    
    //breaker funkcji, można do tego zastosować dowolny przycisk
    if ((millis()-time2) >= 4900) {
      //inwersja obrotów
      K1 = !K1;
      K2 = !K2;
      digitalWrite(Wejscie_Sterownika_3, K1);
      digitalWrite(Wejscie_Sterownika_4, K2);              
      time2 = millis();
    }
    if (StanPrzycisku_3 && PWM != 0) { PWM--; delay(10);}                                        
    if (StanPrzycisku_1 && PWM<=254) { PWM++; delay(10);}
    
    if (StanPrzycisku_2 == HIGH) { znacznik_przerwania = 0; break;}
    delay(100); //bez opoznienia nie ma wyswietlania bo tekst nawet nie zdazy sie pojawic na ekranie.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wywoluje " + Stme + "s" + String(PWM)); 
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("<-  Przerwij  +>");
    
  }analogWrite(Enable_B,PWM=0); //zerujemy PWM dla bezpieczenstwa.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zakonczono"); 
  lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
  lcd.print("Wywolywanie");
  delay(1000);
}
/********************************************************************/
void Plukanie(){
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0; 
  int StanPrzycisku_3 = 0;
  bool znacznik_przerwania = 1;
  bool znacznik_zatwierdzenia =0; 
  bool K1 = 0,K2 = 1; //inicjalnie: false
  unsigned int PWM = 40;
  unsigned long WTime = 0, time1, time2; //lokalny czas funkcji wywolanie.
  String SPWM, Stme; //PWM,Sekundy w string.
  lcd.clear(); //czysc ekran przed wejsciem do petli, inaczej sa artefakty.
  while(!StanPrzycisku_2){ //pętla przyjmujaca czas wykonywania wywolywania
    //przyciski, bez nich nie wyjdziesz z petli, nie zmienisz czasu.
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    //sprawdzamy stany przycisków  dekrementujemy bądź inkrementujemy.
    if (StanPrzycisku_1 && WTime <16000) { 
      WTime = WTime + 1000; 
      delay(100); 
      lcd.clear();
    }          
    
    if (StanPrzycisku_3 && WTime != 0) { 
      WTime = WTime - 1000; 
      delay(100); 
      lcd.clear();
    }
    
    Stme = String((WTime)/1000);
    /*wyswietl panel*/
    lcd.setCursor(0,0);
    lcd.print("Czas : " + Stme + "[min]"); 
    lcd.setCursor(0,1); 
    lcd.print("<-     OK     +>"); delay(140);
    //breaker pętli odczytu czasu
    if (StanPrzycisku_2) {
      znacznik_zatwierdzenia = 1; //znacznik zatwierdzenia to nie znacznik przerwania.
      break;
    }   
  }/********************************************************************/
  delay(100); //program czeka 100 ms aby nie złapać stanu przycisku.
  /********************************************************************/
  //odebranie czasu do zmiennej, funkcja zwraca naliczone milisekundy od momentu uruchomienia ukladu
  time1 = millis();//czas wykorzystywany do obliczania czasu trwania programu.
  time2 = millis();//czas wykorzystywany do obliczania czasu trwania obrotu
  while(((millis()-time1) < WTime*60) && znacznik_zatwierdzenia){ //pętla WYKONANIA FUNKCJI WYWOLANIE przez okreslony czas
    
    analogWrite(Enable_B, PWM); //uruchom silnik.
    //czytaj stany przycisków
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    SPWM = String(PWM); //czytaj od tyłu (prawej) Konwersja na string wartosc PWM i przypisanie do SPWM
    Stme = String((millis()-time1)/1000);
    
    //breaker funkcji, można do tego zastosować dowolny przycisk
    if ((millis()-time2) >= 4900) {
      //inwersja obrotów
      K1 = !K1;
      K2 = !K2;
      digitalWrite(Wejscie_Sterownika_3, K1);
      digitalWrite(Wejscie_Sterownika_4, K2);              
      time2 = millis();
    }
    if (StanPrzycisku_3 && PWM != 0) { PWM--; delay(10);}                                        
    if (StanPrzycisku_1 && PWM<=254) { PWM++; delay(10);}
    
    if (StanPrzycisku_2 == HIGH) { znacznik_przerwania = 0; break;}
    delay(100); //bez opoznienia nie ma wyswietlania bo tekst nawet nie zdazy sie pojawic na ekranie.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Plucze " + Stme + "s" + String(PWM)); 
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("<-  Przerwij  +>");
    
  }analogWrite(Enable_B,PWM=0); //zerujemy PWM dla bezpieczenstwa.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zakonczono"); 
  lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
  lcd.print("Wywolywanie");
  delay(1000);
}
void Utrwalanie(){
}
void Zmiekczanie(){
}
void Uzytkownika(){
  lcd.clear();
  unsigned int PWM = 0;
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0; 
  int StanPrzycisku_3 = 0;
  /*Uruchom Silnik*/
  digitalWrite(Wejscie_Sterownika_3, LOW); 
  digitalWrite(Wejscie_Sterownika_4, HIGH);
  //w tej petli stopem jest srodkowy przycisk.
  while(!StanPrzycisku_2){
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    analogWrite(Enable_B,PWM);
    lcd.setCursor(0,0);
    if (StanPrzycisku_3 && PWM != 0) { PWM--; delay(100); lcd.clear();}                                        
    if (StanPrzycisku_1 && PWM<=254) { PWM++; delay(100); lcd.clear();}
    lcd.print("PWM:" + String(PWM));
    lcd.setCursor(0,1);
    lcd.print("<-  Przerwij  +>");
  } analogWrite(Enable_B,PWM=0);
}