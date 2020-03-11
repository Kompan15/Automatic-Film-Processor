/* www.learningbuz.com */
/*Impport following Libraries*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

using namespace std;

//stale obslugi sterownika silnika
const int Enable_B = 3;
const int Wejscie_Sterownika_3 = 2;
const int Wejscie_Sterownika_4 = 1;


//stale przyciskow
const int Przycisk_1 = 8;
const int Przycisk_2 = 9;
const int Przycisk_3 = 10;  
signed int stanSwitch = 0;

//deklaracja zmiennej czasowej

signed int czas;

//deklaracja tablicy programów
String programy[] = {"Wywolywanie", "Plukanie", "Utrwalanie", "Zmiekczanie", "Uzytkownika"}; //tablica Stringów, słowo to jeden obiekt

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
Serial.begin(9600);
}
void loop() //pętla główna.
{

menu();
inicjalizacja();

}

void menu(){
  
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0; 
  int StanPrzycisku_3 = 0;
  
  do{
    StanPrzycisku_1 = digitalRead(Przycisk_1);
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    StanPrzycisku_3 = digitalRead(Przycisk_3);
    
    if (StanPrzycisku_1 == HIGH) {stanSwitch--;}
    if (StanPrzycisku_3 == HIGH) {stanSwitch++;}
    if (stanSwitch == 5){stanSwitch = 0;}
    if (stanSwitch == -1){stanSwitch = 4;}
    
    lcd.clear();
    lcd.print(programy[stanSwitch]); 
    lcd.setCursor(0,1);  //Defining positon to write from second row,first column .
    lcd.print("<<     OK     >>"); delay(140);
    }while(!StanPrzycisku_2); //dopóki stan przycisku nie jest HIGH
}

void inicjalizacja(){
  
  int StanPrzycisku_1 = 0;
  int StanPrzycisku_2 = 0; 
  int StanPrzycisku_3 = 0;
  
  unsigned long time1;
  String Stme; //sekundy w stringu.
  lcd.clear();
  time1 = millis();
  while((millis()-time1)<5000){
    StanPrzycisku_2 = digitalRead(Przycisk_2);
    Stme = String((millis()-time1)/1000);
    lcd.setCursor(0,0);
    lcd.print(programy[stanSwitch] + " za:" + Stme); 
    lcd.setCursor(0,1); //drugi wiersz, pierwsza kolumna.
    lcd.print("    Przerwij");
    if (StanPrzycisku_2 == HIGH) {break;}
  }
  Uzytkownika();
 }

void Wywolywanie(){
  
  }
void Plukanie(){
  
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
  
    if (StanPrzycisku_1 == HIGH && PWM != 0) {PWM--; delay(100);}
    if (StanPrzycisku_3 == HIGH && PWM<=200) {PWM++; delay(100);}
    
    lcd.setCursor(0,0);
    lcd.print("Szybkosc:" + String(PWM)); 
    lcd.clear();
    }

 
  }
