//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------LIBRERIE----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h> // for ESP8266 Board
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
// Library for interacting with the Telegram API
// Search for "Telegegram" in the Library manager and install UniversalTelegramBot by Brian Lough
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
// Library used for parsing Json from the API responses
// NOTE: There is a breaking change in the 6.x.x version,
// install the 5.13.5 version instead


//-------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------COSTANT&DEFINITION-----------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//------- Replace the following! ------
char ssid[] = "FASTWEBRU";         // your network SSID (name)
char password[] = "fastwebruda"; // your network password
#define TELEGRAM_BOT_TOKEN "1365422019:AAEAjwqsaY3zhMjLUULraW3KsWCXGu5SPwQ"  // Your Telegram Bot Token 
#define wled D4
//------- ---------------------- ------

// This is the Wifi client that supports HTTPS
WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

int delayBetweenChecks = 1000;
unsigned long lastTimeChecked;   //last time messages' scan has been done

unsigned long lightTimerExpires;
boolean lightTimerActive = false;


//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------FUNZIONI----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

void wifiConnection(){  //LASCIATE COSì QUESTA FUNZIONE, NON TOCCARE!
  // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
    // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wled, HIGH);
    Serial.print(".");
    delay(500);
  }
  digitalWrite(wled, LOW);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Only required on 2.5 Beta
  client.setInsecure();
  // longPoll keeps the request to Telegram open for the given amount of seconds if there are no messages
  // This hugely improves response time of the bot, but is only really suitable for projects
  // where the the initial interaction comes from Telegram as the requests will block the loop for
  // the length of the long poll
  bot.longPoll = 60;  
  }

void handleNewMessages(int numNewMessages){
  for (int i = 0; i < numNewMessages; i++){
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String response = "il tuo chat id è: " + String(chat_id);
    bot.sendMessage(chat_id, response);
    Serial.print("mi hanno scritto: "); 
    Serial.println(text);
    Serial.print("il suo chat id è: :");
    Serial.println(chat_id);
    Serial.println();
    }
  }


//-------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------SETUP------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void setup(){
  Serial.begin(115200);
  pinMode(wled, OUTPUT);
  digitalWrite(wled, HIGH);
  wifiConnection();
  
}


//-------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------LOOP---------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void loop(){ 
  if (millis() > lastTimeChecked + delayBetweenChecks){
    // getUpdates returns 1 if there is a new message from Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    if (numNewMessages) {
      Serial.println("got message");
      handleNewMessages(numNewMessages);
      }
      
    lastTimeChecked = millis();
  }
}
