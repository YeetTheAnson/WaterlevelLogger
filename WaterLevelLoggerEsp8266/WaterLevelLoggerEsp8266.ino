#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>

const char* ssid = "valence@unifi";
const char* password = "ValenceTechnology751016";

const int trigPin = 12;
const int echoPin = 14;

ESP8266WebServer server(80);

float todayData[24] = {0};
float weekData[14] = {0};
float monthData[30] = {0};

const char* todayDataFile = "/todayData.json";
const char* weekDataFile = "/weekData.json";
const char* monthDataFile = "/monthData.json";

void handleRoot();
void handleData();
void handleDistance();
void updateData();
float measureDistance();
void loadData();
void saveData();

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  loadData();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/distance", HTTP_GET, handleDistance);

  server.on("/glyph_rep.svg", HTTP_GET, []() {
    File file = LittleFS.open("/glyph_rep.svg", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "image/svg+xml");
    file.close();
  });

  server.begin();
}

void loop() {
  server.handleClient();
  updateData();
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Water Level</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: rgb(250,239,214);
            color: #333;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-image: url('/glyph_rep.svg');
            background-repeat: repeat;
            background-position: top left;
            background-size: 50px 50px;
            position: relative;
            overflow: hidden; 
        }

        body::after {
            content: "";
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(250,239,214,0.7); 
            pointer-events: none; 
        }

        .spotlight {
            position: absolute;
            width: 200px;
            height: 200px;
            border-radius: 50%;
            background: transparent;
            box-shadow: 0 0 0 100vmax rgba(250,239,214, 0.85); 
            pointer-events: none;
            transform: translate(-50%, -50%);
            mix-blend-mode: screen;
        }

        .container {
            text-align: center;
            width: 80%;
            max-width: 800px;
            margin: 0 auto;
            background-color: rgba(255, 255, 255, 0.7); 
            padding: 20px;
            border-radius: 10px;
            z-index: 1; 
            position: relative;
        }

        h1 {
            margin-bottom: 20px;
        }

        canvas {
            width: 100% !important;
            height: auto !important;
        }

        .buttons {
            margin-top: 20px;
        }

        button {
            margin: 5px;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            border: none;
            border-radius: 5px;
            background-color: #007BFF;
            color: white;
        }

        button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Water Level Monitor</h1>
        <canvas id="waterLevelChart"></canvas>
        <div class="buttons">
            <button onclick="plotToday()">Today</button>
            <button onclick="plotLast7Days()">Past Week</button>
            <button onclick="plotMax()">Past Month</button>
        </div>
    </div>
    <div class="spotlight" id="spotlight"></div>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script>
        const ctx = document.getElementById('waterLevelChart').getContext('2d');
        let waterLevelChart;
        const spotlight = document.getElementById('spotlight');

        function plotData(data, label, xLabels) {
            if (waterLevelChart) {
                waterLevelChart.destroy();
            }

            waterLevelChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: xLabels,
                    datasets: [{
                        label: label,
                        data: data,
                        fill: false,
                        borderColor: 'rgb(75, 192, 192)',
                        tension: 0.1
                    }]
                },
                options: {
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Time'
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: 'Water Level (cm)'
                            },
                            min: 0,
                            max: 350
                        }
                    }
                }
            });
        }

        function fetchData() {
            return fetch('/data')
                .then(response => response.json())
                .catch(error => {
                    console.error('Error fetching data:', error);
                });
        }

        function plotToday() {
            fetchData().then(data => {
                if (!data) return;
                const todayData = data.today;
                const xLabels = Array.from({length: todayData.length}, (_, i) => `${i}:00`);
                plotData(todayData, 'Water Level Today', xLabels);
            });
        }

        function plotLast7Days() {
            fetchData().then(data => {
                if (!data) return;
                const last7DaysData = data.week;
                const xLabels = Array.from({length: last7DaysData.length}, (_, i) => `Day ${Math.floor(i / 2) + 1} - ${i % 2 === 0 ? 'AM' : 'PM'}`);
                plotData(last7DaysData, 'Water Level Over Last 7 Days', xLabels);
            });
        }

        function plotMax() {
            fetchData().then(data => {
                if (!data) return;
                const maxData = data.month;
                const xLabels = Array.from({length: maxData.length}, (_, i) => `Day ${i + 1}`);
                plotData(maxData, 'Water Level Over Last 30 Days', xLabels);
            });
        }

        plotToday();

        document.addEventListener('mousemove', function (e) {
            spotlight.style.left = `${e.clientX}px`;
            spotlight.style.top = `${e.clientY}px`;

            const bgShiftX = e.clientX * 0.05;
            const bgShiftY = e.clientY * 0.05;

            document.body.style.backgroundPosition = `${bgShiftX}px ${bgShiftY}px`;
        });

        document.addEventListener('mouseleave', function () {
            document.body.style.backgroundPosition = 'top left';
        });

        setInterval(() => {
            fetch('/distance')
                .then(response => response.json())
                .then(data => {
                    if (data && data.distance !== undefined) {
                        console.log('Measured Distance:', data.distance);
                    }
                })
                .catch(error => {
                    console.error('Error fetching distance:', error);
                });
        }, 2000);
    </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"today\":"; json += "[";
  for (int i = 0; i < 24; i++) json += (i == 0 ? "" : ",") + String(todayData[i]);
  json += "],";
  json += "\"week\":"; json += "[";
  for (int i = 0; i < 14; i++) json += (i == 0 ? "" : ",") + String(weekData[i]);
  json += "],";
  json += "\"month\":"; json += "[";
  for (int i = 0; i < 30; i++) json += (i == 0 ? "" : ",") + String(monthData[i]);
  json += "]}";
  server.send(200, "application/json", json);
}

void handleDistance() {
  float distance = measureDistance();
  String json = "{";
  json += "\"distance\":"; json += String(distance);
  json += "}";
  server.send(200, "application/json", json);
}


float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.0344) / 2;
  return distance;
}

void loadData() {
  File file;
  
  file = LittleFS.open(todayDataFile, "r");
  if (file) {
    String content = file.readString();
    DynamicJsonDocument json(1024);
    deserializeJson(json, content);
    for (int i = 0; i < 24; i++) todayData[i] = json["today"][i].as<float>();
    file.close();
  }
  
  file = LittleFS.open(weekDataFile, "r");
  if (file) {
    String content = file.readString();
    DynamicJsonDocument json(1024);
    deserializeJson(json, content);
    for (int i = 0; i < 14; i++) weekData[i] = json["week"][i].as<float>();
    file.close();
  }
  
  file = LittleFS.open(monthDataFile, "r");
  if (file) {
    String content = file.readString();
    DynamicJsonDocument json(1024);
    deserializeJson(json, content);
    for (int i = 0; i < 30; i++) monthData[i] = json["month"][i].as<float>();
    file.close();
  }
}

void saveData() {
  File file;
  
  file = LittleFS.open(todayDataFile, "w");
  if (file) {
    DynamicJsonDocument json(1024);
    json["today"] = JsonArray();
    for (int i = 0; i < 24; i++) json["today"].add(todayData[i]);
    serializeJson(json, file);
    file.close();
  }
 
}
