#include <ESP8266WiFi.h>
#include <BasicTelegram.h>
 
#define STASSID "your-ssid"
#define STAPSK "your-password"
#define CHAT_ID 9999999999
#define TELEGRAM_TOKEN "your-bot-token"

const char* ssid = STASSID;
const char* password = STAPSK;
int chatId = CHAT_ID;
char* token = TELEGRAM_TOKEN;
TelegramClient client;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // It has to be created after WiFi connection
  client.setup(token);
  


}
 
void loop(){
    // Trying to send a message
  if(client.sendMessage(chatId, "Test Message")){
    Serial.println("Message has been received");
  }else{
    Serial.println("Message couldn't be received");  }
    delay(10000);
}

