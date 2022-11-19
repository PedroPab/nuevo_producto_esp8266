#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define button D4
#define led LED_BUILTIN

//Es el nombre y la contraseña de la nueva red Wifi
const char *ssid_AP = "Virus_Gratis";
const char *password_AP = "12345678";

//inicialisamos las variables para ser usadas con la blibloteca EEPROM
String password_wifi;
String ssid_wifi;

String password_eeprom;
String ssid_eeprom;

boolean starts = false;

unsigned long tiempo_espera = 15000;  //nuestro tiempo maximo de espera para conectarnos al wifi en milis

//Establecemos el puerto 80
ESP8266WebServer server(80);

void setup() {

  Serial.begin(9600);
  resetMemory();

  //iniciamos el modo SoftAP
  WiFi.mode(WIFI_AP);
  //establecemos el nombre y contraseña
  WiFi.softAP(ssid_AP, password_AP);

  //imprimimos datos
  Serial.println("punto de acceso iniciada");
  WiFi.printDiag(Serial);
  Serial.print("AP dirección IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("");

  //establecmos las peticiones como en express
  server.on("/", []() {
    server.send(200, "text/plain", "Hola mundo!!");
  });
  server.on("/hola", HTTP_GET, handleRoot);     //el formulario para  ingresar datos, el tercer parametro es la funcino que se ejecutara al ingresar a esa direccion
  server.on("/login", HTTP_POST, handleLogin);  //se manda el formulario con ayuda del navegador y html a esta direccion

  server.begin();  //iniciamos el servidor
  connectWifi();
}

void connectWifi() {  //cuando termine esta funcio se hira al loop, pero si no se conecta se llamara a si misma

  EEPROM.begin(512);

  Serial.println("{------");
  Serial.println();

  ssid_eeprom = readStringFromEEPROM(0);
  password_eeprom = readStringFromEEPROM(255);

  Serial.println(ssid_eeprom);
  Serial.println(password_eeprom);

  Serial.println("-------}");

  if (ssid_eeprom == "") {
    server.handleClient();  //escucha las solicitudes http de los clientes
    Serial.println(" Credenciales  no guardadas");

  } else {

    Serial.println("punto de acceso apagada");
    WiFi.softAPdisconnect();  //apagamos el punto de acceso

    WiFi.mode(WIFI_STA);                       //iniciamos el modo estacion
    WiFi.begin(ssid_eeprom, password_eeprom);  //nos conectamos con la contrasña guardad de la memoria EEPROM

    unsigned long contadorIntentos = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      contadorIntentos++;
      if (contadorIntentos > millis() + tiempo_espera) {
        Serial.print("tiempo maximo transcurrido");

        return connectWifi();
      }
    }

    WiFi.setAutoReconnect(true);
    Serial.println("WiFi conectada.");
    WiFi.printDiag(Serial);
    Serial.println("");
    Serial.print("STA dirección IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("podemos ejecutar fuciones para nuestro producto");

    delay(5000);
    return;
  }
}

void loop() {//producto o servicio a ejecutar
}

void handleRoot() {  // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"WiFi_SSID\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'John Doe' and 'password123' ...</p>");
}

void handleLogin() {                                             // If a POST request is made to URI /login
  if (!server.hasArg("WiFi_SSID") || !server.hasArg("password")  //toma el los name del formulario y mira si tienen algo , server.hasArg se traduce como tinene argumentos
      || server.arg("WiFi_SSID") == NULL) {                      // If the POST request doesn't have WiFi_SSID and password data
    server.send(400, "text/plain", "400: Invalid Request");      // The request is invalid, so send HTTP status 400
    return;
  }

  //guardamos el los datos en la variables
  ssid_wifi = String(server.arg("WiFi_SSID"));
  password_wifi = String(server.arg("password"));

  //guardamos el nombre y la contraseña en la EEPROM
  StringToEEPROM(0, ssid_wifi);
  StringToEEPROM(255, password_wifi);

  server.send(201, "text/plain", "todo melo");
  EEPROM.end();
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void StringToEEPROM(int offset, const String &str) {  //esta funcion la saque de internet y es muy util, me ayuda a leer y escribir string del la memoria EEPROM
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
  pinMode(button, INPUT_PULLUP);
  EEPROM.begin(512);
  Serial.println("quiere borrar la memoria?");
  delay(500);
  Serial.println("presiona el boton si si");
  delay(3000);
  Serial.println("estado del boton " + String(digitalRead(button)));

  if (digitalRead(button) != LOW) {

    Serial.println("Si, ha bueno ;)");
    Serial.println("Se esta borrando... ");
    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
      Serial.print(".");
    }
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);


    Serial.println("Se borro la memoria con exito ");
    Serial.println(EEPROM.commit());
    delay(5000);
  }
  EEPROM.end();
}
