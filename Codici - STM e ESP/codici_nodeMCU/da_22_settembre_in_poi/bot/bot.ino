#include <ESP8266WiFi.h> // for ESP8266 Board
//#include <ESP8266WiFi.h>//Use for eso32 Board
#include <WiFiClientSecure.h>

#include <UniversalTelegramBot.h>
// Library for interacting with the Telegram API
// Search for "Telegegram" in the Library manager and install UniversalTelegramBot by Brian Lough
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot


#include <ArduinoJson.h>
// Library used for parsing Json from the API responses
// NOTE: There is a breaking change in the 6.x.x version,
// install the 5.13.5 version instead
// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------
char ssid[] = "FASTWEBRU";         // your network SSID (name)
char password[] = "fastwebruda"; // your network password
#define TELEGRAM_BOT_TOKEN "904198135:AAE5jqQQeAYaUNbluG22HL_feCuPbv2RYvU"  // Your Telegram Bot Token 


#define wled D4
#define led D0

#define interruptpin D5 //pin di interrupt
#define wrn0 D1 //bit led n1
#define wrn1 D6 //bit led n2
#define wrn2 D7 //bit led n3
#define wrn3 D3 //bit n1 di tipologia di guasto 
#define wrn4 D8 //bit n2 di tipologia di guasto 

byte warning = 0x01;   // es byte = 00000001
int navvertimento = 0; //quanti avvertimenti sono  arrivati
int cntEvent = 0;     


int ids[10];
//es:
// ids [0] = 123456789  //roberto 
// ids [1] = 987783423  //danjo
//------- ---------------------- ------

// This is the Wifi client that supports HTTPS
WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

int delayBetweenChecks = 1000;
unsigned long lastTimeChecked;   //last time messages' scan has been done

unsigned long lightTimerExpires;
boolean lightTimerActive = false;
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------FUNZIONI
//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------INTERRUPT HANDLER:
void wifiConnection(){
  // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);


    // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

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

  
void interpretaEavverti(int id){                       //funzione che capisce che combinaziona è arrivata e manda il messaggio a id
    int guasto = (warning & 0x18)>>3; //shift codice guasto
    int idLED = (warning & 0x07);
    switch(guasto){      
        case(0):
        String txt = "GUASTO PERTINENTE nel led n° " + String(idLED+1);
        bot.sendMessage(String(id), txt);
        break;        
        case(1):
        String txt = "GUASTO NON PERTINENTE nel led n° " + String(idLED+1);
        myBot.sendMessage(String(id), txt);
        break;        
        case(2): //00000010
        bot.sendMessage(String(id), "FINE STEP (vai e riattacca tutto)");    //aggiungi cose da fare
        break;        
        case(3):
        {
        bot.sendMessage(String(id), "TUTTI I LED SI SONO ROTTI");
        break;
        }
        /*
        default:
        bot.sendMessage(String(id), "ERRORE COMBINAZIONE");
        break;
        */
    }
}



void readpin(){
    bitWrite(warning, 0, digitalRead(wrn0));
    bitWrite(warning, 1, digitalRead(wrn1));
    bitWrite(warning, 2, digitalRead(wrn2));
    bitWrite(warning, 3, digitalRead(wrn3));
    bitWrite(warning, 4, digitalRead(wrn4));
}




ICACHE_RAM_ATTR void interruptfunction() {
  wifiConnection(); //connettiti al wifi
  String txt = "ATTENZIONE!(interrupt) avvertimento n°" + String(cntEvent+1);  
  cntEvent++;
  readpin();
  bot.sendMessage(String(ids[0]), txt);
  interpretaEavverti(ids[i]);

  Serial.println(txt);
  Serial.println("avvertimento n %d arrivato!");
  //while(pollingPin){}
}



void setup() 
{
  Serial.begin(115200);
  /*
 // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

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
*/
  wifiConnection(); //esegue tutte le istruzioni che ho commentato sopra
 
  
  // set IO & interrupt
  pinMode(wled, OUTPUT);
  pinMode(led, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptpin), interruptfunction, RISING); //INTERRUPT PIN = D5 
  pinMode(wrn0, INPUT);   
  pinMode(wrn1, INPUT);
  pinMode(wrn2, INPUT);   
  pinMode(wrn3, INPUT);

  ids[0] = 291655246 ; //robbe
  ids[1] = 196098030 ; //augu
  ids[2] = 134892235 ; //danjo
  ids[3] = -286862230; //le magiche avventure al diee
  ids[4] = -347402800; //gruppo bots
  ids[5] = 000000000 ; //zeri    
  bot.sendMessage(String(ids[0]), "tutto ok con la connessione della NodeMCU e con il bot Telegram");

}


void handleNewMessages(int numNewMessages)
{for (int i = 0; i < numNewMessages; i++) 
  {// If the type is a "callback_query", a inline keyboard button was pressed
    if (bot.messages[i].type ==  F("callback_query")) 
   {String text = bot.messages[i].text;
    Serial.print("Call back button pressed with text: ");
    Serial.println(text);

      if (text == F("ON1")) {digitalWrite(LED_PIN1, LOW);}
      else if (text == F("OFF1")) { digitalWrite(LED_PIN1, HIGH);}
      if (text == F("ON2")) { digitalWrite(LED_PIN2, LOW);}
      else if (text == F("OFF2")) { digitalWrite(LED_PIN2, HIGH);} 
      if (text == F("ON3")) {digitalWrite(LED_PIN3, LOW);}
      else if (text == F("OFF3")) { digitalWrite(LED_PIN3, HIGH);} 
      if (text == F("ON4")) {digitalWrite(LED_PIN4, LOW);}
      else if (text == F("OFF4")) {digitalWrite(LED_PIN4, HIGH);} 
  
    } else {
      String chat_id = String(bot.messages[i].chat_id);
      String text = bot.messages[i].text;
  
      

      if (text == F("/options")) 

        // Keyboard Json is an array of arrays.
        // The size of the main array is how many row options the keyboard has
        // The size of the sub arrays is how many coloums that row has

        // "The Text" property is what shows up in the keyboard
        // The "callback_data" property is the text that gets sent when pressed  
        
       { String keyboardJson = F("[[{ \"text\" : \"L1-ON\", \"callback_data\" : \"ON1\" },{ \"text\" : \"L1-OFF\", \"callback_data\" : \"OFF1\" },{ \"text\" : \"L2-ON\", \"callback_data\" : \"ON2\" },{\"text\" : \"L2-OFF\", \"callback_data\" : \"OFF2\"}],");
       keyboardJson += F("[{ \"text\" : \"L3-ON\", \"callback_data\" : \"ON3\" }, { \"text\" : \"L3-OFF\", \"callback_data\" : \"OFF3\" },{ \"text\" : \"L4-ON\", \"callback_data\" : \"ON4\" },{ \"text\" : \"L4-OFF\", \"callback_data\" : \"OFF4\" }]]");
        bot.sendMessageWithInlineKeyboard(chat_id, "Press The Buttons", "", keyboardJson);
      }

      // When a user first uses a bot they will send a "/start" command
      // So this is a good place to let the users know what commands are available
      if (text == F("/start")) 
      

       { bot.sendMessage(chat_id, "/options : returns the inline keyboard\n", "Markdown");}
      
    }
   }
  }


void loop() 

{ if (millis() > lastTimeChecked + delayBetweenChecks)  
  { // getUpdates returns 1 if there is a new message from Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages) {
                         Serial.println("got response");
                         handleNewMessages(numNewMessages);
                         }

    lastTimeChecked = millis();

    if (lightTimerActive && millis() > lightTimerExpires) {
                                                           lightTimerActive = false;
                                                           digitalWrite(LED_PIN1, LOW);
                                                           digitalWrite(LED_PIN2, LOW);
                                                           digitalWrite(LED_PIN3, LOW);
                                                           digitalWrite(LED_PIN4, LOW);
      
                                                           }
  }
}
