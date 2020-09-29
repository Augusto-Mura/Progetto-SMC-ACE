//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------LIBRERIE----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h> // for ESP8266 Board
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
// Library for interacting with the Telegram API
// Search for "Telegram" in the Library manager and install UniversalTelegramBot by Brian Lough
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
// Library used for parsing Json from the API responses
// NOTE: There is a breaking change in the 6.x.x version,
// install the 5.13.5 version instead


//-------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------COSTANT&DEFINITION-----------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
char ssid[] = "FASTWEBRU";         // network SSID (name)
char password[] = "fastwebruda"; // network password
#define TELEGRAM_BOT_TOKEN "1365422019:AAEAjwqsaY3zhMjLUULraW3KsWCXGu5SPwQ"  // Telegram Bot Token 

#define wled D4 // WiFi status led
#define alertPin D5 //pin di alert
#define wrn0 D1 //bit led n1, o bit per indicare il codice dello step
#define wrn1 D6 //bit led n2
#define wrn2 D7 //bit led n3
#define wrn3 D2 //bit n1 di tipologia di guasto 
#define wrn4 D8 //bit n2 di tipologia di guasto 

byte warning = 0x00;   // stringa di bit che compone il messaggio
int polling = 0; //variabile che se passa alto indica un evento di warning    
String idallarm = "null";//id al quale mandare gli allarmi, viene deciso nel void setup

String ids[10]; // array di id, ogni id indica una chat


WiFiClientSecure client;// Wifi client che supporta HTTPS
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);


//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------FUNZIONI----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void wifiConnection(){   
  // WiFi impostato in station mode e disconnetti da altri router(solo se precedentemente connesso)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  // connessione al WiFi:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //si esce dal while quando la connessione è avvunuta con successo
    digitalWrite(wled, HIGH); //led status WiFi spento
    Serial.print(".");
    delay(500);
  }
  digitalWrite(wled, LOW);//led status WiFi acceso 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //stampa l'ip locale 
  client.setInsecure();
  
  bot.longPoll = 60;// costante utilizzata per migliorare i tempi di risposta del bot telegram
  }

void sendAlertTelegram(String id){//funzione che interpreta la combinazione e manda il messaggio a id
    int guasto = (warning & 0x18)>>3; // filtra il codice di guasto (& 00011000), e lo shifta a destra di 3 pos
    int idLED = (warning & 0x07); // filtra i 3 bit utilizzati per l'indice del led o per il codice dello STEP
    switch(guasto){      
        case(0):
        {String txt = "GUASTO PERTINENTE nel LED n° "+ String(idLED+1);
        bot.sendMessage(String(id), txt);}
        break;        
        case(1):
        {String txt = "GUASTO NON PERTINENTE nel LED n° " + String(idLED+1);
        bot.sendMessage(String(id), txt);}
        break;        
        case(2):
        bot.sendMessage(String(id), "FINE STEP " + String(idLED) + "\n1)Premi il punsalte \n2)Cambia la corrente su i LED \n3)Premi nuovamente il pulsante"); 
        // (usiamo idLED per indicare in che STEP siamo)
        break;        
        case(3):
        bot.sendMessage(String(id), "FINE PROVA (tutti i LED hanno manifestato un guasto).");
        break;
        default:
        bot.sendMessage(String(id), "ERRORE COMBINAZIONE warning");
        break;
    }
}

void readPin(){ //Funzione che crea la variabile warning leggendo i valori in input 
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
  Serial.begin(115200); // apri il monitor seriale
  
  // set Input e Output
  pinMode(wled, OUTPUT);
  digitalWrite(wled, HIGH);
  
  pinMode(alertPin, INPUT); //ALERT PIN = D5
  pinMode(wrn0, INPUT);   
  pinMode(wrn1, INPUT);
  pinMode(wrn2, INPUT);   
  pinMode(wrn3, INPUT);

  ids[0] = "291655246" ; // roberto (usato in debug)
  ids[1] = "-1001219067722"; // canale per gli allarmi sulle prove
  ids[2] = "000000000" ; // zeri (usato in debug)
  idallarm = ids[1];    // settare qui a quale id bisogna mandare gli allarmi
  wifiConnection(); //connessione al WiFI
  bot.sendMessage(String(idallarm), "BOT ATTIVO."); //primo messaggio che indica che il bot è in funzione
}


//-------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------LOOP---------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  delay(100); // 100 millisecondi
  polling = digitalRead(alertPin);
  
  if (polling) {
    readPin();
    sendAlertTelegram(String(idallarm));
    delay(1000); // 1 secondo per evitare di leggere più volte lo stesso alert dall'STM32
    }
}
