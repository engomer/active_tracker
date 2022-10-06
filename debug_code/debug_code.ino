int latt, longg, gnssnum, glonassnum;

void sendSingleCommand(String cmd, int delay_time)
{
    Serial1.println(cmd);
    delay(delay_time);
    Serial.flush();
    Serial1.flush();
}


void showMessage()
{
   Serial1.flush();
   Serial.flush();
   while(Serial1.available())
   {
      Serial.println(Serial1.readStringUntil('\n'));
   }
}

void powerTrigger()
{
    Serial.println("Device is enabling..");
    digitalWrite(12, HIGH);
    delay(3000);
    digitalWrite(12,LOW);
}

void GPRSConfig()
{
    powerTrigger();
    delay(3000);
    sendSingleCommand("AT", 10);
    //delay(3000);
    showMessage();
    showMessage();

    Serial.println();
   // delay(5000);
    showMessage();
    delay(300);
    sendSingleCommand("AT+CGNSPWR=1", 100);
    //delay(2000);
    showMessage();
    delay(300);
    sendSingleCommand("AT+CGNSSEQ=\"GSV\"", 10);
    //delay(2000);
    //sendCommand("AT+CGNSINF", 10);
    showMessage();
    //sendCommand("AT+CGNSINF", 10);
    showMessage();
    //sendCommand("AT+CGNSINF", 10);
    showMessage();
    sendSingleCommand("AT+CGNSINF", 10);
    showMessage();
    //delay(5000);
    showMessage();
    //delay(10000);
}

String getSplittedValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int getGPS()
{

  Serial1.println("AT+CGNSINF");
  delay(350); //500
  Serial1.flush();
  Serial.flush();
  //delay(500);
  
  while(Serial1.available())
    {
        String tempAns = Serial1.readString();
       // Serial.println(tempAns);

        if(getSplittedValue(tempAns,',', 1).equals("1"))
        {
            String latS = getSplittedValue(tempAns,',',3);
            String longS = getSplittedValue(tempAns,',',4);
            String gnss = getSplittedValue(tempAns,',',15);
            String glonass = getSplittedValue(tempAns,',',16);
            
            latt = latS.toFloat();
            longg = longS.toFloat();
            gnssnum = gnss.toInt();
            glonassnum = glonass.toInt();

            Serial.print("GPS Updated: ");
            Serial.print(latt);
            Serial.print(",");
            Serial.print(longg);
            Serial.print(",");
            Serial.print(gnssnum);
            Serial.print(",");
            Serial.print(glonassnum);
            Serial.println();
            return 1;
        }
        else
        {
            Serial.println("Cannot take GPS Signal");
            return 0;
        }
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(12, OUTPUT);
  

  delay(2000);
  GPRSConfig();
  delay(3000);
  int resp = getGPS();
    while(resp!= 1)
    {
      resp = getGPS();
      delay(1000);
    }
  

  Serial.println("Started");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available())
  {
    Serial1.println(Serial.readString());
  }

  while(Serial1.available())
  {
    Serial.println(Serial1.readString());
  }


}
