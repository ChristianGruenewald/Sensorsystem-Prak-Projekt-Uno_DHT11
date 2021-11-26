/*******************************************************************************************
 *******************************************************************************************
 **                                                                                       **
 ** Autor: Christian Grünewald                                                            **
 ** Datum: November 2021                                                                  **
 ** Version: 1.0                                                                          **
 * *****************************************************************************************
*/


#include "DHT.h" 
             
#define DHTPIN 8          
#define DHTTYPE DHT11


                        
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Testprogramm");
  dht.begin();
}
void loop() {
  delay(2000);                   
                                    
  float h = dht.readHumidity();    
  float t = dht.readTemperature(); 
  
  if (isnan(h) || isnan(t)) {       
    Serial.println("Fehler beim auslesen des Sensors!");
    return;
  }

  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(h);                  
  Serial.print(" %\t");              
  Serial.print("Temperatur: ");
  Serial.print(t); 
  Serial.print(" °C\n\r");                 
}
