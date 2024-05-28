int venti1 = 9;
int venti2 = 10;

void changePwmVenti1(float percentage);
void Venti2();

void Venti2(){
  analogWrite(venti2,(int)(50*255/100));
}
// PWM is the value between 0 and 100
void changePwmVenti1(float percentage){
  analogWrite(venti1,(int)(percentage*255/100));
  
}
