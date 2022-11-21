#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <EEPROM.h>

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
WiFiClient client;
HTTPClient http;

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
    server.send(200, "text/html", "<h1>hola la servidor de tu producto</h1><p><a href=\"/hola\">precioname para ir a mandar los datas del wifi al que te quieres conectar :)</a></p>");
  });
  server.on("/hola", HTTP_GET, handleRoot);     //el formulario para  ingresar datos, el tercer parametro es la funcino que se ejecutara al ingresar a esa direccion
  server.on("/login", HTTP_POST, handleLogin);  //se manda el formulario con ayuda del navegador y html a esta direccion

  server.begin();  //iniciamos el servidor
  EEPROM.begin(512);

  Serial.println("{------");
  Serial.println();

  ssid_eeprom = readStringFromEEPROM(0);
  password_eeprom = readStringFromEEPROM(255);

  Serial.println(ssid_eeprom);
  Serial.println(password_eeprom);

  Serial.println("-------}");

  connectWifi();
}

void connectWifi() {  //cuando termine esta funcio se hira al loop, pero si no se conecta se llamara a si misma


  if (ssid_eeprom == "") {
    server.handleClient();  //escucha las solicitudes http de los clientes
    Serial.println(" Credenciales  no guardadas");

  } else {

    Serial.println("{------");
    Serial.println();

    //ssid_eeprom = readStringFromEEPROM(0);
    //password_eeprom = readStringFromEEPROM(255);

    Serial.println(ssid_eeprom);
    Serial.println(password_eeprom);

    Serial.println("-------}");


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

    delay(50);
    return;
  }
  return connectWifi();
}

void loop() {  //producto o servicio a ejecutar
  /// codigo
}

void handleRoot() {  // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"WiFi_SSID\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'John Doe' and 'password123' ...</p>");
}

void handleLogin() {                                                                                // If a POST request is made to URI /login
  if (!server.hasArg("WiFi_SSID") || !server.hasArg("password")                                     //toma el los name del formulario y mira si tienen algo , server.hasArg se traduce como tinene argumentos
      || server.arg("WiFi_SSID") == NULL) {                                                         // If the POST request doesn't have WiFi_SSID and password data
    server.send(400, "text/plain", "400: Invalid Request. Porfavor mande un nombre y contraseña");  // The request is invalid, so send HTTP status 400
    return;
  }

  //guardamos el los datos en la variables
  ssid_wifi = String(server.arg("WiFi_SSID"));
  password_wifi = String(server.arg("password"));

  //guardamos el nombre y la contraseña en la EEPROM
  StringToEEPROM(0, ssid_wifi);
  StringToEEPROM(255, password_wifi);

  //los guardamos en las varibles locales
  ssid_eeprom = ssid_wifi;
  password_eeprom = password_wifi;


  server.send(201, "text/plain", "la informacion fue mandada correctamenre :)");
  EEPROM.end();
  delay(8000);  //retraso para poser mandar el mensaje de confirmacion
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
  EEPROM.begin(512);
  boolean encendido = EEPROM.read(250);

  Serial.println("hola");
  Serial.println("hola");

  Serial.println(EEPROM.read(250));
  if (!encendido) {
    EEPROM.write(250, true);
    EEPROM.commit();
    Serial.println("quiere borrar la memoria?");
    delay(500);

    Serial.println("presiona el boton el boton de reset o desconecte la alimentacion ahora si si");
    delay(3000);
    EEPROM.write(250, false);
    EEPROM.commit();
  }

  if (encendido) {

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

String functionGet(String url) {

  String payload = "error de coneccion";

  if ((WiFi.status() == WL_CONNECTED)) {



    //Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url)) {


      //Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      Serial.print(httpCode);
      payload = http.getString();
      Serial.println(payload);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = http.getString();
          //Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        payload = http.errorToString(httpCode).c_str();
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  return payload;
}

String functionPost(String url, String data, String contentType = "application/json") {

  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    //Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, url);  //HTTP
    http.addHeader("Content-Type", contentType);

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST(data);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String &payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
