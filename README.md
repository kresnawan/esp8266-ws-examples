# esp8266-ws-examples
Since there is no single official WebSocket library for NodeMCU with ESP8266 module, I decided to create this repository to help anyone who struggled with the topic. Actually, there are a lot of open-source projects which create the WebSocket library but after I tried several of them, not all of them are reliable (I don't know whether it is my skill issue or what) but I eventually find one that works fine for my module.

The module used for this implementation : ESP-12E, Model ESP8266MOD, ISM 2.4GHz with CH340C driver<br>

# Scenario
Here, the Go app acts as the server while the NodeMCU acts as a client, the NodeMCU connection sent with the Authorization header "device" which tell the server that it is NodeMCU (Not recommended for production, go with safer way with JWT or something).

The server then checks the header and put NodeMCU connection on separate variable from the regular connection (from postman or stuff). As a test, here whenever a regular client send a message "ON" or "OFF", the NodeMCU will receive it and lights up or turn off the LED.

This bi-directional full-duplex communication is a better way to transmit real-time data rather than periodically sending a regular HTTP request regarding the real-timeness and interactiveness.

Beside that, this enables us to control the microcontroller without having to hardcode it. But, it invoked a new problem, these works as long as the connectivity is reliable for long time. That explain the goals of this project, to make a reliable connection.

There's still an issue about the connection reliability (i've tested it and it disconnects sometime) and I'm currently working on it, feel free to open a pull request if you interested in this topic as well.

# Dependencies
`NTPClient.h` : https://github.com/arduino-libraries/NTPClient
`ArduinoWebsockets.h` : https://github.com/gilmaimon/ArduinoWebsockets
`ESP8266WiFi.h` : https://github.com/esp8266/Arduino

# Getting started
In root directory, run `go run ./cmd/api` to start the WebSocket server
Use tunneling like `ngrok` to get your server accessible for debugging
Set the SSID, password, and server URL on `./board/main/main.ino` matching yours
Compile the program to your NodeMCU.