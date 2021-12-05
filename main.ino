#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

// Set AP credentials
#define AP_SSID "Smart USB"
#define AP_PASS "hello88io"

const char *ssid = "Chich&KO";        //Enter SSID
const char *password = "Sosha_4ever"; //Enter Password

String webpage = "";
String SSID = "";
String PASS = "";

ESP8266WebServer server(80); //Server on port 80

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
// Don't use
void home()
{
    webpage = "";
    webpage = "This is home page\nredirecting to wifi setup";
    server.send(200, "text/plain", webpage);
    wifi();
}

void setup(void)
{
    Serial.begin(115200);
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
            Serial.println("The IP Address of ESP8266 Module is: ");
            Serial.println(WiFi.localIP()); // Print the IP address
            break;
        }
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Can't connect to WIFI");
        Serial.println("Enabling an AP");
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASS);
    }
    server.on("/", wifi);
    server.on("/reboot", reboot);
    server.begin();
    Serial.println(F("Webserver started...")); // Start the webserver
}

void loop()
{
    server.handleClient();
}
