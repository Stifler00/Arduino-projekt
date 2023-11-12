//uključujemo biblioteku za rad sa senzorom otiska prsta
#include <Adafruit_Fingerprint.h>
 
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// Za UNO i ostale bez hardver serijala, treba koristiti softverski serijal
// pin #13 za ulaz (Žuta žica)
// pin #14 izlaz iz arduina (Bijela žica)
SoftwareSerial mySerial(2, 3);

#else
// Na Leonardo/M0/etc, ostalima sa hardverskim serijalom, koristiti hardverski serijal!
// #0 je žuta žica, #1 je bijela žica
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
 
void setup() 
{
 Serial.begin(9600);//Inicijalizacija serijske komunikacije( brzina 9600 bauda) za ispis poruka na Serial monitor
 while(!Serial);
 delay(100);//pauza 100 ms
  
 Serial.println("\n Brisanje svih spremljenih otisaka!");
 Serial.println("Pritisnite D za nastavak.");
 
//čekamo sve dok korisnik ne unese Y u serial monitor
 while (1) {
 if (Serial.available() && (Serial.read() == 'D')) {
 break;
 }
}
 
 finger.begin(57600);//Inicijalizacija komunikacije sa senzorom 
 delay(5);
 //provjeravamo lozinku senzora koja je postavljena u biblioteci( može se promijeniti u kodu)
 //ako je lozinka točna( ako nismo mijenjali onda će biti točna) ispisujemo poruku da je senzor pronađen
 //( ako smo provjerili lozinku znači da je i komunikacija sa senzorom uspjela)
 if (finger.verifyPassword()) {
 Serial.println("Pronadjen senzor otiska prsta!");
 } else {//ako je lozinka netočna ili nije pronađen senzor ispisujemo poruku na serial monitor ostajemo u beskonačnoj while petlji
 Serial.println("Senzor otiska prsta nije pronadjen");
 while (1); 
 }
 //funkciaj za brisanje cijele baze senzora
 finger.emptyDatabase();
 
Serial.println("Otisci obrisani!");
Serial.println("Baza otisaka je prazna");
}
 
void loop() {
}
