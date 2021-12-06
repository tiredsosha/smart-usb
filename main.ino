#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include "FS.h"

// Set AP credentials
#define AP_SSID "Smart USB"
#define AP_PASS "hello88io"

// Set pins
#define DATA 0
#define VOL 2
#define GREEN 15
#define RED 13

// Set WIFI
String ssid = "hello.io";
String password = "hello88io";

// Set temp WIFI
String webpage = "";
String SSID = "";
String PASS = "";

ESP8266WebServer server(80); //Server on port 80

void relayRestart()
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
    if (!success)
    {
        Serial.println("Error mounting the file system");
        return;
    }

    File ssidF = SPIFFS.open("/ssid.txt", "r");
    if (!ssidF)
    {
        Serial.println("Failed to open file for reading");
        File ssidW = SPIFFS.open("/ssid.txt", "w");
        if (!ssidW)
        {
            Serial.println("Error opening file for writing");
            return;
        }
        int bytesWritten = ssidW.print("hello.io|");

        if (bytesWritten == 0)
        {
            Serial.println("File write failed");
            return;
        }
        ssidW.close();
    }
    else
    {
        while (ssidF.available())
        {
            String dataS = ssidF.readStringUntil('|');
            Serial.println(dataS);
            ssid = dataS;
        }
        ssidF.close();
    }

    File passwordF = SPIFFS.open("/password.txt", "r");
    if (!passwordF)
    {
        Serial.println("Failed to open file for reading");
        File passwordW = SPIFFS.open("/password.txt", "w");
        if (!passwordW)
        {
            Serial.println("Error opening file for writing");
            return;
        }
        int bytesWritten = passwordW.print("hello88io|");

        if (bytesWritten == 0)
        {
            Serial.println("File write failed");
            return;
        }
        passwordW.close();
    }
    else
    {
        while (passwordF.available())
        {
            String dataP = passwordF.readStringUntil('|');
            Serial.println(dataP);
            password = dataP;
        }
        passwordF.close();
    }
}

void wifi()
{
    webpage = "";
    webpage += "<!DOCTYPE html>";
    webpage += "<html>";
    webpage += "<body>";
    webpage += "<form action=\"/\" method =\"POST\" onkeydown=\"return event.key != 'Enter'\">";
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
    webpage += "<br><p>Reload relay</p>";
    webpage += "<a href = \"/relay\">";
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
        Serial.println("ssid was : " + SSID);
        SSID = SSID + "|";
        PASS.trim();
        Serial.println("pass was : " + PASS);
        PASS = PASS + "|";

        File file = SPIFFS.open("/ssid.txt", "w");

        if (!file)
        {
            Serial.println("Error opening file for writing");
            return;
        }
        int bytesWritten1 = file.print(SSID);
        if (bytesWritten1 == 0)
        {
            Serial.println("File write failed");
            return;
        }
        file.close();
        File file2 = SPIFFS.open("/password.txt", "w");

        if (!file2)
        {
            Serial.println("Error opening file for writing");
            return;
        }
        int bytesWritten2 = file2.print(PASS);
        if (bytesWritten2 == 0)
        {
            Serial.println("File write failed");
            return;
        }
        file2.close();
    }
}

void reboot()
{
    webpage = "";
    webpage = "WIFI configuration is done. Rebooting";
    Serial.println("rebooting");
    server.send(200, "text/plain", webpage);
    delay(5000);
    ESP.restart();
}

void relay()
{
    server.send(200, "text/plain", "Swithing relay");
    relayRestart();
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
    for (int i = 0; i <= 18; i++)
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
            digitalWrite(GREEN, HIGH);
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
        digitalWrite(RED, HIGH);
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
