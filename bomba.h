int bombaAgua = 8; //señal digital bomba
void run_pump(){
 
  digitalWrite(bombaAgua,LOW);
  delay(4000);  //off
   digitalWrite(bombaAgua,HIGH);
  
  
  
}