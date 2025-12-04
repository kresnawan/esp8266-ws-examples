package handlers

import (
	"fmt"
	"log"
	"net/http"
	"sync"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

var clients = make(map[*websocket.Conn]bool)
var nodeMCUClient = make(map[*websocket.Conn]bool)

var mutex = &sync.Mutex{}

func WsHandler(c *gin.Context) {
	// Upgrade the HTTP connection to a WebSocket connection
	conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)

	if err != nil {
		fmt.Println("Error upgrading:", err)
		return
	}
	defer conn.Close()
	// Listen for incoming messages

	pingPeriod := 30 * time.Second
	ticker := time.NewTicker(pingPeriod)
	defer ticker.Stop()

	pongWait := 60 * time.Second
	conn.SetReadDeadline(time.Now().Add(pongWait))
	conn.SetPongHandler(func(string) error {
		conn.SetReadDeadline(time.Now().Add(pongWait))
		return nil
	})

	done := make(chan struct{})

	go func() {
		defer close(done)
		var counter int = 0
		for {
			select {
			case <-ticker.C:
				if err := conn.WriteMessage(websocket.PingMessage, []byte{}); err != nil {
					log.Println("Ping error: ", err)
					return
				}

				if nodeMCUClient[conn] && counter == 10 {
					log.SetPrefix("[PING] ")
					log.Printf("NodeMCU masih terkoneksi\n")
					counter = 0
				} else if nodeMCUClient[conn] && counter != 10 {
					counter++
				}

			case <-done:
				return
			}

		}
	}()

	mutex.Lock()
	log.SetPrefix("[KONEKSI] ")

	if c.Request.Header.Get("Authorization") == "device" {
		nodeMCUClient[conn] = true
		log.Print("NodeMCU terkoneksi")
	} else {
		clients[conn] = true
		log.Print("Client terkoneksi")
	}

	mutex.Unlock()

	for {
		// Read message from the client
		_, message, err := conn.ReadMessage()

		if err != nil {
			log.SetPrefix("[ERROR] ")
			mutex.Lock()

			if clients[conn] {
				delete(clients, conn)
				log.Printf("Gagal membaca pesan atau putus koneksi dari client\n")
			}

			if nodeMCUClient[conn] {
				delete(nodeMCUClient, conn)
				log.Printf("Gagal membaca pesan atau putus koneksi dari NodeMCU\n")
			}

			conn.Close()

			mutex.Unlock()
			break
		}

		log.SetPrefix("[SWITCH] ")
		if string(message) == "ON" {
			log.Printf("ON\n")
		} else if string(message) == "OFF" {
			log.Printf("OFF\n")
		} else if string(message) == "DC" {
			log.Printf("Mematikan NodeMCU, segera koneksi ulang..")
		}

		// Kirim pesan ke NodeMCU
		for client := range nodeMCUClient {
			err := client.WriteMessage(websocket.TextMessage, message)
			if err != nil {
				log.SetPrefix("[ERROR] ")
				log.Print("Gagal mengirim pesan ke NodeMCU")
				break
			}
		}
	}
}
