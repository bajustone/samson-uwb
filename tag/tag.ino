#include <SPI.h>
#include <DW1000Ranging.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
#include "link.h"

#include <HTTPClient.h>

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
#define PIN_RST 27
#define PIN_IRQ 34
#define BAUD_RATE 115200
#define PIN_TX 17
#define PIN_RX 16
#define TIME_INTERVAL_TO_SEND_DATA 5000




const char *ssid = "CANALBOX-6E36-2G";
const char *password = "xFNVUMvWL3";

WiFiClient client;
String tagId = "007D";

struct MyLink *uwb_data;
long runtime = 0;
long secondCoreRunTime = 0;
String all_json = "";
bool readyToSend = false;
bool data_sent_to_server = false;
int howManyTimes = 0;
BaseType_t core1TaskHandle = NULL;



String CLOUD_API_ENDPOINT  = "https://us-central1-uwb-sms.cloudfunctions.net/TagLinks";

void setup()
{
    Serial.begin(BAUD_RATE);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    
     delay(2000);
    

    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());

    
    delay(1000);

    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, DW_CS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    
    //we start the module as a tag
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
    
    delay(2000);
    uwb_data = init_link();

    xTaskCreatePinnedToCore(
        send_links_to_cloud,
        "send_data_to_server",
        15000,
        &all_json,
        1,
        NULL,
        1
      );
}

void loop()
{
  DW1000Ranging.loop();

  if ((millis() - runtime) > TIME_INTERVAL_TO_SEND_DATA){
    make_link_json(uwb_data, &all_json);
    if(data_sent_to_server == false){
      data_sent_to_server = true;
    }
    runtime = millis();
  }
}



void send_links_to_cloud(void* params){
  Serial.begin(BAUD_RATE);
  while(true){
    data_sent_to_server = false;
    delay(TIME_INTERVAL_TO_SEND_DATA);

  

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI not found");
    continue;
  }


  HTTPClient http;
  String endpoint = CLOUD_API_ENDPOINT + "?tagId=" + tagId + "&links=" + all_json;
  http.begin(endpoint.c_str());
  int httpResponseCode = http.GET();
  if(httpResponseCode > 0){
    howManyTimes++;
    Serial.print("How many times: ");
    Serial.println(howManyTimes);
    Serial.print("Status code: ");
    Serial.println(httpResponseCode);
    Serial.println("Data sent");
  }else{
    Serial.print("Status code: ");
    Serial.println(httpResponseCode);
    Serial.println("Data not sent");
  }

  }
 
}
void newRange() {
  DW1000Ranging.setReplyTime(uint16_t replyDelayTimeUs)
  fresh_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), DW1000Ranging.getDistantDevice()->getRange(), DW1000Ranging.getDistantDevice()->getRXPower());
}

void newDevice(DW1000Device *device){
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
    

    add_link(uwb_data, device->getShortAddress());
}

void inactiveDevice(DW1000Device *device){
  
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);

  delete_link(uwb_data, device->getShortAddress());
}

void get_gps_coordinates(){

}
// 83 - 58/180
// 53 - 53/150
// 63 - 70/

