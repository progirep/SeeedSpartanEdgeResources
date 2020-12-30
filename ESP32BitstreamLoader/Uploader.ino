/*
 * 01LoadDefaultBitstream
 *  
 * loading the default Bitstream
 *
 * The MIT License (MIT)
 * Copyright (C) 2019  Seeed Technology Co.,Ltd.
 */

// include the library:
// #include <spartan-edge-esp32-boot.h>


#include <Arduino.h>
#include <SPI.h>

/* pin define */
#define XFPGA_CCLK_PIN 17
#define XFPGA_DIN_PIN 27
#define XFPGA_PROGRAM_PIN 25
#define XFPGA_INTB_PIN 26
#define XFPGA_DONE_PIN 34

// Load Wi-Fi library
#include <WiFi.h>
#include <esp_spi_flash.h>

// Replace with your network credentials
char ssid[256];
char password[64];
char statusString[64];

#define START_ADDRESS_FPGA_PROGRAM 0x200000
#define START_ADDRESS_WIFI_PASSWD (START_ADDRESS_FPGA_PROGRAM-SPI_FLASH_SEC_SIZE)
#if (START_ADDRESS_FPGA_PROGRAM % SPI_FLASH_SEC_SIZE) != 0
#error Starting address must be dividable by Flash sector size!
#endif
#define START_ADDRESS_FPGA_PROGRAM 0x200000

// Set web server port number to 80
WiFiServer server(80);

// initialize the spartan_edge_esp32_boot library
// spartan_edge_esp32_boot esp32Cla;

// the bitstream name which we loading
#define LOADING_DEFAULT_FIEE "/overlay/default.bit"

void fatal_error(const char *error) {
  Serial.print("Fatal error: ");
  Serial.print(error);
  Serial.print("\n\n");
  while (1) {};
}

SPIClass vspi(VSPI);

// the setup routine runs once when you press reset:
void setup() {
  
  // initialization 
  //esp32Cla.begin();

  // XFPGA pin Initialize
  //esp32Cla.xfpgaGPIOInit();

  // loading the bitstream
  //esp32Cla.xlibsSstream(LOADING_DEFAULT_FIEE);

  Serial.begin(115200);
  Serial.print("                                         \n                               \n                            \n                      \n================[Start]==============\n");

  // The following: Src: https://github.com/m4k3r-org/spartan-edge-esp32-boot/blob/master/src/spartan-edge-esp32-boot.cpp

  // Initialize I/O
  pinMode(XFPGA_INTB_PIN, OUTPUT | PULLUP);
  digitalWrite(XFPGA_INTB_PIN, HIGH);
  pinMode(XFPGA_PROGRAM_PIN, OUTPUT);
  digitalWrite(XFPGA_PROGRAM_PIN, HIGH);
  delay(50);

  // FPGA configuration start sign
  digitalWrite(XFPGA_INTB_PIN, LOW);
  digitalWrite(XFPGA_PROGRAM_PIN, LOW);
  delay(50);
  digitalWrite(XFPGA_INTB_PIN, HIGH);
  digitalWrite(XFPGA_PROGRAM_PIN, HIGH);


  unsigned char byte_buff[1024];
  int bytes_left = 4310752/8;
  int buffer_pos = 100;
  uint32_t readPtr = START_ADDRESS_FPGA_PROGRAM;
  int bitstream_length = 4310752/8;
  
  if (spi_flash_read(readPtr,byte_buff,1024)!=ESP_OK) fatal_error("Error reading bitstream");
  readPtr += 1024;
  
  /* put pins down for Configuration */
  pinMode(XFPGA_CCLK_PIN, OUTPUT);
  pinMode(XFPGA_DIN_PIN, OUTPUT);
  digitalWrite(XFPGA_CCLK_PIN, LOW);
  digitalWrite(XFPGA_DIN_PIN, LOW);

  while (bytes_left>0) {
    while ((buffer_pos<1024) && (bytes_left>0)) {
      byte tb = byte_buff[buffer_pos++];

      for (int j = 0;j < 8;j++) {
        digitalWrite(XFPGA_CCLK_PIN, LOW);
        if ((tb&0x80) == 0x80)
          digitalWrite(XFPGA_DIN_PIN, HIGH);
        else
          digitalWrite(XFPGA_DIN_PIN, LOW);
        tb = tb << 1;
        digitalWrite(XFPGA_CCLK_PIN, HIGH);
      }
      bytes_left--;
    }
    if (bytes_left==0) break;
    if (spi_flash_read(readPtr,byte_buff,1024)!=ESP_OK) fatal_error("Error reading bitstream");
    readPtr += 1024;
    buffer_pos=0;
  }
  digitalWrite(XFPGA_CCLK_PIN, LOW); 

  // Test if writing worked?
  pinMode(XFPGA_DONE_PIN, INPUT);
  if(0 == digitalRead(XFPGA_DONE_PIN)) {
    snprintf(statusString,64,"Failed to configure FPGA");
    Serial.println(statusString);
  }
  else {
    snprintf(statusString,64,"FPGA Successfully configured");
    Serial.println(statusString);
  }

  
  Serial.print("Setting AP (Access Point)...");

  // Read password
  spi_flash_read(START_ADDRESS_WIFI_PASSWD, (uint32_t *)password, 64);
  if ((password[60] == 0xBA) &&
     (password[61] == 0xDE) &&
     (password[62] == 0xAF) &&
     (password[63] == 0xFE)) {
       Serial.print("Using the following Wifi password: ");
       Serial.println(password);
     } else {
      strcpy(password,"Radius210");
      Serial.println("Password not yet set! Using default password.");
     }
                

  String mac = WiFi.macAddress();
  snprintf(ssid,256,"FPGA-%s",mac.c_str());
  
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();

  /*char test[1024];
  if (spi_flash_read(START_ADDRESS_FPGA_PROGRAM,test,1024)!=ESP_OK) {
    Serial.print("Failure to read flash\n");
  } else {
    Serial.print("Flash read correctly!\n");
    Serial.print(test);
    Serial.print("\n");
  }*/

  // SPI
  pinMode(SS, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  digitalWrite(SS, HIGH); 
  vspi.begin (SCK, MISO, MOSI);
  vspi.setBitOrder(MSBFIRST);
  vspi.setDataMode(SPI_MODE0);

  // Clock Diver Values figured out by inspecting spiClk_t from https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-spi.c
  // Good source: https://www.esp8266.com/viewtopic.php?p=13958
  //vspi.setClockDivider(1052673); // 8.0 Mhz ----> Works fine!
  //vspi.setClockDivider(790529); // 10 MHz  ----> Gives Transmission errors!!!
  //vspi.setClockDivider(28871); // different config for 10 Mhz  ----> Also Gives Transmission errors!!!
  //vspi.setClockDivider(32968); // 8.88888 MHz <--- Gives a few errors. May work with a FPGA core speed >100 MHz?
  // vspi.setClockDivider(37129); // 8 MHz, alternative encoding without prescaler
  // 33032 and 28935 also work, 24774 gives transmission errors, 24838 also works (7 clock cycles!), 20741 works as well (6 clock cycles).
  vspi.setClockDivider(20741); // LOW: 5, HIGH:4, N: 5, Prescaler = 0 ---> 80/6 MHz clock rate
  

}


String getPartUntilSlash(String &residue, bool &term) {
  int pos = 0;
  String ret;
  term = false;
  while (pos<residue.length()) {
    switch (residue[pos]) {
      case ' ':
      case '\n':
        term = true;
      case '/':
        ret = residue.substring(0,pos);
        residue = residue.substring(pos+1);
        return ret;
      default:
        pos++;
    }
  }
  ret = residue;
  residue = "";
  return ret;
}

  
/*
 * Sources:
 * 
 * - Examples by Seeed Studio
 * - https://randomnerdtutorials.com/esp32-access-point-ap-web-server/
 * - http://www.journeyintocode.com/2013/11/reading-binary-data-with-file-api-and.html
 */

// the loop routine runs over and over again forever:
void loop() {
  
  WiFiClient client = server.available();   // Listen for incoming clients

  String header = "\n";
  
  if (client) {                             // If a new client connects,
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          //Serial.write("Checking now!");      
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // Checking!
            if (header.indexOf("\nGET /teston/") >= 0) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close\n");
                client.println("Writing/Sending:");
                String signalName=header.substring(header.indexOf("\nGET /teston/")+13);
                if (signalName.startsWith("ss")) {
                    digitalWrite(SS, HIGH); 
                    client.println("SS Signal");
                } else if (signalName.startsWith("sck")) {
                    digitalWrite(SCK, HIGH); 
                    client.println("SCK Signal");
                } else if (signalName.startsWith("mosi")) {
                    digitalWrite(MOSI, HIGH); 
                    client.println("MOSI Signal");
                }
                client.println("\n\n");
                client.stop();
                Serial.write("All done!\n");
                return; // Main loop
            }

            else if (header.indexOf("\nGET /testoff/") >= 0) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close\n");
                client.println("Writing/Sending:");
                String signalName=header.substring(header.indexOf("\nGET /testoff/")+14);
                if (signalName.startsWith("ss")) {
                    digitalWrite(SS, LOW); 
                    client.println("SS Signal");
                } else if (signalName.startsWith("sck")) {
                    digitalWrite(SCK, LOW); 
                    client.println("SCK Signal");
                } else if (signalName.startsWith("mosi")) {
                    digitalWrite(MOSI, LOW); 
                    client.println("MOSI Signal");
                }
                client.println("\n\n");
                client.stop();
                Serial.write("All done!\n");
                return; // Main loop
            }

            // SPI configuration
            else if (header.indexOf("\nGET /spispeed/") >= 0) {
              String rest = header.substring(header.indexOf("\nGET /spispeed/")+15);
              Serial.println("Setting SPI Speed\n");
              bool done;
              String part = getPartUntilSlash(rest,done);
              int speed = part.toInt();

              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close\n");
              client.println("Setting SPI clock divider to:");
              client.println(speed);
              vspi.setClockDivider(speed);
              if (!done)
                client.println("Warning: Not quite well-formed request.");
              client.println("\n\n");
              client.stop();
              Serial.write("All done!\n");
              return; // Main loop
              
            }
            
            // SPI transmission
            else if (header.indexOf("\nGET /spi/") >= 0) {

                // Parse SPI
                uint8_t inputData[64];
                int nofInputChars = 0;
                bool done = false;
                String rest = header.substring(header.indexOf("\nGET /spi/")+10);
                Serial.println("DEbug Data\n");
                while (!done) {
                  String part = getPartUntilSlash(rest,done);
                  Serial.println("Get Part\n");
                  Serial.println(part.c_str());
                  if (part!="") {
                    inputData[nofInputChars++] = part.toInt();
                  }
                }

                Serial.println("Client data core\n");
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close\n");
                client.println("Writing/Sending:");
                
                digitalWrite(SS, LOW); 
                for (int i=0;i<nofInputChars;i++) {
                  int received = vspi.transfer(inputData[i]);
                  char result[64];
                  snprintf(result,64,"- %d / %d",(int)(inputData[i]),received);
                  client.println(result);
                  Serial.println("Send/Receive Byte");
                }
                
                digitalWrite(SS, HIGH); 
                
                // Debug...
                client.println(String(vspi.getClockDivider())+" is the clock divider.\n");
                client.println(String((uint32_t)(vspi.bus()))+" is the bus.\n");
              
                client.println("\n\n");
                client.stop();
                Serial.write("All done!\n");
                return; // Main loop
            }
            
            else if (header.indexOf("GET /passwd/") >= 0) {
              int locStr = header.indexOf("GET /passwd/")+12;
              char pw[64];
              int writeAddress = 0;
              while ((header[locStr]!=' ') && (header[locStr]!='\n')) {
                pw[writeAddress++] = header[locStr++];
              }
              if ((writeAddress>=10) && (writeAddress<=50)) {
                pw[writeAddress++] = 0;

                // Add signature
                pw[60] = 0xBA;
                pw[61] = 0xDE;
                pw[62] = 0xAF;
                pw[63] = 0xFE;
                
                Serial.println("Resetting password to:");
                Serial.println(pw);
                Serial.println("writing now...");
                spi_flash_erase_sector((START_ADDRESS_WIFI_PASSWD) / SPI_FLASH_SEC_SIZE);
                spi_flash_write(START_ADDRESS_WIFI_PASSWD, (uint32_t *)pw, 64);

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/plain");
                client.println("Connection: close");
                client.println("\nPassword was reset to:");
                client.println(pw);
                client.println("\n\n");
                client.stop();
                Serial.write("All done!\n");
                return; // Main loop
                }

              //3.spi_flash_read(0x3C000, (uint32_t *)buff, 64);

                
              client.println("HTTP/1.1 403 Access Denied!");
              client.println("Content-type:text/text");
              client.println("Connection: close");
              client.println("\nPassword reset wrong: Between 10 and 50 characters wanted\n\n");
              client.stop();
              return; // Main loop
              
            } else if (header.indexOf("GET /upload/") >= 0) {
              //Serial.println("Received (Upload):");
              //Serial.println(header.c_str());
              
              // Parsing location
              int locStr = header.indexOf("GET /upload/")+12;
              int writeAddress = 0;
              while (header[locStr]!='/') {
                writeAddress *= 10;
                if ((header[locStr]>='0') && (header[locStr]<='9')) writeAddress += header[locStr] - '0';
                else {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nMalformed upload...\n\n");
                  client.stop();
                  return; // This is loop...                 
                }
                locStr++;
              }

              locStr++;

              // Writing to flash
              uint8_t writeBuffer[2048];
              int writePtr = 0;

              // Parsing data
              while ((header[locStr]!=' ') && (header[locStr]!='\n')) {
                uint8_t thisByte = 0;

                if (writePtr>=2048) {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nData too long...\n\n");
                  client.stop();
                  return; // Main loop
                }
                
                if ((header[locStr]>='0') && (header[locStr]<='9')) thisByte = (header[locStr] - '0')*16;
                else if ((header[locStr]>='a') && (header[locStr]<='f')) thisByte = (header[locStr] - 'a' + 10)*16;
                else {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nMalformed upload (A)...\n\n");
                  client.stop();
                  return; // This is loop...                 
                }

                locStr++;

                if ((header[locStr]>='0') && (header[locStr]<='9')) thisByte += (header[locStr] - '0');
                else if ((header[locStr]>='a') && (header[locStr]<='f')) thisByte += (header[locStr] - 'a' + 10);
                else {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nMalformed upload (B)...\n\n");
                  client.stop();
                  return; // This is loop...                 
                }

                locStr++;

                writeBuffer[writePtr++] = thisByte;
              }

              //Serial.write("Wrote to write buffer\n");
              
              // Fill with 0s until 32-bit aligned
              while (writePtr & 3) writeBuffer[writePtr++] = 0;

              if (writePtr>2048) {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nData too long (B)...\n\n");
                  client.stop();
                  return; // Main loop
                }

              // Address sanity check
              int32_t writePos = START_ADDRESS_FPGA_PROGRAM+writeAddress;
              if ((writePos<0) || (writePos<writeAddress) ||  ((START_ADDRESS_FPGA_PROGRAM+writeAddress+writePtr) < 0)) {
                  client.println("HTTP/1.1 403 Access Denied!");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println("\nAddress is not ok...\n\n");
                  client.stop();
                  return; // Main loop
              }

              // Delete if needed
              if ((writeAddress % SPI_FLASH_SEC_SIZE)==0) {
                spi_flash_erase_sector((START_ADDRESS_FPGA_PROGRAM+writeAddress) / SPI_FLASH_SEC_SIZE);
              }

              //Serial.write("Flash erase done!\n");
              

              spi_flash_write(START_ADDRESS_FPGA_PROGRAM+writeAddress, (uint32_t *)writeBuffer, writePtr);
              //3.spi_flash_read(0x3C000, (uint32_t *)buff, 64);

              //Serial.write("Flash Write done!\n");
              


              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println("\nUpload received...\n\n");


              
              break;
            } else if ((header.indexOf("GET / ") >= 0) || (header.indexOf("GET /\n") >= 0)) {
              
              
              // Display the HTML web page
              //Serial.write("Main Page\n");      
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
  
              
              client.println("<!DOCTYPE HTML>\n<html>\n<head>");
              client.println("<title>FPGA Board Access</title>\n</head>");
              client.println("<body>");
              client.println("<H1>FPGA Board Access</H1>");
              client.print("<p>Status: ");
              client.print(statusString);
              client.println("</p>");
              client.println("<input id=\"browseOpen\" type=\"file\"/>");
              client.println("<script type=\"text/javascript\">\r\n            var fileInput = document.getElementById(\"browseOpen\");\r\n            fileInput.onchange = function () {            \r\n                var fr = new FileReader();\r\n                fr.onloadend = function () {\r\n                    var result = this.result;\r\n                    var hex = \"\";\r\n                    var i=0;\r\n                    var posWrite = 0;\r\n                    for (; i < this.result.length; i++) {\r\n                        var byteStr = result.charCodeAt(i).toString(16);\r\n                        if (byteStr.length < 2) {\r\n                            byteStr = \"0\" + byteStr;\r\n                        }\r\n                        hex += byteStr;\r\n                        if ((i % 2048)==2047) {\r\n                            const http = new XMLHttpRequest()\r\n\r\n                            http.open(\"GET\", \"/upload/\"+(posWrite)+\"/\"+hex,false)\r\n                            http.send()\r\n                            http.onload = () => console.log(http.responseText)\r\n                            if (http.status!=200) {\r\n                                alert(\"Upload failed! \"+http.status)\r\n                                return\r\n                            }\r\n                            hex = \"\"\r\n                            document.getElementById(\"progress\").innerHTML = \"Bytes submitted: \"+(i);\r\n                            posWrite += 2048;\r\n                        }\r\n                    }\r\n                    \r\n                    // Last round\r\n                    const http = new XMLHttpRequest()\r\n                    http.open(\"GET\", \"/upload/\"+(posWrite)+\"/\"+hex,false)\r\n                    http.send()\r\n                    http.onload = () => console.log(http.responseText)\r\n                    if (http.status!=200) {\r\n                        alert(\"Upload failed! \"+http.status)\r\n                        return\r\n                    }                    \r\n                    document.getElementById(\"progress\").innerHTML = \"All Bytes submitted!\";\r\n                    alert(\"Upload finished\");\r\n                    \r\n                };\r\n                fr.readAsBinaryString(this.files[0]);\r\n            };\r\n        </script>\r\n        <div id=\"progress\"/> </div>\r\n    </body>\r\n    </html>\r\n\r\n");
              
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else {
              // 404
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println("URL Not found!\n\n");
              break;
            }
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }        
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    //Serial.println("Client disconnected.");
    //Serial.println("");
  }
}
