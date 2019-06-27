#include <WiFi.h> // biblioteca que possibilita a conexão do ESP32 à internet
#include <PubSubClient.h> // biblioteca responsável  pela comunicação MQTT

// Definição dos pinos
#define PinoTensao 34
#define PinoCorrente 32
#define PinoRele 19

// Parâmentos da conexão WiFi
char ssid[] = "2.4G.NETVIRTUA_26AP502"; // Nome da rede
char pass[] = "3220368400"; // Senha da rede

//char ssid[] = "SFERREIRA"; // Nome da rede
//char pass[] = "12102019"; // Senha da rede

// Parâmentos da conexão com o ThingSpeak
const char* server = "mqtt.thingspeak.com"; //Servidor do broker MQTT
char mqttUserName[] = "ESP32_client"; // Nome do usuário
char mqttPass[] = "BGZX9V9CFUI2L4PQ"; // Chave do MQTT API
long ChannelID = 771688; // ID do canal
char writeAPIKey[] = "LVN75X7UEZZNL8CW"; // Chave de escrita do canal
char readAPIKey[]="B0VS9JT443XFRFCM"; // Chave de leitura do canal

// Inicialização das bibliotecas WiFiClient e PubSubClient 
WiFiClient esp32client; 
PubSubClient mqttClient( esp32client );

// Variáveis globais
float Vrms;
float Irms;
float P;
boolean EstadoSaida; //indica ao loop que a carga está ativada
unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 18L * 1000L; // Publica dados a cada 20 segundos

// Protótipos das funções
void calcula_tensao();   // Calcula a tensão
void calcula_corrente(); // Calcula a corrente
int mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length); // Recebe a mensagem do topic MQTT subscrito
void mqConnect(); // Gera um Client ID único e conecta ao MQTT broker 
int mqttSubscribe(); // Subscreve um campo de um canal do ThingSpeak 
int mqttUnSubscribe(); // Para de subscrever um campo de um canal do ThingSpeak
void mqttPublish(); // Publica mensagens no canal
int connectWifi(); // Conecta na rede WiFi
void getID(char clientID[], int idLength); // Gera um client ID aleatório para a conexão MQTT

//Função de configurações
void setup() {
  Serial.begin( 115200 );
  Serial.println( "Inicio" );
  connectWifi();  // conecta ao WiFi da rede
  mqttClient.setServer( server, 1883 ); // Conecta a porta do MQTT
  mqttClient.setCallback( mqttSubscriptionCallback );   // Configura a função que recebe a mensagem do MQTT 

  //Configura os pinos do ESP32
  pinMode(PinoTensao,INPUT); //Configura o PinoTensao como entrada
  pinMode(PinoCorrente,INPUT); //Configura o PinoCorrente como entrada
  pinMode(PinoRele,OUTPUT); //Configura o PinoRele como saída
  digitalWrite(PinoRele,LOW); //Inicializa o pino do Relé com nível baixo 
}

//Função principal
void loop() {
    
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    
    if (!mqttClient.connected())
    {      
       mqConnect(); // Conecta ao client MQTT se não estiver conectado
       if(mqttSubscribe()==1 ){
                Serial.println( " Subscrito " );
            }
    }
    
    mqttClient.loop(); // Chame o loop para manter a conexão com o servidor

		// Se o intervalo de tempo passou desde a última conexão, publica dados ao ThingSpeak
		if (millis() - lastConnectionTime > postingInterval) 
		{
			mqttpublish();
			//Escreve os dados publicados na porta Serial
			Serial.print(Vrms);
			Serial.print(" - ");
			Serial.print(Irms);
			Serial.print(" - ");
			Serial.println(EstadoSaida);
		}
}

/**
 * Publica os dados no topico correpondente ao canal do ThingsPeak  
 */

void mqttpublish() {
  
	calcula_tensao();
	if(EstadoSaida == 1){ //Testa se o dispositivo está ligado para então realizar a medição da corrente
		calcula_corrente();
	}
	else Irms = 0;
	P = Vrms*Irms; // Calcula a potência
  
  // Cria uma string com os dados para enviar ao ThingSpeak
  String data = String("field1=" + String(Vrms, DEC) + "&field2=" + String(Irms, DEC) + "&field3=" + String(P, DEC));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  Serial.println(msgBuffer);
  
  // Cria uma string com o topic e publica dados para os campos do canal do ThingSpeak
  String topicString ="channels/" + String( ChannelID ) + "/publish/"+String(writeAPIKey);
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  mqttClient.publish( topicBuffer, msgBuffer );

  lastConnectionTime = millis(); //Reseta o temporizador para publicar as mensagens
}

/**
 * Faz a conexão ao broker MQTT
 */

void mqConnect()
{
    char clientID[ 9 ];
    
    // Loop until connected.
    while ( !mqttClient.connected() )
    {

        getID(clientID,8);
       
        // Connect to the MQTT broker.
        Serial.print( "Tentando conexão MQTT..." );
        if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) )
        {
            Serial.println( "Conectado com o Client ID:  " + String( clientID ) + " Usuário "+ String( mqttUserName ) + " Pwd "+String( mqttPass ) );
           
        } else
        {
            Serial.print( "Falha, rc = " );
            Serial.print( mqttClient.state() );
            Serial.println( " Nova tentativa será feita em 5 segundos" );
            delay( 5000 );
        }
    }
}

/**
 * Cria um cliente aleatório client ID
 *   clientID - char matriz para saída
 *   idLength - tamanho do clientID
 */

void getID(char clientID[], int idLength){
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                        // Utilizado para geração aleatória do Client ID

    // Gera o ClientID
    for (int i = 0; i < idLength ; i++) {
        clientID[ i ] = alphanum[ random( 51 ) ];
    }
    clientID[ idLength ] = '\0';
}

/**
 *	Subscreve o campo 4 do canal
 */
 
int mqttSubscribe(){
    String myTopic;
    
    myTopic="channels/"+String( ChannelID )+"/subscribe/fields/field4/"+String( readAPIKey );
    
    Serial.println( "Subscrito de " +myTopic );
    Serial.println( "Estado= " + String( mqttClient.state() ) );
    char charBuf[ myTopic.length()+1 ];
    myTopic.toCharArray( charBuf,myTopic.length()+1 );
    
    return mqttClient.subscribe(charBuf);
}

/**
 *	Deixa de subscrever o campo 4 do canal
 */

int mqttUnSubscribe(){
    String myTopic;
      
    myTopic="channels/"+String( ChannelID )+"/subscribe/fields/field4/"+String( readAPIKey );

    char charBuf[ myTopic.length()+1 ];
    myTopic.toCharArray( charBuf, myTopic.length()+1 );
    return mqttClient.unsubscribe( charBuf );
    
}

/**
 *	Faz a conexão com o WiFi
 */

int connectWifi()
{
    
    while ( WiFi.status() != WL_CONNECTED ) {
        WiFi.begin( ssid, pass );
        delay( 2500 );
        Serial.println( "Conectando ao WiFi" ); 
    }
    Serial.println( "Conectado" );
}

/**
 *	Faz o cálculo da tensão
 */

void calcula_tensao(){
  int leitura_v;
  float soma_v = 0;
  float valor_v;
  
    for (int i = 0; i <= 1920; i++){
        leitura_v=analogRead(PinoTensao); //aquisição da leitura do ADC
        soma_v+=leitura_v*leitura_v; //somatório do quadrado da leitura
        delayMicroseconds(510); //aguarda 390ms para próxima leitura
     }

     valor_v = (float) sqrt(soma_v/1920)*(3.300/4096.00);  //Calcula a tensão da saída do sensor
     
     if (valor_v == 0) {
      Vrms=0;
     }
     else{
      Vrms = (float) 15.28615*valor_v*valor_v + 76.91308*valor_v - 41.78664;
     }
}

/**
 *	Faz o cálculo da corrente
 */

void calcula_corrente(){
  int leitura_i;
  float soma_i = 0;
  float valor_i;
  
  soma_i = 0;
  for (int i = 0; i <= 1920; i++){
      leitura_i=analogRead(32); //aquisição da leitura do ADC 
      leitura_i = leitura_i - 3000;
       
         
      soma_i+=leitura_i*leitura_i; //somatório do quadrado da leitura
      delayMicroseconds(390); //aguarda 390ms para próxima leitura
   }
   valor_i = (float) sqrt(soma_i/1920)*(3.300/4096.00)/0.185; //faz o calculo do valor eficaz//Calcula a tensão da saída do sensor e divide pela sensibilidade
   Irms = (float) 1.170989024*(valor_i)-0.072530783;
}

/**
 *	Função de callback que recebe a mensagem do tópico subscrito
 */

int mqttSubscriptionCallback( char* topic, byte* payload, unsigned int mesLength ) 
{
    String msg;
    //obtem a string do payload recebido
    for(int i = 0; i < mesLength; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }  
    //Executa uma ação dependendo da string recebida:
	
    //verifica se deve colocar nivel alto de tensão na saída do PinoRele:
    if (msg.equals("1"))
    {
        digitalWrite(PinoRele, HIGH);
        EstadoSaida = 1;
    }
 
    //verifica se deve colocar nivel baixo de tensão na saída D0:
    if (msg.equals("0"))
    {
        digitalWrite(PinoRele, LOW);
        EstadoSaida = 0;
    }     
}
