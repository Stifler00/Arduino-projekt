//uključujemo biblioteku za rad sa senzorom otiska prsta
#include <Adafruit_Fingerprint.h>

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

uint8_t id;

void setup()
{
  Serial.begin(9600);//Inicijalizacija serijske komunikacije(brzina 9600 bauda)za ispis poruka na Serial monitor
  while(!Serial);
  delay(100);//pauza 100 ms
  Serial.println("\nUpisivanje novog otiska prsta");

  finger.begin(57600);//Inicijalizacija komunikacije sa senzorom

  //provjeravamo lozinku senzora koja je postavljena u biblioteci( može se promijeniti u kodu)
  //ako je lozinka točna( ako nismo mijenjali onda će biti točna) ispisujemo poruku da je senzor pronađen
  //( ako smo provjerili lozinku znači da je i komunikacija sa senzorom uspjela)
  if (finger.verifyPassword()) {
    Serial.println("Pronadjen senzor otiska prsta!");
  } else {
  //ako je lozinka netočna ili nije pronađen senzor ispisujemo poruku na serial monitor ostajemo u beskonačnoj while petlji
    Serial.println("Senzor otiska prsta nije pronadjen");
    while (1) { delay(1);}
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
  Serial.println("\nSpreman za dodavanje otiska!");
  Serial.println("Unesite ID#(od 1 do 127) pod kojim zelite spremiti novi otisak prsta...");
  id = readnumber();
  if (id == 0) {//ako je uneseni broj 0 vraćamo se na početak i tražimo ponovno unošenje broja
    return;
  }
  Serial.print("Upisivanje ID #");
  Serial.println(id);
  //pozivamo funkciju koja obavlja unošenje novog otiska
  while (! getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("cekam novi otisak prsta s ID #"); Serial.println(id);
  //sve dok ne dobijemo ispravan otisak ponavljamo postupak slikanja
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();// uzimamo sliku sa senzora otiska i u varijablu p spremamo povratnu vrijednost
    //prema vrijednosti u varijabli p ispisujemo odgovarajuću poruku što je senzor napravio,
    //odnosno dali se dogodila greška
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Slika napravljena");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        delay(3000);//pauza da se . ne ispisuje stalno
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Komunikacijska greska");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Greska sa slikom");
        break;
      default:
        Serial.println("Nepoznata greska");
        break;
    }
  }

  p = finger.image2Tz(1);//snimljenu sliku pretvaramo u drugi format koji se može pohraniti u memoriju
  switch (p) {
    //prema vrijednosti u varijabli p ispisujemo dali je pretvaranje uspjelo ili se dogodila greška
    case FINGERPRINT_OK:
      Serial.println("Slika pretvorena");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Slika neispravna");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Komunikacijska greska");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Nemoguce pronaci znacajke otiska");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Nemoguce pronaci znacajke otiska");
      return p;
    default:
      Serial.println("Nepoznata greska");
      return p;
  }
  // korisnik treba maknuti prst sa senzora te onda ponovno staviti kako bi potvrdili otisak
  Serial.println("Maknite prst sa senzora");
  delay(3000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Stavite isti prst ponovno na senzor");
  //kada korisnik ponovno stavi prst ponavljamo slikanje i konverziju slike
  //te ispisujemo poruke ako se dogodi greška kao i u prethodnom slučaju
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Slika napravljena");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        delay(100);//pauza da se . ne ispisuje stalno
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Komunikacijska greska");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Greska sa slikom");
        break;
      default:
        Serial.println("Nepoznata greska");
        break;
    }
  }
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Slika pretvorena");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Slika neispravna");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Komunikacijska greska");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Nemoguce pronaci znacajke otiska");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Nemoguce pronaci znacajke otiska");
      return p;
    default:
      Serial.println("Nepoznata greska");
      return p;
  }

  //Ispisujemo korisniku da je obrada slike gotova i da se izrađuje model otiska
  Serial.print("Pravim model za #"); Serial.println(id);

  p = finger.createModel();//funkcija koju pozivamo kako bi napravili model otiska prsta
  // provjeravamo povratnu vrijednost funkcije kako bi znali što je napravljeno
  if (p == FINGERPRINT_OK) {
    Serial.println("Otisci se poklapaju!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Komunikacijska greska");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Otisci se ne poklapaju");
    return p;
  } else {
    Serial.println("Nepoznata greska");
    return p;
  }
  //ako se otisci poklapaju onda spremamo model otiska u memoriju senzora
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  //funkcija za spremanje modela otiska u memoriju senzora koja nam vraća jeli spremanje uspjelo ili ne
  //Ispisujemo poruku ovisno o povratnoj vrijednosti funkcije za spremanje
  if (p == FINGERPRINT_OK) {
    Serial.println("Spremljeno!\n");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Komunikacijska greska");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Nemoguce spremiti na toj lokaciji ( probajte drugi ID)");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Greska sa spremanjem u memoriju");
    return p;
  } else {
    Serial.println("Nepoznata greska");
    return p;
  }
  return true;
}
