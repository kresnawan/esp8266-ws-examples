package main

import (
	// "urbanf/a/internal/handlers"
	"log"
	"os"
	"urbanf/a/internal/routers"
)

// Upgrader is used to upgrade HTTP connections to WebSocket connections.

func main() {
	file, err := os.OpenFile("app.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if err != nil {
		log.Fatal("Failed to open the log file: ", err)
	}
	defer file.Close()

	log.SetOutput(file)
	routers.RouterInit()

	// go handlers.HandleMessage()
}
