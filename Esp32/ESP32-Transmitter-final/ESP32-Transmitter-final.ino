#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN "2WkSCDZKE-Zr0yrlMoqtgxDPXxQN2g8V"
#define BLYNK_TEMPLATE_ID "TMPL3_4ZVHEt9"
#define BLYNK_TEMPLATE_NAME "Flex Sensor Serial Monitor"
#define flexPin 34

#include<WiFi.h>
#include<esp_now.h>
#include<HTTPClient.h>
#include<BlynkSimpleEsp32.h>
//blynk
char auth[] = BLYNK_AUTH_TOKEN;

//wifi
const char *ssid = "onlyinternet";
const char *pwd = "nahipata";

//google sheet
String Google_Script_Url = "https://script.google.com/macros/s/AKfycbxymW_vNewZmTpuN4e00qsw3od5n2YVhhzqzKfArWFjtRVImOuNW_0RBVQbUJ0Ev3ZcrA/exec";

//espnow
uint8_t broadcastAddress[] = {0x08, 0x3A, 0x8D, 0xD0, 0x70, 0xFD};
struct message{
  int Sensor_Value;
};
struct message myData;

esp_now_peer_info_t peerInfo;

//general
int Path_Switch_State = 1;
int reading = 0;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(flexPin, INPUT);
  blynkSetup();
  delay(1000);
  wifiSetup();
  espnowSetup();
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast sent Package Status: \t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendData(int Value)
{
  if(WiFi.status() == WL_CONNECTED){
  String Send_data_url = Google_Script_Url + "?sts=write";
  Send_data_url += "&data=" + String(Value);

  Serial.println();
  Serial.println("------------");
  Serial.println("Send data to Google SpreadSheet....");
  Serial.print("Url : ");
  Serial.print(Send_data_url);
  
  HTTPClient http;
  
  //HTTp get request
  http.begin(Send_data_url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  //gets the http status code
  int httpCode = http.GET();
  Serial.print("HTTP Status Code : ");
  Serial.println(httpCode);

  //Getting response from google sheets
  String payload;
  if(httpCode > 0)
  {
    payload = http.getString();
    Serial.println("Payload : " + payload);
  }

  http.end();
  Serial.println("-------------");
  }
}

void loop() {
  Blynk.run();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    reading = analogRead(flexPin);
    process(reading);
  }

}

void process(int reading)
{
    Blynk.virtualWrite(V0, reading);
    for(int i = 0; i < 30; i++)
    {

    }

    Serial.print("Sensor: ");
    Serial.println(reading);

    if (Path_Switch_State) {
      // WIFI PATHWAY
      sendData(reading);
    } else {
      // ESPNOW PATHWAY
      myData.Sensor_Value = reading;
      // Send message via esp_now
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(myData));
      if (result == ESP_OK) {
        Serial.println(" Sending Confirmed ");
      } else {
        Serial.println(" Sending Error ");
      }
    }
}


void blynkSetup(){

  Blynk.begin(auth, ssid, pwd, "blynk.cloud", 8080);//"192.168.137.148",80 
}

void wifiSetup(){

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);

  Serial.println("Started");
  Serial.print("Connecting....");

  int connecting_process_timed_out = 20;//20 seconds
  connecting_process_timed_out = connecting_process_timed_out * 2;//40 for calc.
  //if it seems that it cannot be connected to Wifi within 20seconds, the ESP32 will restart
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0)
      connecting_process_timed_out--;
    else if(connecting_process_timed_out == 0)
    {
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println("WiFi ready to go");
}

void espnowSetup(){
  
  //initialize esp_now
  if(esp_now_init() != ESP_OK)
  {
    Serial.println("Error intializing esp_now");
    return;
  }

  //register the send call back, a function is called back whenever the data is sent
  esp_now_register_send_cb(onDataSent);

  //Register Peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //Add peer
  if(esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.print("Failed to add peer");
    return;
  }

  Serial.println("Esp check");
}


BLYNK_WRITE(V1){
  Path_Switch_State = param.asInt();
  if(Path_Switch_State)
  {
    Serial.println("-----------------Current Path : GoogleSpreadSheet - Machine Learning Route-------------------");
  }
  else
  {
    Serial.println("-----------------Current Path : ESPNOW Route-------------------------------------------------");
  }
}

