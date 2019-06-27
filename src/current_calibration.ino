int leitura_i;
float soma_i;
float valor_i;
float Irms;
float conversao;


void setup() {
  Serial.begin(115200);
  pinMode(19,OUTPUT);
  digitalWrite(19,HIGH);
}

void loop() {
  soma_i = 0;
  for (int i = 0; i <= 1920; i++){
      leitura_i=analogRead(32); //aquisição da leitura do ADC 
      leitura_i = leitura_i - 3000;
       
         
      soma_i+=leitura_i*leitura_i; //somatório do quadrado da leitura
      delayMicroseconds(390); //aguarda 390ms para próxima leitura
   }
   conversao = (float) (3.300/4096.00)/0.185;
   valor_i = (float) conversao*sqrt(soma_i/1920); //faz o calculo do valor eficaz//Calcula a tensão da saída do sensor   
   //Serial.println(valor_i);
   //valor_i = (float) valor_i/0.185;
   //Serial.println(String(valor_i,DEC));
   //Serial.println(valor_i);
   Irms = (float) 1.170989024*(valor_i)-0.072530783;
   Serial.println(Irms);
   
   
   
   
   
   
   

   //Serial.print(valor);
   //Serial.print(" - ");
   //Serial.println(corrente);
  //delay(1000);
}
