#define interruptpin D5 //pin di interrupt
#define wrn0 D1 //bit led n1
#define wrn1 D6 //bit led n2
#define wrn2 D7 //bit led n3
#define wrn3 D3 //bit n1 di tipologia di guasto 
#define wrn4 D8 //bit n2 di tipologia di guasto 

byte warning[17]; 


void setup() {
  // set IO 
  pinMode(interruptpin, OUTPUT); //INTERRUPT PIN = D5
  digitalWrite(interruptpin, LOW);
  pinMode(wrn0, OUTPUT);   
  pinMode(wrn1, OUTPUT);
  pinMode(wrn2, OUTPUT);   
  pinMode(wrn3, OUTPUT);
//Pertinenti:       Non pertinenti:
  warning[0]= 0x00; warning[8]=  0x08;
  warning[1]= 0x01; warning[9]=  0x09;
  warning[2]= 0x02; warning[10]= 0x0A;
  warning[3]= 0x03; warning[11]= 0x0B;
  warning[4]= 0x04; warning[12]= 0x0C;
  warning[5]= 0x05; warning[13]= 0x0D;
  warning[6]= 0x06; warning[14]= 0x0E;
  warning[7]= 0x07; warning[15]= 0x0F;
//Fine step:  
  warning[16]= 0x10;
//Tutti rotti:  
  warning[17]= 0x18;
  delay(2000);
}

void loop() {
  for(int i=0; i<18; i++){
    digitalWrite(wrn0, bitRead(warning[i], 0));
    digitalWrite(wrn1, bitRead(warning[i], 1));
    digitalWrite(wrn2, bitRead(warning[i], 2));
    digitalWrite(wrn3, bitRead(warning[i], 3));
    digitalWrite(wrn4, bitRead(warning[i], 4));

    digitalWrite(interruptpin, HIGH);
    delay(1000);
    digitalWrite(interruptpin, LOW);
    delay(10000);
  }
}
