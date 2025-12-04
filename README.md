# esp8266-ws-examples
Since there is no single official WebSocket library for NodeMCU with ESP8266 module, I decided to create this repository to help anyone who struggled with the topic. Actually, there are a lot of open-source projects which create the WebSocket library but after I tried several of them, not all of them are reliable (I don't know whether it is my skill issue or what) but I eventually find one that works fine for my module.

The module used for this implementation : ESP-12E, Model ESP8266MOD, ISM 2.4GHz with CH340C driver, NodeMCU V3<br>
Kudos to gilmaimon and everyone for making the library :
https://github.com/gilmaimon/ArduinoWebsockets

# Scenario
Here, the Go app acts as the server while the NodeMCU acts as a client, the NodeMCU connection sent with the Authorization header "device" which tell the server that it is NodeMCU (Not recommended for production, go with safer way with JWT or something).

The server then checks the header and put NodeMCU connection on separate variable from the regular connection (from postman or stuff). As a test, here whenever a regular client send a message "ON" or "OFF", the NodeMCU will receive it and lights up or turn off the LED, then the NodeMCU send a feedback to the server about current condition to ensure that the condition is controlled.

In my opinion, this bi-directional full-duplex communication is a better way to transmit real-time data rather than periodically sending a regular HTTP request regarding the real-timeness and interactiveness. And with this basics, it can be crafted for more advanced system based on your needs with IoT in mind.

There's still an issue about the connection reliability (i've tested it and it disconnects sometime) and I'm currently working on it
