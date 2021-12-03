/*******************************************************************************************
 *******************************************************************************************
 **                                                                                       **
 ** Autor: Christian Grünewald                                                            **
 ** Datum: November 2021                                                                  **
 ** Version: 1.0                                                                          **
 * *****************************************************************************************
*/


#include "DHT.h" 
#include <LiquidCrystal.h>
             
#define DHTPIN 13          
#define DHTTYPE DHT11

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
                        
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.begin(16, 2);
  lcd.print("*** DHT11 ***");
  delay(500);
  lcd.setCursor(0,1);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  Serial.begin(9600);
  Serial.println("DHT11 Testprogramm");
  dht.begin();
}
void loop() {
  delay(2000);                   
                                    
  float h = dht.readHumidity();    
  float t = dht.readTemperature(); 
  lcd.setCursor(0,0);
  
  if (isnan(h) || isnan(t)) {       
    Serial.println("Fehler beim auslesen des Sensors!");
    lcd.print("Fehler!");
    return;
  }

  lcd.print("Humi.: ");
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Temp.: ");
  lcd.print(t);
  lcd.print("C");

  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(h);                  
  Serial.print(" %\t");              
  Serial.print("Temperatur: ");
  Serial.print(t); 
  Serial.print(" °C\n\r");                 
}
