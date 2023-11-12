//uključujemo biblioteku za rad sa senzorom otiska prsta
#include <Adafruit_Fingerprint.h>
#include <Servo.h>


int ledR = 2; //crvena LED dioda
int ledY = 3; //žuta LED dioda
int ledG = 4; //zelena LED dioda
Servo myservo; //definiranje objekta myservo
int servoPin = 1; //definiranje pina za upravljanje servomotorom

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// Za UNO i ostale bez hardver serijala, treba koristiti softverski serijal
// pin #13 za ulaz (Žuta žica)
// pin #14 izlaz iz arduina (Bijela žica)
SoftwareSerial mySerial(13, 14);

#else
// Na Leonardo/M0/etc, ostalima sa hardverskim serijalom, koristiti hardverski serijal!
// #0 je žuta žica, #1 je bijela žica
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
String IDime;
int otisakID = 0;
 
void setup() 
{
 pinMode(ledR, OUTPUT);
 pinMode(ledY, OUTPUT);
 pinMode(ledG, OUTPUT);
 myservo.attach(servoPin);
 
 Serial.begin(9600);//Inicijalizacija serijske komunikacije( brzina 9600 bauda) za ispis poruka na Serial monitor
 while (!Serial);
 delay(100);//pauza 100 ms
  
 Serial.println("\nSenzor otiska prsta_ ocitavanje");
 
 finger.begin(57600);//Inicijalizacija komunikacije sa senzorom 
 delay(5);
 //provjeravamo lozinku senzora koja je postavljena u biblioteci( može se promijeniti u kodu)
 //ako je lozinka točna( ako nismo mijenjali onda će biti točna)
 //ispisujemo poruku da je senzor pronađen( ako smo provjerili lozinku znači da je i komunikacija sa senzorom uspjela)
 if (finger.verifyPassword()) {
 Serial.println("Pronadjen senzor otiska prsta!");
 } else {
 //ako je lozinka netočna ili nije pronađen senzor ispisujemo poruku na serial monitor ostajemo u beskonačnoj while petlji
 Serial.println("Senzor otiska prsta nije pronadjen");
 while (1) { delay(1); }
 }

 finger.getTemplateCount();//funkcija za dohvaćanje podataka o broju spremljenih otisaka
 //ispisujemo broje spremljenih otisaka na Serial monitor 
 
 if (finger.templateCount == 0) {
  Serial.print("Senzor ne sadrzi nijedan otisak prsta. Zapocni upis otiska.");
 } else {
   Serial.println("Ceka se valjani otisak...");
   Serial.print("Senzor ima "); Serial.print(finger.templateCount); Serial.println(" spremljenih otisaka");
 }
}
 
void loop() 
{
  digitalWrite(ledR, HIGH);
 getFingerprintID();//funkcija u kojoj očitavamo otiska prsta, ako je prst prislonjen na senzor 
 delay(50);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Snimljena slika");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Nije otkriven prst"); delay(2500);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Greska u komunikaciji");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Greska snimanja");
      return p;
    default:
      Serial.println("Nepoznata greska");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Slika pretvorena");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Slika previse neuredna");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Greska u komunikaciji");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Nije moguce pronaci znacajke otiska prsta");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Nije moguce pronaci znacajke otiska prsta");
      return p;
    default:
      Serial.println("Nepoznata greska");
      return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Pronadjeno podudaranje otiska!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Greska u komunikaciji");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Nije pronadjeno podudaranje");
    digitalWrite(ledY, HIGH);
    delay(100);
    digitalWrite(ledY, LOW);
    delay(100);
     digitalWrite(ledY, HIGH);
    delay(100);
    digitalWrite(ledY, LOW);
    delay(100);
    //žuta LED dioda označava da korisnik nije verificiran
    //te da se pokuša sa drugim prstom
    return p;
  } else {
    Serial.println("Nepoznata greska");
    return p;
  }
  //uspješan pronalazak otiska prsta te se pali zelena LED dioda
  //a gasi se crvena
  digitalWrite(ledG, HIGH);
  digitalWrite(ledR, LOW);
  delay(2000);
   myservo.write(0);
   delay(800);
   myservo.write(90);
   delay(800);
  digitalWrite(ledG, LOW);
  digitalWrite(ledR, HIGH);
  otisakID = finger.fingerID;

  Serial.print("Dobrodosao"); Serial.print(IDime);
  Serial.print(" (ID #"); Serial.print(finger.fingerID); Serial.print(")");
  Serial.print(" s pouzdanjem "); Serial.println(finger.confidence);
 
  return finger.fingerID;
}
 
//funkcija u kojoj očitavamo otisak i koja nam vraća -1 ako se dogodila greška ili
//ID otiska prsta kao se podudara s nekim od spremljenih otisaka 
int getFingerprintIDez() {
 uint8_t p = finger.getImage();
 if (p != FINGERPRINT_OK) return -1;
 
 p = finger.image2Tz();
 if (p != FINGERPRINT_OK) return -1;
 
 p = finger.fingerFastSearch();
 if (p != FINGERPRINT_OK) return -1;

  //uspješan pronalazak otiska prsta te se pali zelena LED dioda
  //a gasi se crvena
  digitalWrite(ledG, HIGH);
  digitalWrite(ledR, LOW);
  delay(2000);
   myservo.write(0);
   delay(800);
   myservo.write(90);
   delay(800);
  digitalWrite(ledG, LOW);
  digitalWrite(ledR, HIGH);
  otisakID = finger.fingerID;

 
  Serial.print("Dobrodosao"); Serial.print(IDime);
  Serial.print(" (ID #"); Serial.print(finger.fingerID); Serial.print(")");
  Serial.print(" s pouzdanjem "); Serial.println(finger.confidence);
 //ispisujemo koliko je pouzdan pročitan otisak s određenim otiskom iz baze 
 return finger.fingerID; //funkcija vraća ID otiska prsta
}
