void setup() {
  Serial.begin(9600);
}

void loop() {
    float temp;
    int aux;
    int i;
    delay(100);
    while(1){
      aux=0;
      for( i=0; i< 20; i++ ){
        aux+=analogRead(A0);
      }
      aux/=20;
      temp = ((float)aux)*5/1024;   //Lo mapeo de 0-3.3v
      temp /= 156;           //Lo divido por la ganancia del operacional
      temp =23701.6*temp-8.75;       //Mapeo la temperatura
      Serial.println(temp);

      delay(1000);
    }
}
