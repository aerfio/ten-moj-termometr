#include <EEPROM.h>
#include "LiquidCrystal.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>


LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Informacja o pod³¹czeniu nowego wyœwietlacza
OneWire oneWire(A0); //Pod³¹czenie do A0
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
int licznikZapisan = 0;
int seconds, hours, minutes;
bool endEepromSaving = false;
bool tempBool = true;

void setup() {
	Serial.begin(9600);
	sensors.begin(); //Inicjalizacja czujnikow
	lcd.createChar(0, stopnie);
	pinMode(9, OUTPUT); //Dioda jako wyjœcie
	pinMode(12, OUTPUT);
	pinMode(8, INPUT_PULLUP); //Przycisk jako wejœcie
	//eeprom
	zeroEEPROM();
	//czas
	StartTime = millis();
	//ta instrukcja musi byc tutaj zeby "wszystko sie zgadzalo"
	lcd.begin(16, 2); //Deklaracja typu

}


void loop() {
	sensors.requestTemperatures(); //Pobranie temperatury czujnika
	//Serial.print("Aktualna temperatura: ");
	//Serial.println(sensors.getTempCByIndex(0));  //Wyswietlenie informacji


	analogWrite(11, jasnosc); //Generuj sygna³ PWM o zadanym wype³nieniu   
	zmianaJasnosciNaPrzycisku();
	mojLcdTemper();
	//mojLcdJasn();
	mojLcdCounter();
	//Serial.print("Czas:");
	//Serial.println((float)(millis() - StartTime)/1000);
	counter++;
	delay(500);


	if (endEepromSaving == false) {
		zapiszTempDoEEPROM();
		//digitalWrite(9, HIGH);
	}
	else {
		digitalWrite(12, HIGH);
	}

	/*Serial.println(zmianaCzasuNaFloat(seconds, minutes, hours));
	Serial.println(seconds);
	Serial.println(minutes);
	Serial.println(hours);
	Serial.println("lol");*/
}

void mojLcdTemper() {

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

	seconds = (int)((millis() - StartTime) / 1000);
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

	if (minutes % 60 < 10)
	{
		lcd.print("0");
		lcd.print(minutes % 60);
	}
	else {
		lcd.print(minutes % 60);
	}

	lcd.print(":");

	if (seconds % 60 < 10)
	{
		lcd.print("0");
		lcd.print(seconds % 60);
	}
	else {
		lcd.print(seconds % 60);
	}
	//Serial.println(seconds);
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


void zapiszTempDoEEPROM() {
	int temp;
	if (licznikZapisan<509) {//ostatnie 3 miejsca w komorkach na temp i na godzine wypelniam zerami, "bo tak"
		if (seconds % 170 == 0 ) {//24h po 60 minut po 60 sec rozdzielone na ~512 pomiarow daje 168 z hakiem, ale jezeli chce mierzyc 24h to musze tu dac cos wiecej, hence my number of choice is 170
			sensors.requestTemperatures(); //Pobranie temperatury czujnika
			float temperatura = sensors.getTempCByIndex(0);
			EEPROM.put(licznikZapisan, temperatura);
			EEPROM.put(licznikZapisan + 512, seconds);
			Serial.print("licznikZapisan:");
			Serial.print(licznikZapisan);
			Serial.print("\\temp");
			Serial.print(temperatura);
			Serial.print("\\czas:");
			Serial.println(seconds);
			licznikZapisan++;
		}
		
		EEPROM.get(licznikZapisan + 512 - 1, temp);
		/*Serial.print(temp);
		Serial.print(":");
		Serial.println(seconds);*/
		if (seconds % 170 == 1 && temp!=(seconds-1)) {//to kurewstwo wyzej czasem nie lapie, dlatego trzeba sprawdzic jedna liczbe dalej
			sensors.requestTemperatures(); //Pobranie temperatury czujnika
			float temperatura = sensors.getTempCByIndex(0);
			EEPROM.put(licznikZapisan, temperatura);
			EEPROM.put(licznikZapisan + 512, seconds);
			Serial.print("licznikZapisan:");
			Serial.print(licznikZapisan);
			Serial.print("\\temp");
			Serial.print(temperatura);
			Serial.print("\\czas:");
			Serial.println(seconds);
			licznikZapisan++;
		}
		
	}
	else {
		for (int i = 509; i < 512; i++)
		{
			EEPROM.put(i, 0);
			EEPROM.put(i + 512, 0);
		}
		endEepromSaving = true;	
	}
}



void zeroEEPROM() {
	for (int i = 0; i < EEPROM.length(); i++) {
		EEPROM.write(i, 0);
	}
}


//sprawdz czy zapiszTempDoEEPROM zapisuje dobre wartosci