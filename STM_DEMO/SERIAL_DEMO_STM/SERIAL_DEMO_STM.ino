
HardwareSerial Seriall(USART3);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Seriall.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

    Serial.print("Message : ");
    Seriall.println("AT");
    while(Seriall.available())
    {
        Serial.print(Seriall.readString());
    }
  
    Serial.print("\n");
}
