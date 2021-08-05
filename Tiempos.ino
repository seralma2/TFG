int pinRadar = 7;
long tiempo1 = 0;
long tiempo2 = 0;
int estado = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(pinRadar, INPUT);
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
    if(digitalRead(pinRadar) == HIGH and estado == 0){
        //Serial.print(pinRadar);
        tiempo1 = millis();
        estado = 1;
    }
    if(digitalRead(pinRadar) == LOW and estado == 1){
        tiempo2 = millis();
        estado = -1;
    }
    if (tiempo1 > 0 and tiempo2 > 0 and estado == -1){
      Serial.print(tiempo1);
      Serial.print(",");  
      Serial.print(tiempo2);
      Serial.println();
      estado = 0;
   }
     
}
