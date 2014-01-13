/*
 * 
 * by Mael reymond
 * see readme and maeldiy.wordpress.com for more information and inspiration
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * 
 *             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.

Phonedoorv2_0.pde:
This sketch broadcast udp frame to LAN when a discrete input is activated
It then broadcast jpeg images, not done through web server to allow boracast over LAN and optimise frame rate

 created 22 Nov 2013 by mael reymond
 */

#include <Bounce.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Timer.h>
#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h>    
#define chipSelect 4
// Pins for camera connection are configurable.
// With the Arduino Uno, etc., most pins can be used, except for
// those already in use for the SD card (10 through 13 plus
// chipSelect, if other than pin 10).
// With the Arduino Mega, the choices are a bit more involved:
// 1) You can still use SoftwareSerial and connect the camera to
//    a variety of pins...BUT the selection is limited.  The TX
//    pin from the camera (RX on the Arduino, and the first
//    argument to SoftwareSerial()) MUST be one of: 62, 63, 64,
//    65, 66, 67, 68, or 69.  If MEGA_SOFT_SPI is set (and using
//    a conventional Arduino SD shield), pins 50, 51, 52 and 53
//    are also available.  The RX pin from the camera (TX on
//    Arduino, second argument to SoftwareSerial()) can be any
//    pin, again excepting those used by the SD card.
// 2) You can use any of the additional three hardware UARTs on
//    the Mega board (labeled as RX1/TX1, RX2/TX2, RX3,TX3),
//    but must specifically use the two pins defined by that
//    UART; they are not configurable.  In this case, pass the
//    desired Serial object (rather than a SoftwareSerial
//    object) to the VC0706 constructor.

// Using SoftwareSerial (Arduino 1.0+) or NewSoftSerial (Arduino 0023 & prior):
#if ARDUINO >= 100
// On Uno: camera TX connected to pin 2, camera RX to pin 3:
SoftwareSerial cameraconnection = SoftwareSerial(5, 6);
// On Mega: camera TX connected to pin 69 (A15), camera RX to pin 3:
//SoftwareSerial cameraconnection = SoftwareSerial(69, 3);
#else
NewSoftSerial cameraconnection = NewSoftSerial(5, 6);
//Adafruit_VC0706 cam = Adafruit_VC0706(115200); //could be necessary?
#endif

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
//Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);


const int buttonPin = 8;     // the number of the pushbutton pin
const int debug_enable = 1; //debug = 1, not debug =0
// Variables will change:
int ledState = 1;  //reading = 1;        // the current state of the output pin
int buttonState=0;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int reading = 1; // reading allowed or not
//Timer t,MESSAGE_interval;                     // the timer which perform the keepalive every keep_alive_intervall milliseconds

// Instantiate a Bounce object with a 200 millisecond debounce time
Bounce bouncer = Bounce( buttonPin,2000 ); 

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 350;    // the debounce time; increase if the output flickers
unsigned int intervalle, is_MESSAGE_sent =0;
long keep_alive_intervall = 90000, MESSAGE_sending_interval_time = 90000,last_MESSAGE_Time;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
/*byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; */
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x00, 0xF0, 0x6F };
IPAddress ip(192, 168, 0, 177);


IPAddress remoteIPadress_broadcast(192, 168, 0, 255); 

unsigned int localPort = 8888;      // local port to listen on
unsigned int destinationPort = 5650;      // port distant a connecter si necessaire
unsigned int timeout = 20; 
unsigned int ack = 0;  //acknowledge
unsigned int nb_envoi = 0;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet, 
char packetRead[UDP_TX_PACKET_MAX_SIZE]; 
char check[]="OKFROMTAB";
char  MESSAGE[] = "send_request_calls";  
char keep_alive_message[] = "keep_alive_message";
int tick; 
EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP
long debug_value;
     
void setup() {
     Serial.begin(115200);
  // start the Ethernet and UDP:
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);  //enable internal pull up resistor
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);

  //int tick = t.every(100000, keepaliv);
    // When using hardware SPI, the SS pin MUST be set to an
  // output (even if not connected or used).  If left as a
  // floating input w/SPI on, this can cause lockuppage.
#if !defined(SOFTWARE_SPI)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if(chipSelect != 53) pinMode(53, OUTPUT); // SS on Mega
#else
  if(chipSelect != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
#endif
#endif

 // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  cam.setImageSize(VC0706_160x120);          // small
  cam.setCompression(0xEE); // overwrite compression settings 00 to FF FF is higher rate
  Serial.println("comp setting");
  uint8_t Compression = cam.getCompression();  //Remove because need to debug
  Serial.println(Compression);
  
  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");
  

}

void loop() {

  bouncer.update ( );
 // t.update(); // send keepalive every keep_alive_intervall milliseconds
  int reading = digitalRead(buttonPin);
  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH), 
 
   if (debug_enable == 1) {
      Serial.print("    bouncer ");
      Serial.println(bouncer.read());
       
      Serial.print("            bouncer rising edge ");
      Serial.println(bouncer.risingEdge());
      
      Serial.print("            bouncer fallingEdge ");
      Serial.println(bouncer.fallingEdge());
      Serial.print("      button pin read ");
      Serial.println(reading);
  }
 //LANCER LA SEQUENCE D'ENVOIE RECEPTION
//   if (buttonState == HIGH && is_MESSAGE_sent == 0) {
     if (bouncer.fallingEdge() == true) {  
/*     is_MESSAGE_sent = 1;
     lastDebounceTime =0;
     buttonState = LOW;
  */  if (debug_enable == 1) {
       Serial.print("sending MESSAGE ");
       for (int p =0; p < 5000; p++)  {         } //little wait before printing out data, visual effect
       Serial.print("ack avant while ack = :");  
       Serial.println(ack);  
     } // end debug
  ack=0;
 udpsend(remoteIPadress_broadcast,destinationPort,MESSAGE);
  for (int im =0; im < 100; im++)
    {
       cam.takePicture();
  // Get the size of the image (frame) taken  
  uint16_t jpglen = cam.frameLength();
  Serial.print("Storing ");
  Serial.print(jpglen, DEC);  //  Serial.print(jpglen, DEC);
  Serial.print(" byte image.");

  int32_t time = millis();
  pinMode(8, OUTPUT);
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
   // imgFile.write(buffer, bytesToRead);
  // socketSend(bu, uint16_t size, uint8_t ip[], uint16_t port);
   // udpsend(remoteIPadress_broadcast,destinationPort,MESSAGE);
    Serial.write(buffer,bytesToRead);
    if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
     // Serial.print('.');
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  cam.resumeVideo();
      
    }
 
 /*
     
     int packetSize = Udp.parsePacket();
     if(packetSize)
        {
          // debug : identifiant expediteur
          Serial.print("Received packet of size);     Serial.println(packetSize);    Serial.print(From ");
          IPAddress remote = Udp.remoteIP(); 
          for (int i =0; i < 4; i++)
          {
            Serial.print(remote[i], DEC);
            if (i < 3)
            {
              Serial.print(".");
            }
          }   // end debug identifier A METTRE EN COMMENTAIRE POUR FINAL RELEASE
          Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);  // read the packet into packetBufffer
          Serial.println("Contents:");  // debug
          Serial.print(packetBuffer); // DEBUG
          Serial.println("+"); // DEBUG
          if (debug_enable == 1) { 
             Serial.print("ack :");  // debug
             Serial.println(ack);  // debug
             int sLen = packetSize +1; // to be sure of what is the next caracter ?
             for(int s=0; s<sLen; s++)
              {
               Serial.print("theCommand[");
               Serial.print(s);
               Serial.print("] is {");
               Serial.print(packetBuffer[s]);
               Serial.print("} which has an ascii value of ");
               Serial.println(packetBuffer[s], DEC);
             }
             for (int k =0; k < 5000; k++) //this produce a little pause to provide time to acknoledge on serial debug screen
            {          }
           }
        
     }
     */
     
  }
   
  
     memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE); // clear the ethernet buffer (at least try :-)
}
void keepaliv(){
    if (debug_enable == 1) { 
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!   SENDING KEEP ALIVE !!!!!!!!!!!!!!!!!!!!!!!!!!");  // debug
    }
 
     udpsend(remoteIPadress_broadcast,destinationPort,keep_alive_message);
      
        int packetSize = Udp.parsePacket();
         if(packetSize){
        Udp.read(packetRead,UDP_TX_PACKET_MAX_SIZE);
    //delay(10);
         if (debug_enable == 1) { 
             Serial.print("ack apres check:");  // debug
             Serial.println(ack);  // debug
             int sLen = packetSize +1; // to be sure of what is the next caracter ?
               Serial.print("sLen ");
               Serial.println(sLen);
             for(int s=0; s<sLen; s++)
              {
               Serial.print("theCommand[");
               Serial.print(s);
               Serial.print("] is {");
               Serial.print(packetRead[s]);
               Serial.print("} which has an ascii value of ");
               Serial.println(packetRead[s], DEC);
             }
         } 
       }
}
   
void udpsend(IPAddress ipdestinataire, unsigned int port_destinataire, char message[]) {
  
    Udp.beginPacket(ipdestinataire, port_destinataire);
    Udp.write(message);
    Udp.endPacket();
}
void doAfter()
{
  Serial.println("    RESTART reading");
  reading = 1;
}



