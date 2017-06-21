package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
	"io/ioutil"
	"bytes"
	"compress/gzip"
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

	result, err := ioutil.ReadAll(r.Body)
	if err != nil{
		fmt.Println("1 err=", err.Error())
	}
	r.Body.Close()
	fmt.Println("length=", len(result), result)
	g, err:= gzip.NewReader(bytes.NewBuffer(result))
	if err != nil{
		fmt.Println("2 err=", err.Error())
	}
	defer g.Close()
	data,err:= ioutil.ReadAll(g)
	if err != nil{
		fmt.Println("3 err=", err.Error())
	}
	fmt.Println("ungzip size=", len(data))
	fmt.Println("data=", string(data))
	str = (string)(s)

	w.Write([]byte(str))
	
}

func main() {
	fmt.Println("Hello World")

	th := &timeHandler{format: time.RFC1123}
	http.Handle("/string", String("I'm a frayed knot."))
	http.Handle("/struct", &Struct{"Hello", ":", "Gophers!"})
	http.Handle("/time", th)
	http.Handle("/rest/gw/1.0/terminal/", String("Received terminal entity"))

	log.Fatal(http.ListenAndServe("112.74.112.103:80", nil))
}
