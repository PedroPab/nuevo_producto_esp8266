#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

const char *ssid_AP = "Virus_Gratis";
const char *password_AP = "12345678";
String password_wifi ;
String ssid_wifi;

String password_eeprom;
String ssid_eeprom;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  delay(10);
  Serial.println();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_AP, password_AP);

  Serial.println("WiFi conectada.");

  Serial.println();
  WiFi.printDiag(Serial);
  Serial.println();

  Serial.print("AP dirección IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    server.send(200, "text/plain", "Hola mundo!!");
  });
  server.on("/hola", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.begin();

  Serial.println("Servidor inicializado.");

}
void loop() {
  Serial.println("{------");
  Serial.println();
  EEPROM.get(0, password_eeprom);
  EEPROM.get(100, ssid_eeprom);
  Serial.println(password_eeprom);
  Serial.println(ssid_eeprom);
  Serial.println("-------}");

  if (ssid_eeprom != "hola" ) {
    server.handleClient();
    Serial.println(" wifi no guardado");

  } else  {

  }



  delay(100);
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"WiFi_SSID\" placeholder=\"WiFi_SSID\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'John Doe' and 'password123' ...</p>");
}

void handleLogin() {                         // If a POST request is made to URI /login
  if ( ! server.hasArg("WiFi_SSID") || ! server.hasArg("password")
       || server.arg("WiFi_SSID") == NULL ) { // If the POST request doesn't have WiFi_SSID and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  ssid_wifi = String(server.arg("WiFi_SSID"));
  password_wifi = String(server.arg("password"));

  EEPROM.put(0, password_wifi);
  EEPROM.put(100, ssid_wifi);
  Serial.println("guardado ");
  Serial.println(EEPROM.commit());//hay que poner un commit para que se guarde
  Serial.println(server.arg("WiFi_SSID"));
  Serial.println(server.arg("password"));

  server.send(201, "text/plain", "todo melo");

  //  if (server.arg("WiFi_SSID") == "John Doe" && server.arg("password") == "password123") { // If both the WiFi_SSID and the password are correct
  //    server.send(200, "text/html", "<h1>Welcome, " + server.arg("WiFi_SSID") + "!</h1><p>Login successful</p>");
  //  } else {                                                                              // WiFi_SSID and password don't match
  //    server.send(401, "text/plain", "401: Unauthorized");
  //  }
}
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
