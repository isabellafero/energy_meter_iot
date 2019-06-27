//declara as várias utilizadas
int leitura;
double soma;
double valorADC;
double tensao;
int PinoTensao = 34; //Pino analógico conectado ao sensor

void setup() {
  Serial.begin(115200); //Habilita a comunicação serial
  pinMode(19,OUTPUT);
  digitalWrite(19,HIGH);
}

void loop() {
  soma=0;
    for (int i = 0; i <= 1920; i++){
        leitura=analogRead(PinoTensao); //aquisição da leitura do ADC
        //Serial.println(leitura);
        soma+=leitura*leitura; //somatório do quadrado da leitura
        delayMicroseconds(390); //aguarda 390ms para próxima leitura
     }
     valorADC = (float) sqrt(soma/1920)*(3.300/4096.00);  //Calcula a tensão da saída do sensor
     tensao = (float) 15.28615*valorADC*valorADC + 76.91308*valorADC - 41.78664;
      Serial.println(tensao); //Imprime a tensao
}
//double tensao;
//Serial.print(" - ");

//tensao = 152.33*valor - 156.77;
