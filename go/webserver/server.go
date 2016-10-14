package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
)

type String string

type Struct struct {
	Greeting string
	Punct    string
	Who      string
}

type timeHandler struct {
	format string
}

func (th *timeHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	tm := time.Now().Format(th.format)
	w.Write([]byte("The time is: " + tm))
}

func (s *Struct) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte(s.Greeting + s.Punct + s.Who))
}

func (s String) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	//w.Write([]byte())
	var str string
	str = (string)(s)
	w.Write([]byte(str))
}

func main() {
	fmt.Println("Hello World")

	th := &timeHandler{format: time.RFC1123}
	http.Handle("/string", String("I'm a frayed knot."))
	http.Handle("/struct", &Struct{"Hello", ":", "Gophers!"})
	http.Handle("/time", th)

	log.Fatal(http.ListenAndServe("localhost:4000", nil))
}
