/*Elaborado por: Pamela Espinoza de los Monteros Camarillo*/
#include <Wire.h>
#include "temperature.h"
#include "bomba.h"
#include "ventilador.h"
#include "foco.h"
#include "pid.h"
#define SLAVE_ADDR 0x0A  // Dirección I2C del Arduino         <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
//float temp;     <<<<<<
float receivedValue = 0; //Accion a realizar
int tipeEventReq = 0;  //tipo de evento realizado
float percentage= 0; //PWM ventilador

// Prototypes

void setup() {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDR);  // Inicializa el Arduino como un dispositivo I2C con dirección SLAVE_ADDR     <<<<<<<
  Wire.onReceive(receiveEvent);  // Configura la función receiveEvent() para manejar la recepción de datos <<<<<<<
  Wire.onRequest(requestEvent);                   //  <<<<<<<<<<<<<<<<<<<<<<
  pinMode(bombaAgua,OUTPUT); //Salida de la bomba de agua x D8
  pinMode(venti1,OUTPUT); //Salida de la bomba de agua x D9
  pinMode(venti2,OUTPUT); //Salida de la bomba de agua x D10
  digitalWrite(bombaAgua,HIGH);

  pinMode(ZXPIN, INPUT); //pin d2
	// digitalPinToInterrupt may not work, so we choose directly the
	// interrupt number. It is Zero for pin 2 on Arduino UNO
	// attachInterrupt(digitalPinToInterrupt(ZXPIN), zxhandle, RISING);
	attachInterrupt(0, zxhandle, RISING); 
	// Setup output (triac) pin
	pinMode(TRIAC, OUTPUT); //D2
	// Blink led on interrupt
	pinMode(13, OUTPUT);
  pinMode(led, OUTPUT); //12
	Serial.begin(9600);
}

void loop() {
   if (Serial.available() > 0) {
    int newReq = Serial.parseInt();
    if (newReq >= 0 && newReq <= 3) {
       receivedValue = newReq;
       Serial.print("Tipo de evento solicitado: ");
       Serial.println(receivedValue);
     } else {
       Serial.println("Tipo de evento inválido");
     }
  }
  //receivedValue = 2;
 
	//receivedValue =Serial.parseInt();
    
    
  read_Temperature(); 
  executePid();
  //pid_f(); <<<<<
 
  Serial.println(avgtemp);
  if(receivedValue == 1){
    //Encender sistema de irrigación
    run_pump();
    tipeEventReq = 1;     //<<<<<<
    receivedValue = 0;
  }else if(receivedValue == 2){
    //executePid();   //<<<<<
    //Encender radiador
    //percentage = pid_f(); //<<<<<<
    lampPower(70);
    
    tipeEventReq = 2;     // <<<<<<
    receivedValue = 0;
  }
  else if(receivedValue == 3){
    //Encender ventilador}
    Venti2();
    changePwmVenti1(70.0);
    delay(20000);
    changePwmVenti1(0.0);
    tipeEventReq = 3;     //<<<<<<
    receivedValue = 0;
  } 
  
  delay(3000);
  }
  
void requestEvent(){                                      //<<<<<<<<<<<<<<<<<<<<<<<
  if (tipeEventReq == 0){ // sen via temperatura
    Serial.println("se encendio tmp");
    Serial.println(avgtemp);
    //Wire.write((byte*) &tipeEventReq, sizeof(int));
    Wire.write((byte*) &avgtemp, sizeof(float));  
  }else if(tipeEventReq == 1){ // se envia bomba
    Serial.print("se encendio la bomba");
    Wire.write((byte*) &tipeEventReq, sizeof(int));
    tipeEventReq = 0;
  }else if(tipeEventReq == 2){ // se envia luz
  Serial.print("se encendio la luz");
    Wire.write((byte*) &tipeEventReq, sizeof(int));
    tipeEventReq = 0;
  }else if(tipeEventReq == 3){ // se envia ventilador
  Serial.print("se encendio el vent");
    Wire.write((byte*) &tipeEventReq, sizeof(int));
    tipeEventReq = 0;
  }
  
}
void receiveEvent(int numBytes) {
  if (numBytes == sizeof(float)) {
    Wire.readBytes((char *)&receivedValue, sizeof(float)); 
    Serial.print("Power received from Raspberry Pi: ");
    Serial.println(receivedValue); 
  }
}           // <<<<<<<<<<<<<<<<<<<<<<<<<<
