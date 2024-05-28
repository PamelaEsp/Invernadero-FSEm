// Variable para almacenar el valor obtenido del sensor (0 a 1023) 
float tempC2; 
float tempC1; 
float avgtemp; //Almacena el promedio de los sensores de temperatura

float read_Temperature(void){
  //Alamcena la lectura del sensor de los pines A0 y A1
  tempC1 = analogRead(0); 
  tempC2 = analogRead(1); 

   // Calculamos la temperatura haciendo la conversion del voltaje
  tempC1 = (5.0 * tempC1 * 100.0)/1024.0; 
  tempC2 = (5.0 * tempC2 * 100.0)/1024.0; 
  //Se obtiene el promedio de ambas temprarutas y esta será la tempretura enviada
	avgtemp = (tempC1 + tempC2) /2;
}

