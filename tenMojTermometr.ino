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

int licznikZapisan = 0;
int seconds, hours, minutes;
bool endEepromSaving = false;
bool tempBool = true;
int wewLicznik=0;

void setup() {
	Serial.begin(9600);
	sensors.begin(); //Inicjalizacja czujnikow
	lcd.createChar(0, stopnie);
	pinMode(9, OUTPUT); //Dioda jako wyjœcie
	pinMode(12, OUTPUT);
	pinMode(8, INPUT_PULLUP); //Przycisk jako wejœcie
	//czas
	StartTime = millis();
	//ta instrukcja musi byc tutaj zeby "wszystko sie zgadzalo"
	lcd.begin(16, 2); //Deklaracja typu
	EEPROMtest();
}


void loop() {
	analogWrite(11, jasnosc); //Generuj sygna³ PWM o zadanym wype³nieniu   
	zmianaJasnosciNaPrzycisku();
	mojLcdTemper();
	mojLcdCounter();
	

	if (endEepromSaving == false) {
		zapiszTempDoEEPROM();
	}
	else {
		digitalWrite(12, HIGH);
	}
	delay(500);
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

	//lcd.print("Time:");
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

	lcd.setCursor(12, 1);
	lcd.print(licznikZapisan);
}


void zmianaJasnosciNaPrzycisku() {

	if (digitalRead(8) == LOW) { //Jeœli przycisk wciœniêty
		digitalWrite(9, HIGH); //W³¹cz diodê

		if (brightLCD == false) {
			brightLCD = true;
			jasnosc = 200;
		}
		else {
			jasnosc = 0;
			brightLCD = false;
		}
	}
	else {
		digitalWrite(9, LOW);

	}
}


void zapiszTempDoEEPROM() {
	int temp;
	int Y = 1021;
	float temperatura;
	if (licznikZapisan<Y) {
		if (seconds % 85 == 0 ) {
			sensors.requestTemperatures(); //Pobranie temperatury czujnika
			temperatura = sensors.getTempCByIndex(0);
			EEPROM.put(licznikZapisan, changeTempToByte(temperatura));

			licznikZapisan++;
			Serial.print(licznikZapisan);
			Serial.print(";");
			Serial.print(changeTempToByte(temperatura));
			Serial.print(";");
			Serial.println(seconds);

			wewLicznik = seconds;

		}
		
		if (seconds % 85 == 1 && wewLicznik!=(seconds-1)) {//to kurewstwo wyzej czasem nie lapie, dlatego trzeba sprawdzic jedna liczbe dalej
			sensors.requestTemperatures(); //Pobranie temperatury czujnika
			temperatura = sensors.getTempCByIndex(0);
			EEPROM.put(licznikZapisan, changeTempToByte(temperatura));
			licznikZapisan++;
			Serial.print(licznikZapisan);
			Serial.print(";");
			Serial.print(changeTempToByte(temperatura));
			Serial.print(";");
			Serial.println(seconds);
		}

		
	}
	else {
		EEPROM.put(1021, hours);
		EEPROM.put(1022, minutes%60);
		EEPROM.put(1023, seconds%60);
		endEepromSaving = true;	
	}
}



void EEPROMtest() {
	byte lol;
	for (int i = 0; i < 256; i++)
	{
		lol = (byte)0;
		EEPROM.update(i, lol);
	}
}

byte changeTempToByte(float temp) {
	return ((byte)(roundf(temp * 10)-100)); //bierzemy np 37.78 -> robimy z tego 378, odejmujemy 100 i mamy bajta (-100 zeby temp zawsze sie miescila w przedziale ktory chce, chyba ze temp w pokoju przekroczu 35.5 stopni iksde)
}

