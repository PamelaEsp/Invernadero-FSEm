/*
* arduino-test-dc.cpp
*
* Author:  Mauricio Matamoros
* Date:    2020.03.01
* License: MIT
*
* Controls the power output of a resistive load using
* zero-cross detection and a TRIAC. Code for Arduino UNO
*
*/
int ZXPIN = 2;  //Pin D2 cruce por cero
int TRIAC = 3;  //Pin D3 cruce por cero
int led = 4;   //Pin D12 cruce por cero
//prototypes
float powerTime[101]={
	8127,8005,7903,7812,7728,7649,7574,7502,7433,7367,
	7302,7239,7178,7117,7059,7000,6944,6887,6832,6777,
	6723,6670,6616,6564,6512,6460,6409,6358,6307,6257,
	6207,6157,6107,6058,6008,5959,5910,5860,5811,5762,
	5713,5665,5615,5566,5517,5468,5419,5369,5320,5270,
	5220,5170,5120,5069,5018,4967,4916,4865,4812,4759,
	4707,4654,4600,4546,4491,4436,4380,4324,4266,4209,
	4150,4090,4030,3968,3905,3842,3777,3710,3643,3574,
	3503,3430,3355,3277,3198,3115,3029,2940,2845,2747,
	2643,2531,2412,2281,2138,1975,1785,1550,1222,0,0
};
// Globals
volatile bool flag = false;
int pdelay = 0;
int inc = 1;

void turnLampOn();
void zxhandle();

void lampPower(int percentage){
  pdelay = powerTime[percentage];
}

void turnLampOn(){
	// Turn sentinel LED on
	digitalWrite(13, HIGH);
  //digitalWrite(4, HIGH);
	// Send a 20us pulse to the TRIAC
	digitalWrite(TRIAC, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIAC, LOW);
  digitalWrite(led, LOW);

}

void zxhandle(){
	// TRIAC automatically shuts down on ZX
	digitalWrite(TRIAC, LOW);
	digitalWrite(13, LOW);
  digitalWrite(led, HIGH);
	delayMicroseconds(pdelay);

	if(pdelay > 0) turnLampOn();
}
