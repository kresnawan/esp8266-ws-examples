package routers

import (
	"log"
	"urbanf/a/internal/handlers"
	"urbanf/a/internal/middleware"

	"github.com/gin-gonic/gin"
)

func RouterInit() {
	gin.SetMode(gin.ReleaseMode)
	Router := gin.Default()

	Router.SetTrustedProxies([]string{"127.0.0.1"})
	Router.Use(middleware.CORSMiddleware())

	v1Router := Router.Group("/v1")
	{
		v1Router.GET("", func(c *gin.Context) {
			c.JSON(200, gin.H{"message": "Hello world"})
		})
		v1Router.GET("/log", func(c *gin.Context) {
			filePath := "app.log"
			c.File(filePath)
		})
		v1Router.GET("/ws", handlers.WsHandler)
	}

	log.Printf("Running server...\n")
	log.Fatal(Router.Run())

}
