//-------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------- GLOBAL
//-------------------------------------------------------------------------------------------------------------------------
#include "CTBot.h"

//-------------------------------------------------------------------------------------------------------------------------DEFINIZIONI e INIZIALIZZAZIONE INGRESSI E USCITE:
#define wled D4
#define led D0
#define interruptpin D5 //pin di interrupt
#define wrn0 D1 //bit led n1
#define wrn1 D6 //bit led n2
#define wrn2 D7 //bit led n3
#define wrn3 D3 //bit di tipo messaggio (ne aggiungeremo un altro)

#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed
#define STATUS "status"

CTBot myBot;
CTBotInlineKeyboard myKbd;

int ids[10];
//es:
// ids [0] = 123456789  //roberto 
// ids [1] = 987783423  //danjo

String ssid  = "cuginet"; // REPLACE mySSID WITH YOUR WIFI SSID: TISCALI-Wifi
String passw = "cugicugimilitari"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "904198135:AAE5jqQQeAYaUNbluG22HL_feCuPbv2RYvU"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
byte warning = 0x01;   // es byte = 00000001
int navvertimento = 0; //quanti avvertimenti sono  arrivati
int cntEvent = 0;      

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------FUNZIONI
//-------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------INTERRUPT HANDLER:

void interpretaEavverti(int id){                       //funzione che capisce che combinaziona è stata mandata e manda il messaggio a id
    switch(warning){
        case(0x0):
        myBot.sendMessage(id, "si è spento il led 1");
        break;
        case(0x1):
        myBot.sendMessage(id, "combinazione 1");
        break;
        case(0x2):
        myBot.sendMessage(id, "combinazione 2");
        break;
        case(0x3):
        myBot.sendMessage(id, "combinazione 3");
        break;
        case(0x4):
        myBot.sendMessage(id, "combinazione 4");
        break;
        //-------aggiungere gli altri
        default:
        myBot.sendMessage(id, "è arrivata la combinazione di default");
        break;
    }
}

void readpin(){
    bitWrite(warning, 0, digitalRead(wrn0));
    bitWrite(warning, 1, digitalRead(wrn1));
    bitWrite(warning, 2, digitalRead(wrn2));
    bitWrite(warning, 3, digitalRead(wrn3));
}

void status(int id){
    readpin();
    interpretaEavverti(id);
    myBot.sendMessage(id, "qua dovrei mandare altre opzioni di sistema");
}

void rispondi(TBMessage msg){
    // check what kind of message I received
    if (msg.messageType == CTBotMessageText) {
        // received a text message
        if (msg.text.equalsIgnoreCase("/Comandi")) {
            // the user is asking to show the inline keyboard --> show it
            myBot.sendMessage(msg.sender.id, "Inline Keyboard", myKbd);
        }
        else {
            // the user write anithing else --> show a hint message
            myBot.sendMessage(msg.sender.id, "Try '/Comandi'");
        }
    }   
    else if (msg.messageType == CTBotMessageQuery) {
        // received a callback query message
        if (msg.callbackQueryData.equals(LIGHT_ON_CALLBACK)) {
            // pushed "LIGHT ON" button...
            digitalWrite(led, LOW); // ...turn on the LED (inverted logic!) 
            // terminate the callback with an alert message
            myBot.endQuery(msg.callbackQueryID, "Light is on", true);
        }  
        else if (msg.callbackQueryData.equals(LIGHT_OFF_CALLBACK)) {
            // pushed "LIGHT OFF" button...
            digitalWrite(led, HIGH); // ...turn off the LED (inverted logic!)
            // terminate the callback with a popup message
            myBot.endQuery(msg.callbackQueryID, "Light is off");
        }
        else if (msg.callbackQueryData.equals(STATUS)) {
            myBot.endQuery(msg.callbackQueryID, "monitoraggio sistema...");
            status(msg.sender.id);
        }
    }
}

ICACHE_RAM_ATTR void interruptfunction() {
  myBot.wifiConnect(ssid, passw); //connettiti al wifi
  String txt = "ATTENZIONE! avvertimento n°" + String(cntEvent+1);  
  cntEvent++;
  readpin();
  for(int i = 0; i<3; i++){
    myBot.sendMessage(ids[i], txt);
    interpretaEavverti(ids[i]);
  }
  Serial.println(txt);
  Serial.println("avvertimento n %d arrivato!");
  //while(pollingPin){}
}



//-------------------------------------------------------------------------------------------------------------------------SETUP:
void setup() {
    // initialize the Serial
    Serial.begin(115200);
    
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

    Serial.println("Starting TelegramBot...");

    // connect the ESP8266 to the desired access point
    myBot.wifiConnect(ssid, passw);
 
    // set the telegram bot token
    myBot.setTelegramToken(token);
    
    // check if all things are ok
    if (myBot.testConnection()){
        Serial.println("\ntestConnection OK");
        digitalWrite(wled, LOW);   //IL LED è ATTIVO BASSO! LOW = ACCESO
    } 
    else {
        Serial.println("\ntestConnection NOK");
        digitalWrite(wled, HIGH); //HIGH = SPENTO
    }
    myBot.sendMessage(ids[0], "Avviamento...");

    // inline keyboard customization
    // add a query button to the first row of the inline keyboard
    myKbd.addButton("LIGHT ON", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
    // add another query button to the first row of the inline keyboard
    myKbd.addButton("LIGHT OFF", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
    // add a new empty button row
    myKbd.addRow();
    myKbd.addButton("STATUS SISTEMA", STATUS, CTBotKeyboardButtonQuery);
}



//-------------------------------------------------------------------------------------------------------------------------LOOP:
void loop() {    //while (1)
    TBMessage msg; //msg è un pacchetto relativo a un messaggio, contiene: id,nome,testo,data,ora,tipo di messaggio.
    if (myBot.getNewMessage(msg)) rispondi(msg);
    //if (digitalRead(pollingPin)) polling();
    delay(1000);
}
