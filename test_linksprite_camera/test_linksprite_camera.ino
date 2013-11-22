/* Linksprite */

#include <SoftwareSerial.h>

byte incomingbyte;
SoftwareSerial mySerial(5,6);                     //Configure pin 5 and 6 as soft serial port
int a=0x0000,j=0,k=0,count=0;                    //Read Starting address       
uint8_t MH,ML;
boolean EndFlag=0;
                               
void SendResetCmd();
void SendTakePhotoCmd();
void SendReadDataCmd();
void StopTakePhotoCmd();

void setup()
{ 
  Serial.begin(19200);
  mySerial.begin(115200);
}

void loop() 
{
   Serial.write(mySerial.read());
     SendResetCmd();
     delay(4000);                               //After reset, wait 2-3 second to send take picture command
      StopTakePhotoCmd();  //mael add
      SendTakePhotoCmd();
Serial.println("bla bla debut");
     while(mySerial.available()>0)
      {
        incomingbyte=mySerial.read();
    Serial.print(incomingbyte);
    Serial.println("serial reading ");
      Serial.write(mySerial.read());
      }   
      byte a[32];
      
      while(!EndFlag)
      {  
         j=0;
         k=0;
         count=0;
            Serial.println("sending read ");
         SendReadDataCmd();

         delay(25);
          while(mySerial.available()>0)
          {
            Serial.println("reading on going1 ");
               incomingbyte=mySerial.read();
               k++;
               if((k>5)&&(j<32)&&(!EndFlag))
               {
               a[j]=incomingbyte;
               if((a[j-1]==0xFF)&&(a[j]==0xD9))      //Check if the picture is over
               EndFlag=1;                           
               j++;
	       count++;
Serial.println("reading on going ");
               }
          }
                    Serial.println("reading done ");
          for(j=0;j<count;j++)
          {   if(a[j]<0x10)
              Serial.print("0");
              Serial.print(a[j],HEX);
              Serial.print(" ");
          }                                       //Send jpeg picture over the serial port
          Serial.println();
      }      
     while(1);
}

//Send Reset command
void SendResetCmd()
{
      mySerial.print(0x56);
      mySerial.print(0x00);
      mySerial.print(0x26);
      mySerial.print(0x00);
}

//Send take picture command
void SendTakePhotoCmd()
{
      mySerial.print(0x56);
      mySerial.print(0x00);
      mySerial.print(0x36);
      mySerial.print(0x01);
      mySerial.print(0x00);  
}

//Read data
void SendReadDataCmd()
{
      MH=a/0x100;
      ML=a%0x100;
      mySerial.print(0x56);
      mySerial.print(0x00);
      mySerial.print(0x32);
      mySerial.print(0x0c);
      mySerial.print(0x00); 
      mySerial.print(0x0a);
      mySerial.print(0x00);
      mySerial.print(0x00);
      mySerial.print(00); //mySerial.print(MH);
      mySerial.print(00);  //mySerial.print(ML); 
      mySerial.print(0x00);
      mySerial.print(0x00);
      mySerial.print(0x00);
      mySerial.print(0x20);
      mySerial.print(0x00);  
      mySerial.print(0x0a);
      a+=0x20;                            //address increases 32£¬set according to buffer size
}

void StopTakePhotoCmd()
{
      mySerial.print(0x56);
      mySerial.print(0x00);
      mySerial.print(0x36);
      mySerial.print(0x01);
      mySerial.print(0x03);        
}









