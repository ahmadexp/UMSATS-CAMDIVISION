/*LAST EDIT: ELIAS 
 FEB 19 2013
 VIEW README
 */

const int cols = 256;
int rows = 48;
int dataIn[cols+1][3];
int count = 0;
int rgbCount = 0;
boolean fullR = false;
boolean ack = false;
void setup()
{
  Serial.begin(9600);
}

//Main loop
void loop()
{  
  if (fullR)//if a full packet has been received
  {
    /*Send a full row back:
     - Send a header first
     - Then loop through all pixels, sending rgb of each one
     - indicate end of transmission by setting fullR to false
     - then send request for another row
     */
    Serial.write(100);//send header 
    Serial.flush();  
    for(int i=1; i<cols+1; i++)
    {    
      Serial.write(dataIn[i][0]);
      Serial.write(dataIn[i][1]);
      Serial.write(dataIn[i][2]);    
    }  
    fullR = false;
    ack = false;
    dataIn[0][0] = 0;
    delay(200);
   
    while(Serial.read() != 90){  
      Serial.write(89); 
      delay(10);
    } 
  }
}//end Main loop

/*Method is triggered when there is byte on serial port */
void serialEvent() {
  int inByte = Serial.read();

  if(dataIn[0][0] == 101) {
    /*Put inByte into array
     RGBcount - keeps track of which value (r,g, or b) is to have the value
     - loops after 3 (0,1,2)
     Once A pixels have been received (24 rgb vals), fullR=true. Get ready to receive back
     */
    dataIn[count][rgbCount] = inByte;    
    rgbCount = (rgbCount+1)%3;
    if(rgbCount == 0)
      count++;          
    if (count > cols)
    {
      fullR = true;
      dataIn[0][0] = 0;
    }
  }
  else if(inByte==101){//receive header
    dataIn[0][0] = inByte;
    count=1;
    rgbCount=0;
  }
  else if(inByte == 88){
    Serial.write(87);
  }//end else if 
  else if(inByte == 90){
    ack = true;
  }//end else if 
}//end serialEvent




