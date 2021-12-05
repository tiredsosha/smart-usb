#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "FS.h"

// Set AP credentials
#define AP_SSID "Smart USB"
#define AP_PASS "hello88io"

// Set pins
#define DATA 1
#define VOL 2
#define GREEN 4
#define RED 5

// Set WIFI
char *ssid = "hello.io";
char *password = "hello88io"; 

// Set temp WIFI
String webpage = "";
String SSID = "";
String PASS = "";

ESP8266WebServer server(80); //Server on port 80


void relay() 
{
    digitalWrite(DATA, LOW);
    delay(500);
    digitalWrite(VOL, LOW);
    delay(500);
    digitalWrite(VOL, HIGH);
    delay(500);
    digitalWrite(DATA, HIGH);
} 

void files() 
{
    bool success = SPIFFS.begin();

    if (!success) {
        Serial.println("Error mounting the file system");
    return;
    }
    File creds = SPIFFS.open("/creds.txt", "r");

    if (!creds) {
        Serial.println("Failed to open file for reading");
        File credsW = SPIFFS.open("/creds.txt", "w");
        if (!credsW) {
            Serial.println("Error opening file for writing");
            return;
        }
        int bytesWritten = credsW.print("hello.io hello88io");

        if (bytesWritten == 0) {
            Serial.println("File write failed");
            return;
        }
        credsW.close();
    } 
    else
    {
        while (creds.available()) 
        {
            String data = creds.read();
            Serial.write(data);
            ssid = getValue(data, ' ', 0);
            password = getValue(data, ' ', 1);
        }
        creds.close();
    }
} 

void wifi()
{
    webpage = "";
    webpage += "<!DOCTYPE html>";
    webpage += "<html>";
    webpage += "<body>";
    webpage += "<form action=\"/wifi\" method =\"POST\" onkeydown=\"return event.key != 'Enter'\">";
    webpage += "<h1>WIFI Settings</h1>";
    webpage += "<label for=\"fname\">SSID: </label>";
    webpage += "<input type=\"text\" id=\"ssid\" name=\"ssid\"><br><br>";
    webpage += "<label for=\"lname\">Password: </label>";
    webpage += "<input type=\"text\" id=\"pass\" name=\"pass\"><br><br>";
    webpage += "<input type=\"submit\" value=\"Save\">";
    webpage += "</form>";
    webpage += "<p>Reboot device after changing wifi</p>";
    webpage += "<a href = \"/reboot\">";
    webpage += "<button>Reboot</button>";
    webpage += "</a>";
    webpage += "</body>";
    webpage += "</html>";
    server.send(200, "text/html", webpage);
    if (server.args() > 0)
    {
        for (uint8_t i = 0; i <= server.args(); i++)
        {
            String Argument_Name = server.argName(i);
            String client_response = server.arg(i);
            if (Argument_Name == "ssid")
                SSID = client_response;
            if (Argument_Name == "pass")
                PASS = client_response;
        }
        SSID.trim();
        PASS.trim();
        Serial.println("ssid was : " + SSID);
        Serial.println("pass was : " + PASS);
    }
}

void reboot()
{
    webpage = "";
    webpage = "WIFI configuration is done. Rebooting";
    server.send(200, "text/plain", webpage);
    Delay(5000);
    ESP.restart();
}

void relay()
{
    server.send(200, "text/plain", "swithing relay");
    relay();
}

void setup(void)
{
    Serial.begin(115200);

    // Pins setup
    pinMode(DATA, OUTPUT);
    pinMode(VOL, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(RED, OUTPUT);
    
    // Relay config
    digitalWrite(DATA, LOW);
    digitalWrite(VOL, LOW);

    // Read file
    files();

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    for (int i = 0; i <= 19; i++)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            delay(10000);
            Serial.println("connecting");
        }
        else
        {
            Serial.println("");
            Serial.println("WiFi connection Successful");
            Serial.println("The IP Address of Smart USB is: ");
            Serial.println(WiFi.localIP()); // Print the IP address
            digitalWrite(GREEN, LOW);
            break;
        }
    }
    // Starting AP
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Can't connect to WIFI");
        Serial.println("Enabling an AP");
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASS);
        digitalWrite(RED, LOW);
    }
    // Set routes
    server.on("/", wifi);
    server.on("/reboot", reboot);
    server.on("/relay", relay);
    // Start Webserver
    server.begin();
    Serial.println(F("Webserver started..."));
}

void loop()
{
    server.handleClient();
}
