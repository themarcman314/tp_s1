#include <SPI.h>  
#include <RH_RF95.h>  
#include <dht.h>

dht DHT;


// This is the address that indicates a broadcast
#define RH_BROADCAST_ADDRESS 0xff

#define DHT22_PIN     5


struct
{
    uint32_t total;
    uint32_t ok;
    uint32_t crc_error;
    uint32_t time_out;
    uint32_t connect;
    uint32_t ack_l;
    uint32_t ack_h;
    uint32_t unknown;
} stat = { 0,0,0,0,0,0,0,0};

// Singleton instance of the radio driver  
RH_RF95 rf95;  
float frequency = 868.1;  
 
void setup()  
{  
  pinMode(8, OUTPUT);
  Serial.begin(9600);  
  //while (!Serial) ; // Wait for serial port to be available  
  Serial.println("Start LoRa Client");  
  if (!rf95.init()) //[NOM DE L’AUTEUR]   
  Serial.println("init failed");  
  // Setup ISM frequency  
  rf95.setFrequency(frequency);  
    // Setup Power,dBm  
  rf95.setTxPower(13);  //25 mW is 13.9 dB
   
  // Setup Spreading Factor  
  rf95.setSpreadingFactor(7);  
 
  // Setup BandWidth, option  
  rf95.setSignalBandwidth(125000);  
 
  // Setup Coding Rate: 5(4/5),6(4/6),7(4/7),8(4/8)  
  rf95.setCodingRate4(5);  
}  
 
void loop()  
{  
  Serial.println("Sending to LoRa Server");  
  // Send a message to LoRa Server  
  uint8_t data[] = "something";  
  rf95.send(data, sizeof(data));  
 
  rf95.waitPacketSent();  
  // Now wait for a reply  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];  
  uint8_t len = sizeof(buf);  
 
if (rf95.waitAvailableTimeout(4000)) //(rf95.waitAvailableTimeout(3000))  
{  
  // Should be a reply message for us now  
   if (rf95.recv(buf, &len))  
  {  
    Serial.print("got reply: ");  
    Serial.println((char*)buf);
    
    // Get the RSSI of the received packet
    int8_t rssi = rf95.lastRssi();
    Serial.print("Received signal strength (RSSI): ");
    Serial.print(rssi);
    Serial.println(" dBm");

    // Get the SNR of the received packet
    float snr = rf95.lastSNR();
    Serial.print("Signal-to-Noise Ratio (SNR): ");
    Serial.print(snr);
    Serial.println(" dB");
    // Le SNR est lié avec la fiabilité de la communication.

    digitalWrite(8, !digitalRead(8));
   }  
    else  
   {  
    Serial.println("recv failed");  
   }  
  }  
 else  
  {  
        Serial.println("No reply, is LoRa server running?");  
  }
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)\tTime (us)");
  // READ DATA
  Serial.print("DHT22, \t");

  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();

  stat.total++;
  switch (chk)
  {
  case DHTLIB_OK:
      stat.ok++;
      Serial.print("OK,\t");
      break;
  case DHTLIB_ERROR_CHECKSUM:
      stat.crc_error++;
      Serial.print("Checksum error,\t");
      break;
  case DHTLIB_ERROR_TIMEOUT:
      stat.time_out++;
      Serial.print("Time out error,\t");
      break;
  case DHTLIB_ERROR_CONNECT:
      stat.connect++;
      Serial.print("Connect error,\t");
      break;
  case DHTLIB_ERROR_ACK_L:
      stat.ack_l++;
      Serial.print("Ack Low error,\t");
      break;
  case DHTLIB_ERROR_ACK_H:
      stat.ack_h++;
      Serial.print("Ack High error,\t");
      break;
  default:
      stat.unknown++;
      Serial.print("Unknown error,\t");
      break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.print(DHT.temperature, 1);
  Serial.print(",\t");
  Serial.print(stop - start);
  Serial.println();

  if (stat.total % 20 == 0)
  {
      Serial.println("\nTOT\tOK\tCRC\tTO\tCON\tACK_L\tACK_H\tUNK");
      Serial.print(stat.total);
      Serial.print("\t");
      Serial.print(stat.ok);
      Serial.print("\t");
      Serial.print(stat.crc_error);
      Serial.print("\t");
      Serial.print(stat.time_out);
      Serial.print("\t");
      Serial.print(stat.connect);
      Serial.print("\t");
      Serial.print(stat.ack_l);
      Serial.print("\t");
      Serial.print(stat.ack_h);
      Serial.print("\t");
      Serial.print(stat.unknown);
      Serial.println("\n");
  }
  delay(3000);  
} 