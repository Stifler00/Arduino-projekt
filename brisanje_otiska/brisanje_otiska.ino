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
  
 Serial.println("\nBrisanje spremljenih otisaka");
 
 finger.begin(57600);//Inicijalizacija komunikacije sa senzorom 
 
 //provjeravamo lozinku senzora koja je postavljena u biblioteci( može se promijeniti u kodu)
 //ako je lozinka točna( ako nismo mijenjali onda će biti točna)
 //ispisujemo poruku da je senzor pronađen( ako smo provjerili lozinku znači da je i komunikacija sa senzorom uspjela)
 if (finger.verifyPassword()) {
 Serial.println("Pronadjen senzor otiska prsta!");
 } else {
 //ako je lozinka netočna ili nije pronađen senzor ispisujemo poruku na serial monitor ostajemo u beskonačnoj while petlji
 Serial.println("Senzor otiska prsta nije pronadjen");
 while (1); 
 }
}


//funkcija koja čita broj sa Serial monitora i vraća taj broj
uint8_t readnumber(void) {
 uint8_t num = 0;
  
 while (num == 0) {
 while (! Serial.available());
 num = Serial.parseInt();
 }
 return num;
}
 
void loop() 
{
 Serial.println("Unesite ID # otiska (od 1 do 127) koji želite obrisati...");
 uint8_t id = readnumber();
 //u varijablu id spremamo broj koji nam vraća funkcija readnumber koja čita broj sa Serial monitora
 if (id == 0) {// ako je id nula vraćamo se na početak te ponovno čekamo unos broja 
 return;
 }
 
 Serial.print("Brisanje ID #");
 Serial.println(id);//ispisujemo koji ID se briše
  
 deleteFingerprint(id);// pozivamo funkciju za brisanje otiska
}
 
// funkcija za brisanje otisaka
uint8_t deleteFingerprint(uint8_t id) {
 uint8_t p = -1;
  
 p = finger.deleteModel(id); //brišemo otisak s predanim ID brojem, a funkcija nam vraća rezultat brisanja 
 
 //ovisno što nam je prethodna funkcija vratila( uspješno brisanje ili pogrešku) ispisujem poruku ovisno što se dogodilo 
 if (p == FINGERPRINT_OK) {
 Serial.println("Obrisano!");
 } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
 Serial.println("Greska u komunikaciji");
 return p;
 } else if (p == FINGERPRINT_BADLOCATION) {
 Serial.println("Nije moguce obrisati na toj lokaciji");
 return p;
 } else if (p == FINGERPRINT_FLASHERR) {
 Serial.println("Greska pisanja po memoriji");
 return p;
 } else {
 Serial.print("Nepoznata greska: 0x"); Serial.println(p, HEX);
 return p;
 } 
}
