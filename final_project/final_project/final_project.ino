#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>


const char* ssid="IZZI-B428";
const char* password="F82DC039B428";
int LED=2;
int websockMillis=50;

const int trigP = 2;  //D4 Or GPIO-2 of nodemcu
const int echoP = 0;  //D3 Or GPIO-0 of nodemcu

long duration;
int distance;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 500;  //the value is a number of mill

ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite,JSONtxt;


const char webSiteCont[] PROGMEM = 
R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Proyecto instrumentación</title>

    <style>
        html {
            margin: 0px;
            padding: 0px;
        }

        .bar {
            width:0px;
            height:12px;
            top: 9px;
            background-color: blue;
            margin-top:8px;
        }

        #rectangle{
            width: 1401px;
            
            background-image: 
                linear-gradient(90deg, 
                rgba(73, 73, 73, 0.5) 0%, 
                rgba(73, 73, 73, 0.5) 1%, 
                transparent 1%
                ),linear-gradient(180deg, 
                #ffffff 50%, 
                transparent 50%
                ), 
                linear-gradient(90deg, 
                transparent 50%, 
                rgba(73, 73, 73, 0.5) 50%, 
                rgba(73, 73, 73, 0.5) 52%, 
                transparent 52%
                ), 
                linear-gradient(180deg, 
                #ffffff 70%, 
                transparent 70%
                ), 
                linear-gradient(90deg, 
                transparent 10%,
                rgba(73, 73, 73, 0.4) 10%, 
                rgba(73, 73, 73, 0.4) 12%, 
                transparent 12%, 
                
                transparent 20%,
                rgba(73, 73, 73, 0.4) 20%, 
                rgba(73, 73, 73, 0.4) 22%, 
                transparent 22%, 
                
                transparent 30%, 
                rgba(73, 73, 73, 0.4) 30%,
                rgba(73, 73, 73, 0.4) 32%, 
                transparent 32%, 
                
                transparent 40%, 
                rgba(73, 73, 73, 0.4) 40%, 
                rgba(73, 73, 73, 0.4) 42%, 
                transparent 42%, 
                
                transparent 60%, 
                rgba(73, 73, 73, 0.4) 60%, 
                rgba(73, 73, 73, 0.4) 62%, 
                transparent 62%, 
                
                transparent 70%, 
                rgba(73, 73, 73, 0.4) 70%, 
                rgba(73, 73, 73, 0.4) 72%, 
                transparent 72%, 
                
                transparent 80%, 
                rgba(73, 73, 73, 0.4) 80%, 
                rgba(73, 73, 73, 0.4) 82%, 
                transparent 82%, 
                
                transparent 90%, 
                rgba(73, 73, 73, 0.4) 90%, 
                rgba(73, 73, 73, 0.4) 92%, 
                transparent 92%
                );
            
        
            background-size: 140px 40px;
            min-height: 40px;
            
            /* only needed for labels */
            white-space:nowrap;
            font-size:0;
            margin:auto;
            margin-top: 100px;
            padding:0;
        }

        label {
            font-size:16px;
            padding-top:2px;
            display:inline-block;
            width:140px;
            text-indent:3px;
        }
    </style>
</head>

<body>

    <div id="rectangle">
        <label>0</label>
        <label>100</label>
        <label>200</label>
        <label>300</label>
        <label>400</label>
        <label>500</label>
        <label>600</label>
        <label>700</label>
        <label>800</label>
        <label>900</label>
        <label>1000</label>
        <div class="bar" id="bar"></div>
    </div>

    <div id="rectangle">
        <label>0</label>
        <label>100</label>
        <label>200</label>
        <label>300</label>
        <label>400</label>
        <label>500</label>
        <label>600</label>
        <label>700</label>
        <label>800</label>
        <label>900</label>
        <label>1000</label>
        <div class="bar" id="bar2"></div>
    </div>

    <div id="rectangle">
        <div class="bar" id="bar3"></div>
    </div>

    <script>
        var counter = 0;
        var time = 0;
        var a = 0;
        InitWebSocket()

        function InitWebSocket() {
            websock = new WebSocket('ws://' + window.location.hostname + ':88/');
            websock.onmessage = function (evt) {
                JSONobj = JSON.parse(evt.data);
                var dist = parseInt(JSONobj.Distance);
                document.getElementById("bar").style.width = dist + "px";
            }
        } 

        var band = false;

        document.getElementById("bar3").style.width = '100%';
        document.getElementById("bar2").style.width = '100%';

        counter = (Math.floor(Math.random() * 10) * 100)
        setInterval(() => {
            document.getElementById("bar2").style.width = counter + 'px';
            counter--
            // max();
            if (counter <= 100) {
                counter = (Math.floor(Math.random() * 10) * 100);
            }
        }, 5);

        setInterval(() => {
            initWebsocket3();
        }, 1000);

        function max() {
            setTimeout(() => {
                
            }, 50)
        }

        function initWebsocket3() {
            setTimeout(() => {
                document.getElementById("bar3").style.backgroundColor = band ? 'green' : 'yellow'
            }, 900)
            band = !band
        }
    </script>
</body>

</html>
)=====";
void WebSite(){

  server.send(400,"text/html",webSiteCont);
  
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(500);  
  }
  WiFi.mode(WIFI_STA);
  Serial.println(" Start ESP ");
  Serial.println(WiFi.localIP());
  server.on("/",WebSite);
  server.begin();
  webSocket.begin();

  pinMode(trigP, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoP, INPUT);   // Sets the echoPin as an Input
  startMillis = millis();  //initial start time 
}

int dist = 0;


void loop() 
{
  webSocket.loop();
  server.handleClient();

  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    digitalWrite(trigP, LOW);   // Makes trigPin low
    delayMicroseconds(2);       // 2 micro second delay 
    
    digitalWrite(trigP, HIGH);  // tigPin high
    delayMicroseconds(10);      // trigPin high for 10 micro seconds
    digitalWrite(trigP, LOW);   // trigPin low
    
    duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
    distance= duration*0.034/2;        //Calculating actual/real distance
    
    //Serial.print("Distance = ");        //Output distance on arduino serial monitor 
    Serial.println(distance);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.

  }

  
  //dist = distance * 1000 / 22;

  if(dist > (distance * 1400 / 22) )
      dist = dist - 1;
  else if(dist < (distance * 1400 / 22) )
      dist = dist + 1;
  else
      dist = (distance * 1400 / 22);

   if(distance > 22)
      dist = 1400;
      
  //if(dist == 1000)
  //  dist = 0;
    
  //Serial.println(dist);
  String distaceStr = String(dist);
  JSONtxt = "{\"Distance\":\""+distaceStr+"\"}";
  webSocket.broadcastTXT(JSONtxt);

}
