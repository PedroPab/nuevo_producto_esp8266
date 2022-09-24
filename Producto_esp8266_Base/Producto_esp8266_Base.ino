#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define button D4
#define led LED_BUILTIN


const char *ssid_AP = "Virus_Gratis";
const char *password_AP = "12345678";

String password_wifi ;
String ssid_wifi;

String password_eeprom;
String ssid_eeprom;

boolean  starts = false;

ESP8266WebServer server(80);

void setup() {
  pinMode(led, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(button , INPUT_PULLUP);
  EEPROM.begin(512);
  delay(250);
  Serial.begin(115200);


  delay(1000);
  resetMemory();


  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_AP, password_AP);

  Serial.println("red iniciada");
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

}
void loop() {
  EEPROM.begin(512);

  Serial.println("{------");
  Serial.println();

  ssid_eeprom = readStringFromEEPROM(0);
  password_eeprom = readStringFromEEPROM(255);

  Serial.println(ssid_eeprom);
  Serial.println(password_eeprom);

  Serial.println("-------}");

  if (ssid_eeprom == "") {
    server.handleClient();
    Serial.println(" Credenciales  no guardadas");

  }  else {

    Serial.println(" wifi apagada");
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_eeprom, password_eeprom);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    WiFi.setAutoReconnect(true);
    Serial.println("WiFi conectada.");
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.println();
    Serial.print("STA dirección IP: ");
    Serial.println(WiFi.localIP());
    delay(5000);
    return;

  }



  delay(1000);
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"WiFi_SSID\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'John Doe' and 'password123' ...</p>");
}

void handleLogin() {                         // If a POST request is made to URI /login
  if ( ! server.hasArg("WiFi_SSID") || ! server.hasArg("password")
       || server.arg("WiFi_SSID") == NULL ) { // If the POST request doesn't have WiFi_SSID and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  ssid_wifi = String(server.arg("WiFi_SSID"));
  password_wifi = String(server.arg("password"));

  StringToEEPROM(0, ssid_wifi);
  StringToEEPROM(255, password_wifi);


  //  if (EEPROM.commit()) {
  //    Serial.println("datos guardados");
  //  } else {
  //    Serial.println("error en la memoria, no se pudo guardar los datos");
  //
  //  }



  server.send(201, "text/plain", "todo melo");

}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void StringToEEPROM(int offset, const String &str) {
  byte len = str.length();
  EEPROM.write(offset, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(offset + i + 1, str[i]);
    EEPROM.commit();
  }
}

String readStringFromEEPROM(int offset) {
  int len = EEPROM.read(offset);
  int i;
  char data[len + 1];
  for (i = 0; i < len; i++)
    data[i] = EEPROM.read(offset + i + 1);
  data[i] = 0;
  return String(data);
}

void resetMemory() {
  EEPROM.begin(512);
  Serial.println();
  Serial.println("aueires borra la memoria?");
  Serial.println(digitalRead(button));
  digitalWrite(led, HIGH);
  delay(1000);

  if (digitalRead(button) == LOW) {
    delay(3000);
    Serial.println("SI, ha bueno ;)");

    if (digitalRead(button) == LOW) {
      Serial.println("Se esta borrando ");
      for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
      }
      digitalWrite(led, HIGH);
      delay(100);
      digitalWrite(led, LOW);
      delay(100);
      digitalWrite(led, HIGH);
      delay(100);
      digitalWrite(led, LOW);
      //EEPROM.end();

      Serial.println("Se borro la memoria con exito ");
      Serial.println(EEPROM.commit());
      delay(5000);
      //      for (int i = 0; i < 512; i++) {
      //
      //        Serial.println(EEPROM.read(i));
      //      }

    }
  }
  //EEPROM.end();

}
