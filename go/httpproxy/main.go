package main

import (
	"fmt"
	"net/http"
	"time"
)

type customHandler struct {
}

func (cb *customHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Custom Handler")
	fmt.Println("url=", r.URL)
	fmt.Println("host=", r.Host)
	fmt.Println("header=", r.Proto)

	w.Write([]byte("CustomHander!!!"))
}

func test() {
	var server *http.Server = &http.Server{
		Addr:           ":8011",
		Handler:        &customHandler{},
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}

	server.ListenAndServe()
	fmt.Println("Server listen to socket")
	select {}
}
func main() {
	fmt.Println("HTTP Proxy server written by Richard Qian")
	test()
}
