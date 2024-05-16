#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN "2WkSCDZKE-Zr0yrlMoqtgxDPXxQN2g8V"
#define BLYNK_TEMPLATE_ID "TMPL3_4ZVHEt9"
#define BLYNK_TEMPLATE_NAME "Flex Sensor Serial Monitor"
#define servoPin 0

#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<BlynkSimpleEsp8266.h>
#include<espnow.h>
#include<ArduinoJson.h>
#include<Servo.h>

//blynk
char auth[] = BLYNK_AUTH_TOKEN;

//wifi
const char *ssid = "Redmi 9 Power";
const char *pwd = "mnbvcxz123";

//google spreadsheet
String Google_Script_Url = "https://script.google.com/macros/s/AKfycbxymW_vNewZmTpuN4e00qsw3od5n2YVhhzqzKfArWFjtRVImOuNW_0RBVQbUJ0Ev3ZcrA/exec";

//espnow
struct message{
  int Sensor_Value;
};
struct message myData;

//general
Servo myServo;
int servoAngle = 0;
int Path_Switch_State = 0;
bool dState = false;
unsigned long previousMillis = 0;
const long interval = 1000;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Sensor: ");
  Serial.println(myData.Sensor_Value);

  if(myData.Sensor_Value < 390)
  {
    servoAngle = 1900;
  }
  else
  {
    servoAngle = 800;
  }

  myServo.writeMicroseconds(servoAngle);
  delay(200);  
}

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  blynkSetup();
  wifiSetup();
  espSetup();
}

void loop() {
  Blynk.run();
  if(Path_Switch_State)
  {
    getDatafromSheets();
    if(dState)
    {
      servoAngle = 1900;
    }
    else
    {
      servoAngle = 800;
    }
    myServo.writeMicroseconds(servoAngle);
    delay(1000);
  }
  else
  {
    
  }
}

void blynkSetup(){

  Blynk.begin(auth, ssid, pwd, "blynk.cloud", 8080);//"192.168.137.148",80 
}

void wifiSetup()
{
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

void espSetup()
{
  //Init Esp - now
  if(esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);  
}

void getDatafromSheets()
{
  WiFiClient client;

  if(WiFi.status() == WL_CONNECTED && client.connect(Google_Script_Url, 443))
  {
    HTTPClient http;

    String url = Google_Script_Url + "?sts=read";
    Serial.println(url);

    http.begin(client, url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpStatusCode = http.GET();

    if(httpStatusCode == HTTP_CODE_OK )
    {
      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      dState = doc[0];

      Serial.println("dState: " + String(dState));
    }
    else
    {
      Serial.println("Error fetching Data ");
    }

    http.end();
    Serial.println("---------------------------------------");
  }
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

BLYNK_WRITE(V0){
  int Sensor_Value = param.asInt();
  Serial.println("Sensor: " + String(Sensor_Value));

  if(Sensor_Value > 1000)
  {
    servoAngle = 1900;
  }
  else
  {
    servoAngle = 800;
  }

  myServo.writeMicroseconds(servoAngle);
  delay(200);  
}

