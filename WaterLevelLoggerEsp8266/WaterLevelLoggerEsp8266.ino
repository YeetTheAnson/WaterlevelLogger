#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "SSID";
const char* password = "Password";

const int trigPin = 12;  // D6
const int echoPin = 14;  // D5

ESP8266WebServer server(80);

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

  server.on("/", HTTP_GET, handleRoot);

  server.begin();
}

void loop() {
  server.handleClient();
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
            background-image: url('glyph_rep.svg');
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

        function plotToday() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    const todayData = data.today;
                    const xLabels = Array.from({length: todayData.length}, (_, i) => `${i}:00`);
                    plotData(todayData, 'Water Level Today', xLabels);
                });
        }

        function plotLast7Days() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    const last7DaysData = data.week;
                    const xLabels = Array.from({length: last7DaysData.length}, (_, i) => `Day ${Math.floor(i / 2) + 1} - ${i % 2 === 0 ? 'AM' : 'PM'}`);
                    plotData(last7DaysData, 'Water Level Over Last 7 Days', xLabels);
                });
        }

        function plotMax() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
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

        // Fetch distance every 2 seconds for debugging
        setInterval(() => {
            fetch('/distance')
                .then(response => response.json())
                .then(data => {
                    console.log('Measured Distance:', data.distance);
                });
        }, 2000);
    </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
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

void updateData() {
  static float todayData[24] = {0};
  static float weekData[14] = {0};
  static float monthData[30] = {0};
  
  unsigned long currentMillis = millis();
  static unsigned long lastUpdate = 0;
  

  server.on("/distance", HTTP_GET, [distance]() {
    String json = "{ \"distance\": " + String(distance) + " }";
    server.send(200, "application/json", json);
  });
  }
}
