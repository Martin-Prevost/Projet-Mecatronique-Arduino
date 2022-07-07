#include <IRremote.h>
#include <Servo.h>  //import librairie du servomoteur
#include "rgb_lcd.h"
#include <Wire.h>
#include <SoftwareSerial.h>

//Selection de mode
int selec = 0;
char pos = 'Z';

//Moteur
Servo servoL; //Créer une instance de la class Servo
Servo servoR;
const int aL = 90, aR = 90, L80=85, L100=103;
const int pas = 45;
int posL=90;
int posR=90;

//Joystick
int x0 = 210, x1 = 410, x2 = 610, x3 = 810, x;
int y0 = 210, y1 = 410, y2 = 610, y3 = 810, y;

//IR commande
const char DIN_RECEPTEUR_INFRAROUGE = A1; //Déclaration du pin sur lequel est branché le récepteur
IRrecv monRecepteurInfraRouge(DIN_RECEPTEUR_INFRAROUGE); //Déclaration de l'utilisation du récepteur IR
decode_results messageRecu; //Variable qui permet de stocker le message reçu par le récepteur

//Ultra son
int trigPin1 = 6;
int echoPin1 = 7;
int trigPin2 = 8;
int echoPin2 = 9;
long duration1, duration2;
int d1, d2;
int s1 = 10, s2 = 20, s3 = 6, s4 = 15;
bool rien;

//LCD
rgb_lcd lcd;

//App inventor
SoftwareSerial SerialBT(2, 3);
bool envoie = false;

//Capteur gyroscopique
float AxeX;
bool correction =false;

void setup() {
  Serial.begin(9600);

  //Moteur L et R
  servoL.attach(4); //L
  servoR.attach(5); //R

  //Joystick
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  //irCom
  monRecepteurInfraRouge.enableIRIn();//Initialisation:activation du récepteur IR

  //Ultra son
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  //App inventor
  SerialBT.begin(9600);
  //pinMode(A0, INPUT); pas utile ?

  //LCD
  lcd.begin(16, 2);
  lcd.setRGB(0, 147, 255);
  lcd.print("Choisir l'option");
  lcd.setCursor(0, 1);
  lcd.print("1, 4 ou 7");

  //Capteur gyroscopique
  pinMode(A0, INPUT);
  
  //Moteur
  servoL.write(posL);
  servoR.write(posR);
}

void setMoteur(int l, int r){
    if (posL < l ){ 
        posL++;
        servoL.write(posL);
    } else if (posL > l){
        posL--;
        servoL.write(posL);
    }
    if (posR < r ){
        posR++;
        servoR.write(posR);
    } else if (posR > r){
        posR--;
        servoR.write(posR);
    }
}

void moteur() {
  switch (pos) {
    case 'A':
        setMoteur(115,65);
        Serial.println("Avance");
        break;
      
    case 'B':
        setMoteur(180,0);
        Serial.println("Avance v");
        break;

    case 'C':
        Serial.println("Tourne à droite");
        setMoteur(115,115);
        break;

    case 'D':      
        Serial.println("Tourne à droite");
        setMoteur(180,180);
        break;

    case 'E':
        Serial.println("Recule");
        setMoteur(65,115);
        break;

    case 'F':
        Serial.println("Recule vite");
        setMoteur(0,180);
        break;

    case 'G':
        Serial.println("Tourne à gauche");
        setMoteur(65,65);
        break;

    case 'H':
        setMoteur(0,0);
        break;

    case 'Z':
      //Serial.println("Stop");
      servoL.write(aL);
      servoR.write(aR);
      posL = aL;
      posR = aR;
      break;
  }
}

void joystick() {
  x = analogRead(A2);
  y = analogRead(A3);

  if (y1 <= y && y <= y2) {
    if  (x <= x0) {
      pos = 'H' ;
    } else if (x <= x1) {
      pos = 'G' ;
    } else if (x2 <= x && x <= x3) {
      pos = 'C';
    } else if (x >= x3) {
      pos = 'D' ;
    } else {
      pos = 'Z';
    }
  } else if (x1 <= x && x <= x2) {
    if  (y <= y0) {
      pos = 'B' ;
    } else if (y <= y1) {
      pos = 'A' ;
    } else if (y2 <= y && y <= y3) {
      pos = 'E';
    } else if (y >= y3) {
      pos = 'F' ;
    } else {
      pos = 'Z';
    }
  } else {
    pos = 'Z';
  }
}

void irCom() {
  if (monRecepteurInfraRouge.decode(&messageRecu)) {
    switch (messageRecu.value) {
      case 0xFF02FD:
        Serial.println("avancer vitesse 2");
        pos = 'B';
        break;
      case 0xFF906F:
        Serial.println("droite vitesse 2");
        pos = 'D';
        break;
      case 0xFFE01F:
        Serial.println("gauche vitesse 2");
        pos = 'H';
        break;
      case 0xFF18E7:
        Serial.println("reculer vitesse 1");
        pos = 'E';
        break;
      case 0xFF38C7:
        Serial.println("reculer vitesse 2");
        pos = 'F';
        break;
      case 0xFFB04F:
        Serial.println("droite vitesse 1");
        pos = 'C';
        break;
      case 0xFF6897:
        Serial.println("gauche vitesse 1");
        pos = 'G';
        break;
      case 0xFFA857:
        Serial.println("avancer vitesse 1");
        pos = 'A';
        break;
      case 0xFF9867:
        Serial.println("Arreter");
        pos = 'Z';
        break;
      case 0xFF30CF:
        Serial.println("Mode 1");
        selec = 1;
        break;
      case 0xFF10EF:
        Serial.println("Mode 2");
        selec = 2;
        break;
      case 0xFF42BD:
        Serial.println("Mode 3");
        selec = 3;
        break;
    }
    monRecepteurInfraRouge.resume();
  }
}

void selectFN() {
  if (monRecepteurInfraRouge.decode(&messageRecu)) {
    Serial.println(messageRecu.value, HEX);
    switch (messageRecu.value) {
      case 0xFF30CF:
        Serial.println("Mode 1");
        lcd.clear();
        lcd.print("Mode 1");
        selec = 1;
        break;
      case 0xFF10EF:
        Serial.println("Mode 2");
        lcd.clear();
        lcd.print("Mode 2");
        selec = 2;
        break;
      case 0xFF42BD:
        Serial.println("Mode 3");
        lcd.clear();
        lcd.print("Mode 3");
        selec = 3;
        break;
    }
    monRecepteurInfraRouge.resume();
  }
}

void ultraSon() {
  // Gestion de l'émission
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  String l1, l2;
  int r = 73, g = 255, b = 0;

  // Gestion de la réception
  duration1 = pulseIn(echoPin1, HIGH);

  // Calcul
  d1 = duration1 * 0.034 / 2;

  if (d1 <= s1) {
    if (pos == 'A' || pos == 'B') {
      pos = 'Z';
    }
    l1 = "Stop";
    r = 255, g = 0, b = 0;
  } else if (d1 <= s2) {
    r = 255, g = 0, b = 0;
    l1 = (String) d1;
  } else {
    l1 = (String) d1;
  }
  
  //Arrière
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn(echoPin2, HIGH);
  d2 = duration2 * 0.034 / 2;

  if (d2 <= s3) {
    if (pos == 'E' || pos == 'F') {
      pos = 'Z';
    }
    l2 = "Stop";
    r = 255, g = 0, b = 0;
  } else if (d2 <= s4) {
    l2 = (String) d2;
    r = 255, g = 0, b = 0;
  } else {
    l2 = (String) d2;
  }

  printEcran("Avant : " + l1, "Arriere : " + l2, r, g, b);
}

void appRecep() {
  if (SerialBT.available()) {
    pos = SerialBT.read();
  }
}

void appEnvoie() {
  static uint32_t lastTimeEnvoi = 0;    //faire que tout les 500ms
  if( millis() >  lastTimeEnvoi + 500 ){
    uint16_t dis1 = d1;          
    uint16_t dis2 = d2 + 30000; 
    SerialBT.write( dis1 %256 );   
    SerialBT.write( dis1 /256 );   
    SerialBT.write( dis2 %256 );  
    SerialBT.write( dis2 /256 );  
    lastTimeEnvoi = millis();
  }

}

void gyroscopique() {
  AxeX = (analogRead(A0) - 333) * 1.285;

  if (AxeX >= 50) {
    pos = 'B';
    correction =true;
  } else if (AxeX <= -50) {
    pos = 'F';
    correction =true;
  } 
  if (AxeX < 16 && AxeX >-16 && correction){
    correction=false;
    pos='Z';
  }
}

void printEcran(String l1, String l2, int  r, int g, int b) {
  static uint32_t lastTime = 0;
  if (millis() > lastTime + 100) {
    lcd.clear();
    lcd.setRGB(r, g, b);
    lcd.setCursor(0, 0);
    lcd.print(l1);
    lcd.setCursor(0, 1);
    lcd.print(l2);
    lastTime = millis();
  }
}

void loop() {
  if (selec != 0) {

    switch (selec) {        //selection mode
      case 1:
        irCom();
        break;

      case 2:
        selectFN();
        joystick();
        break;

      case 3:
        selectFN();
        appRecep();
    }
    gyroscopique();
    if(!correction){ultraSon();}
    appEnvoie();   
    moteur();

  } else if (selec == 0) {
    selectFN();
  }
}
