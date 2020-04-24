#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
 
#define DHTPIN D5 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
 
const char* wifiName = "NOME DA SUA REDE";
const char* wifiPass = "SENHA DA SUA REDE";

DHT dht(DHTPIN, DHTTYPE);
  
ESP8266WebServer server(80);  //Define server object
 
//Handles http request 
void handleData() {
  String webPage;
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int leitura = analogRead(0);

  data["temperature"] = (String)t + "°C";
  data["humidity"] = (String)h + "%";
  
  data["sensor0"] = (String)(int)FazLeituraUmidade() + "%";
  data["solo_status0"] = retornaStatusSolo(leitura);
 
  data.printTo(webPage);  //Store JSON in String variable
  server.send(200, "text/html", webPage);
}
   
// the setup function runs once when you press reset or power the board
void setup() {
  
  Serial.begin(9600);
  delay(10);
  Serial.println();
  dht.begin();
  Serial.print("Connecting");
  
  WiFi.begin(wifiName, wifiPass);   //Connect to WiFi
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP
 
  server.on("/", handleData);      //Associate handler function to web requests
    
  server.begin(); //Start web server
  Serial.println("HTTP server started");
}

String retornaStatusSolo(int byteSolo)
{
  String valorSolo;
  
    if (byteSolo > 0 && byteSolo < 400)
    {
        valorSolo = "Solo umido";
    }

    if (byteSolo > 400 && byteSolo < 800)
    {
        valorSolo = "Umidade moderada";
    }

    if (byteSolo > 800 && byteSolo < 1024)
    {
        valorSolo = "Solo seco";
    }

    return valorSolo;
}

float FazLeituraUmidade(void)
{
    int ValorADC;
    float UmidadePercentual;
 
     ValorADC = analogRead(0);   //978 -> 3,3V
 
     //Serial.print("[Leitura ADC] ");
     //Serial.println(ValorADC);
 
     //Quanto maior o numero lido do ADC, menor a umidade.
     //Sendo assim, calcula-se a porcentagem de umidade por:
     //      
     //   Valor lido                 Umidade percentual
     //      _    0                           _ 100
     //      |                                |   
     //      |                                |   
     //      -   ValorADC                     - UmidadePercentual 
     //      |                                |   
     //      |                                |   
     //     _|_  978                         _|_ 0
     //
     //   (UmidadePercentual-0) / (100-0)  =  (ValorADC - 978) / (-978)
     //      Logo:
     //      UmidadePercentual = 100 * ((978-ValorADC) / 978)  
      
     UmidadePercentual = 100 * ((978-(float)ValorADC) / 978);
     Serial.print("[Umidade Percentual] ");
     Serial.print((int)UmidadePercentual);
     Serial.println("%");
 
     return (int)UmidadePercentual;
}
 
void loop() {
  //Handle Clinet requests
  server.handleClient();
  FazLeituraUmidade();

  delay(1000);
}
