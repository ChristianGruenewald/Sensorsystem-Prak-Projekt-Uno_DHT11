# Sensorsystem-Prak-Projekt-Uno_DHT11 
 
## Schaltung:

- Pullup 5k Widerstand gegen Signal wichtig!
![image](https://user-images.githubusercontent.com/95079900/143572654-5e1f8327-9091-4de2-957b-ae08101d5c41.png)

## Programmierung in Ardurino
- For Ardrino Uno
- Important, include and download DHT.h!
./Projektfolder/src/Ardurino

## Programmierung in C mit Atmel-Studio
- Datenblätter beachten! Atmel Studio 8.0.
- Atmega 328p : https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf 
- Setup Atmel Studio to upload Code without Atmel Programmer via ICSP: https://www.youtube.com/watch?v=zEbSQaQJvHI 
./Projectfolder/src/Atmel-Nativ_C
- ! Neu: USART Protkoll wurde entwickelt: 
Wenn ein 'u' oder ein 'U' als ASCII Zeichen gesendet wird, antwortet der Atmega328p mit den ausgelesenen Daten des DHT11, auch als ASCII mit Nachkommastellen. 

