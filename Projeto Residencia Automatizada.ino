// * Programa: Tutorial Casa Monitorada *
//Autor: Curto Circuito * 
//Descrição: Programa de monitoramento e controle residencial através de aplicativo * 
//em comunicação MQTT com o ESP8266 
//******************************************************************************/

/*BIBLIOTECAS*/ 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>


#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];                                                                        

#define DHTPIN 5                       
#define DHTTYPE DHT22                 //define sensor de temperatura para o tipo DHT22

DHT dht(DHTPIN, DHTTYPE);                                                                                                                                                 
const char* ssid = "xxxxxxxx";        //define o nome da rede                                                     
const char* password = "xxxxxxx";     //define a senha da rede                                                      
const char* mqtt_server = "driver.cloudmqtt.com";    //server MQTT                                   
WiFiClient espClient;                                                                             
PubSubClient client(espClient);                                                                   
unsigned long lastMsg = 0;                                                                        
int value = 0;                                                                                    


char Sala;
char Quarto;
char Cozinha;
char Banheiro;
char VentiladorA;        //o ventilador conta com dois pinos destinados ao controle INB e INA, sendo que o acionamento define o sentido da rotação
char Automatico;         //variavel para controle do ventilador de forma automática

float h = dht.readHumidity();
float t = dht.readTemperature();

void setup() {

  dht.begin();
  Serial.println(F("Sensor DHT22"));
  Sala = 4;            //ligado ao pino d2
  Quarto = 2;          //ligado ao pino d4
  Cozinha = 14;        //ligado ao pino d5
  Banheiro = 12;       //ligado ao pino d6
  VentiladorA = 13;    //ligado ao pino d7
  Automatico == 'A';   // habilita por padrão modo automático
  
  pinMode(Sala, OUTPUT);
  pinMode(Quarto, OUTPUT);
  pinMode(Cozinha, OUTPUT);
  pinMode(Banheiro, OUTPUT);
  pinMode(VentiladorA, OUTPUT);

  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 18587);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  Serial.println("");
  Serial.print("Conectando com ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //delay (500);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Serial.println(F("Erro na leitura da temperatura!"));
  }
  else {
    Serial.print(F("Temperatura: "));
    Serial.print(t);
    Serial.print(F("°C"));
    sprintf(msg, "%f", t);
    client.publish("casa/temperatura", msg);
    Serial.print(F("     Umidade: "));
    Serial.print(h);
    Serial.println(F("%"));
    sprintf(msg, "%f", h);
    client.publish("casa/umidade", msg);
  }
  if (!client.connected()) {
    reconnect();
  }

//Acionando o modo automático do ventilador, ligando e desligando de acordo com a temperatura
  if (Automatico == 'A') {
    if (t > 23) {
      digitalWrite(VentiladorA, LOW);
      snprintf (msg, MSG_BUFFER_SIZE, "O ventilador está ligado");
      client.publish("casa/ventilador", msg);

    } else if (t < 23) {
      digitalWrite(VentiladorA, HIGH);
      snprintf (msg, MSG_BUFFER_SIZE, "O ventilador está desligado");
      client.publish("casa/ventilador", msg);
    }
  }
   delay(500);
  client.loop();

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }


 
//Acionando o modo manual dos componentes, inclusive do ventilador.
  
  Serial.println("");
  if ((char)payload[0] == 'S') {
    digitalWrite(Sala, HIGH);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz da sala está ligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/sala", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 's') {
    digitalWrite(Sala, LOW);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz da sala está desligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/sala", msg);
  }
  Serial.println("");
  if ((char)payload[0] == 'Q') {
    digitalWrite(Quarto, HIGH);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz do quarto está ligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/quarto", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'q') {
    digitalWrite(Quarto, LOW);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz do quarto está desligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/quarto", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'C') {
    digitalWrite(Cozinha, HIGH);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz da cozinha está ligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/cozinha", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'c') {
    digitalWrite(Cozinha, LOW);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz da cozinha está desligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/cozinha", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'B') {
    digitalWrite(Banheiro, HIGH);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz do banheiro está ligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/banheiro", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'b') {
    digitalWrite(Banheiro, LOW);
    snprintf (msg, MSG_BUFFER_SIZE, "A luz do banheiro está desligada");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/banheiro", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'v') {
    digitalWrite(VentiladorA, HIGH);
    snprintf (msg, MSG_BUFFER_SIZE, "O ventilador está desligado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/ventilador", msg);
  }

  Serial.println("");
  if ((char)payload[0] == 'V') {
    digitalWrite(VentiladorA, LOW);
    snprintf (msg, MSG_BUFFER_SIZE, "O ventilador está ligado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/ventilador", msg);
  }


  //RECEBE O SINAL DO MODO AUTOMÁTICO
  Serial.println("");
  if ((char)payload[0] == 'a') {
    Automatico = 'a';
    snprintf (msg, MSG_BUFFER_SIZE, "Modo Automático do Ventilador está desligado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/automatico", msg);
  }
  Serial.println("");
  if ((char)payload[0] == 'A') {
    Automatico = 'A';
    snprintf (msg, MSG_BUFFER_SIZE, "Modo Automático do Ventilador está ligado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("casa/automatico", msg);
  }
  //FIM - RECEBE O SINAL DO MODO AUTOMÁTICO

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Aguardando conexão MQTT...");
    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    if (client.connect("usuario", "vnuvzsfc", "cUsSfAR62mUw")) {
      Serial.println("conectado");
      client.subscribe("casa/publisher");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println("tente novamente em 5s");
      delay(2000);
    }
  }
}