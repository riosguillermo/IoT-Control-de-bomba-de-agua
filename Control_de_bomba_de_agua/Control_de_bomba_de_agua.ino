//10/08/2024
//Codigo de un dispositivo IoT que controla la temperatura de una bomba de agua.
//Utiliza Thingsboard como plataforma de dashboard y CallmeBot como API para alertas de whatsapp.
//Mas informacion: https://github.com/riosguillermo/IoT-Control-de-bomba-de-agua



#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <UrlEncode.h>
#include <OneWire.h>               
#include <DallasTemperature.h>

#define PIN_BUZZ D2 // -->PIN del buzzer
#define ONE_WIRE_BUS D4 //--> PIN del dallas 18b20 (sensor de temperatura)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature Dallas(&oneWire);

const char* ssid = "Wifi_name"; // --> Nombre de la red wifi
const char* password = "Wifi_password"; // --> Contraseña de la red wifi

#define TOKEN "device_token" // -- >Token del dispositivo en Thingsboard
char ThingsboardHost[] = "demo.thingsboard.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

//TEMPERATURA
float temperatura;
unsigned long contTemp = 0;
const int intervaloTemp = 60000; --> Cada cuanto se medira la temperatura (en milisegundos)
const int umbralTemperatura = 34; --> Umbral de temperatura para sonar la alarma

//ALARMA
bool alarmaSuena = false;
unsigned long contAlarma = 0;
bool sentidoAlarma = true;
const int intervaloAlarma = 410;


// Whatsapp config
String phoneNumber = "phone"; -->Numero de telefono para enviar alerta por whatsapp
String apiKey = "apikey"; --> Apikey del callmebot
bool enviarWhatsapp = false;
const String mensaje = "Alerta bomba de agua!\n Temperatura: "; --> Mensaje que se enviara por whatsapp
const int tiempoParaEnviarWhatsapp = 600000; -->Tiempo de espera antes de enviar un mensaje por whatsapp cuando la temperatura supere el umbral (en milisegundos)
unsigned long contWhatsapp = 0;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer( ThingsboardHost, 1883 );
  Dallas.begin();

  
}
 
void loop() {
    if ( !client.connected() ) {reconnect();}
  Alarma();
  if(millis() > contTemp){
      contTemp = millis() + intervaloTemp;
      temperatura = TomarTemperatura();
      Serial.println(temperatura);
      
      ProcesoDataHttp(); 

      if(temperatura >= umbralTemperatura){
        alarmaSuena = true;
        if(enviarWhatsapp == false){
           enviarWhatsapp = true;
           contWhatsapp = millis() + tiempoParaEnviarWhatsapp;
        }    
      } else{
        alarmaSuena = false;
        enviarWhatsapp = false;       
      }

      
      if(enviarWhatsapp == true && (millis() > contWhatsapp)){
        sendMessage(mensaje + temperatura);
        enviarWhatsapp = false;
      }
      
  }
}

void Alarma(){
  if(alarmaSuena == true){
    if(millis() > contAlarma){
      if(sentidoAlarma == true){
        analogWrite(PIN_BUZZ, 255);
        sentidoAlarma = false;
        contAlarma = millis() + intervaloAlarma;
     }else{
        analogWrite(PIN_BUZZ, 0);
        sentidoAlarma = true;
        contAlarma = millis() + intervaloAlarma;
      }
    }
  }else{
     analogWrite(PIN_BUZZ, 0);
  }
}

void ProcesoDataHttp()
{
  

  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"temp\":";
 payload += temperatura; 
  payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish( "v1/devices/me/telemetry",attributes);
  Serial.println( attributes );
   
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 500 );
    }
  }
}

void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

float TomarTemperatura(){
  float temp;
  Dallas.requestTemperatures(); // Send the command to get temperature readings 
  temp = Dallas.getTempCByIndex(0);
  return temp;
}
