#include <EEPROM.h>
#include "LiquidCrystal.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>


LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Informacja o pod³¹czeniu nowego wyœwietlacza
OneWire oneWire(A0); //Pod³¹czenie do A5
DallasTemperature sensors(&oneWire); //Przekazania informacji do biblioteki
byte stopnie[8] = {                //konfigurajca znaku stopnie
	0b00100,
	0b01010,
	0b10001,
	0b01010,
	0b00100,
	0b00000,
	0b00000,
	0b00000
};
int jasnosc = 20;
bool brightLCD = false;
unsigned long StartTime;
int counter = 0;

void setup() {
	Serial.begin(9600);
	sensors.begin(); //Inicjalizacja czujnikow
	lcd.createChar(0, stopnie);
	pinMode(9, OUTPUT); //Dioda jako wyjœcie
	pinMode(8, INPUT_PULLUP); //Przycisk jako wejœcie
	//eeprom
	Serial.print("EEPROM length:");
	Serial.println(EEPROM.length());

	StartTime = millis();
	Serial.println(StartTime);
}


void loop() {
	sensors.requestTemperatures(); //Pobranie temperatury czujnika
	Serial.print("Aktualna temperatura: ");
	Serial.println(sensors.getTempCByIndex(0));  //Wyswietlenie informacji
	

	analogWrite(11, jasnosc); //Generuj sygna³ PWM o zadanym wype³nieniu   
	zmianaJasnosciNaPrzycisku();
	mojLcdTemper();
	//mojLcdJasn();
	mojLcdCounter();
	//Serial.print("Czas:");
	//Serial.println((float)(millis() - StartTime)/1000);
	counter++;
	delay(500);
}

void mojLcdTemper() {
	lcd.begin(16, 2); //Deklaracja typu
	lcd.setCursor(0, 0); //Ustawienie kursora
	lcd.print("Temp:"); //Wyœwietlenie tekstu
	lcd.print(sensors.getTempCByIndex(0));
	lcd.print((char)0);
	lcd.print("C");
}
//void mojLcdJasn() {
//	lcd.setCursor(0, 1); //Ustawienie kursora
//	lcd.print("Jasnosc:");
//	lcd.print(jasnosc);
//	lcd.print("\/225");
//}

void mojLcdCounter() {
	lcd.setCursor(0, 1); //Ustawienie kursora
	int seconds, hours, minutes;
	seconds = (float)((millis() - StartTime) / 1000);
	minutes = seconds / 60;
	hours = minutes / 60;
	
	lcd.print("Time:");
	if (hours < 10) {
		lcd.print(0);
		lcd.print(hours);
	}
	else {
		lcd.print(hours);
	}

	lcd.print(":");

	if (minutes%60<10)
	{
		lcd.print("0");
		lcd.print(minutes % 60);
	}
	else {
		lcd.print(minutes % 60);
	}

	lcd.print(":");

	if (seconds % 60<10)
	{
		lcd.print("0");
		lcd.print(seconds % 60);
	}
	else {
		lcd.print(seconds % 60);
	}
	
}


void zmianaJasnosciNaPrzycisku() {

	if (digitalRead(8) == LOW) { //Jeœli przycisk wciœniêty
		digitalWrite(9, HIGH); //W³¹cz diodê

		if (brightLCD == false) {
			brightLCD = true;
			jasnosc = 200;
		}
		else {
			jasnosc = 5;
			brightLCD = false;
		}
	}
	else {
		digitalWrite(9, LOW);

	}
}

//
//void testEPROM() {
//	for (int i = 0; i <EEPROM.length(); i++)
//	{
//		EEPROM.write(i, i);
//		Serial.println(EEPROM.read(i));
//	}
//}
//1023 bajty, bo wypisuje wartoœci od 0 do 255

//float b = 37.78;
//int a = (int)(roundf(b * 10));
//cout << a;
//
//watchdog