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
char ssid[] = "cuginet";         // your network SSID (name)
char password[] = "cugicugimilitari"; // your network password
#define TELEGRAM_BOT_TOKEN "1365422019:AAEAjwqsaY3zhMjLUULraW3KsWCXGu5SPwQ"  // Your Telegram Bot Token 

#define wled D4
#define interruptpin D5 //pin di interrupt
#define wrn0 D1 //bit led n1
#define wrn1 D6 //bit led n2
#define wrn2 D7 //bit led n3
#define wrn3 D2 //bit n1 di tipologia di guasto 
#define wrn4 D8 //bit n2 di tipologia di guasto 

byte warning = 0x01;   // es byte = 00000001
int navvertimento = 0; //quanti avvertimenti sono  arrivati
int cntEvent = 0; 
int polling = 0; //segnale che se passa alto indica un evento di warning    
String idallarm = "null";//id al quale mandare gli allarmi, si setta nel void setup

String ids[10];
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
//-------------------------------------------------------FUNZIONI----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void wifiConnection(){   //LASCIATE COSì QUESTA FUNZIONE, NON TOCCARE!
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

void interpretaEavverti(String id){//funzione che capisce che combinaziona è arrivata e manda il messaggio a id
    int guasto = (warning & 0x18)>>3; //shift codice guasto
    int idLED = (warning & 0x07);
    switch(guasto){      
        case(0):
        {String txt = "GUASTO PERTINENTE nel led n° " + String(idLED+1);
        bot.sendMessage(String(id), txt);}
        break;        
        case(1):
        {String txt = "GUASTO NON PERTINENTE nel led n° " + String(idLED+1);
        bot.sendMessage(String(id), txt);}
        break;        
        case(2): //00000010
        bot.sendMessage(String(id), "FINE STEP " + String(idLED) + "(vai e riattacca tutto)");// usiamo idled per sapere in che step siamo, aggiungi cose da fare{}
        break;        
        case(3):
        bot.sendMessage(String(id), "TUTTI I LED SI SONO ROTTI");
        break;
        default:
        bot.sendMessage(String(id), "ERRORE COMBINAZIONE");
        break;
    }
}

void readpin(){                   // CREA LA VARIABILE WEARNING LEGGENDO IL VALORE DEI BIT DEL LED E DEL GUASTO
    bitWrite(warning, 0, digitalRead(wrn0));
    bitWrite(warning, 1, digitalRead(wrn1));
    bitWrite(warning, 2, digitalRead(wrn2));
    bitWrite(warning, 3, digitalRead(wrn3));
    bitWrite(warning, 4, digitalRead(wrn4));
}


//-------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------SETUP------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  
  // set IO 
  pinMode(wled, OUTPUT);
  digitalWrite(wled, HIGH);
  
  pinMode(interruptpin, INPUT); //INTERRUPT PIN = D5
  pinMode(wrn0, INPUT);   
  pinMode(wrn1, INPUT);
  pinMode(wrn2, INPUT);   
  pinMode(wrn3, INPUT);

  ids[0] = "291655246" ; //robbe
  ids[1] = "196098030" ; //augu
  ids[2] = "134892235" ; //danjo
  ids[3] = "-1001219067722"; //canale per gli allarmi sulle prove
  ids[5] = "000000000" ; //zeri
  idallarm = ids[3];    // <-----------------------------------------settare qui a quale id bisogna mandare gli allarmi
  wifiConnection(); //esegue tutte le istruzioni che ho commentato sopra
  bot.sendMessage(String(idallarm), "BOT ATTIVO");
}


//-------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------LOOP---------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  delay(100); //100 millisecondi
  polling = digitalRead(interruptpin);
  
  if (polling) {
    readpin();
    interpretaEavverti(String(idallarm));
    delay(1100); //metto 5 secondi per fare i test a mano, poi ri metteremo 1100 millisecondi
    }
}
