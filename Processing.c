
/*LAST EDIT: ELIAS 
 FEB 19 2013
 VIEW README
 */

import processing.serial.*;

Serial myPort;                     // Create object from Serial class
PImage img;                        // Image 
int val;                           // Data received from the serial port
int cols = 256;                     // Number of pixels in a row
int rows = 256;                     // Number of pixels in a column
int[][] dataIn = new int[cols+1][3];   //8 bytes data plus a header, r=[][0] g=[][1] b=[][2]
int count = 0;                     //counts to see if fullR is full
int rgbCount = 0; 
int y=0;                           //y value of pixels
boolean fullR = false;             //condition if dataIn is full
boolean once = true;               //boolean to send only 8 pixels
boolean ack = false;               //turns true when ack is received from arduino (87)


void setup() 
{
  size(1300, 1000);
  img = (loadImage("pic4.jpg"));

  /*//Use this to create your own pic of a specific colour to test individual RGB vals
   img = createImage(cols,rows,RGB); // 
   for(int i = 0; i < img.pixels.length; i++) {
   img.pixels[i] = color(0, 0, 255); 
   }
   */

  loadPixels(); 
  System.out.println(img.pixels.length);
  String portName = "COM4";
  myPort = new Serial(this, portName, 9600);
  background(0);
  dataIn[0][0] = 0;

  //send ack every 2 seconds, wait for ack back (see serial event
  //this begins negotiation between machines
  //reception of the ack is handled by serial interrupt method below
  while (!ack) {  
    myPort.write(88); 
    delay(2000);
  }
    ack = true;
}//end setup


/*The main loop*/
void draw()
{      
  //write X pixel data to serial
  //for (int y=0;y<48;y++) {   ***Don't actually need to increment y right now b/c it is incremented
  //                              the correct amount below in the fullR loop -- which is convenient!   
  if (ack) {
    myPort.write(101);
    for (int x=0;x<cols;x++) {
      //get pixel, extract rgb vals from pixel using bit shifting (faster)
      color rgb = img.pixels[y*cols + x];
      int r = rgb >> 16 & 0xFF; //Equivalent to, but faster than red(rgb)
      int g = rgb >> 8 & 0xFF;
      int b = rgb & 0xFF;
      myPort.write(r);
      myPort.write(g);
      myPort.write(b);
      println("Sent " + x);
      //delay(50);
    }
    // for (int x=0;x<10;x++)
    //      myPort.write(255);
    //set this to wait for another request 
    ack = false;
  }
  //if a full packet has been received, print to screen
  if (fullR)
  { 
    for (int i=1; i<dataIn.length; i++)
    {    
      System.out.println("R " + dataIn[i][0] + " G " + dataIn[i][1] + " B" + dataIn[i][2]);
    }  
    fullR = false;
    for (int x=1; x<dataIn.length; x++)
    {
      fill(dataIn[x][0], dataIn[x][1], dataIn[x][2]);
      //stroke(255);
      rect((x-1)*5, y*5, 5, 5);
    }
    y+=1;
    System.out.println(y);
  }
}//end loop

/*Method is triggered when there is byte on serial port */
void serialEvent(Serial thisPort) {
  if (thisPort == myPort)
  {
    // variable to hold the data on bus      
    int inByte = thisPort.read();

    if (dataIn[0][0] == 100)//if receive header has been received
    {
      /*Put inByte into array
       RGBcount - keeps track of which value (r,g, or b) is to have the value
       - loops after 3 (0,1,2)
       Once 8 pixels have been received (24 rgb vals), fullR=true. Get ready to receive back
       */
      dataIn[count][rgbCount] = inByte;
      System.out.println(inByte + "in Count: " + count + " RGB: " + rgbCount);    
      rgbCount = (rgbCount+1)%3;
      if (rgbCount == 0)
        count++;          
      if (count >= dataIn.length)
      {
        fullR = true;
        System.out.println("ONE ROW");
        dataIn[0][0] = 0;
      }
    }
    else if (inByte == 100)//header character to indicate transmission of a pixel
    {
      System.out.println("Header received");
      dataIn[0][0] = inByte;
      count=1;
    }
    else if (inByte == 87 && dataIn[0][0] == 0)//ack received from arduino && waiting for ack; 
    {
      //we will only receive ack to start transmission if we are done receiving
      //when finished receiving, dataIn is set to 0;
      //set ack = true to start transmission
      //technically, checking dataIn is irrelavent b/c it is checked before inbyte is checked for 87,
      // but we are preventing future challenges if we rearrange the order of this cascading if statement
      ack = true;
      System.out.println("Ack 87 received");
    }
     else if (inByte == 89 && dataIn[0][0] == 0)//ack received from arduino && waiting for ack; 
    {
      //we will only receive ack to start transmission if we are done receiving
      //when finished receiving, dataIn is set to 0;
      //set ack = true to start transmission
      //technically, checking dataIn is irrelavent b/c it is checked before inbyte is checked for 87,
      // but we are preventing future challenges if we rearrange the order of this cascading if statement
      myPort.write(90);
      ack = true;
      System.out.println("Ack 90 received");
    }
  }
}//end serialEvent   
