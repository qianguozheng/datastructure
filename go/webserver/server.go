package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
	"io/ioutil"
	"bytes"
	"compress/gzip"
	"os"
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

//// 接受http post请求，并且将数据内容写入文件里面
func appendToFile(filename string, content []byte) error {
	f, err := os.OpenFile(filename, os.O_WRONLY, 0644)
	if err != nil{
		fmt.Println("file create failed. err:"+err.Error())
	} else {
		n, _ := f.Seek(0, os.SEEK_END)
		_, err = f.WriteAt(content, n)
	}
	defer f.Close()
	return err
}
type Test string

func (t Test) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	fmt.Println("This is t Test /test")

	result, err := ioutil.ReadAll(r.Body)
	if err != nil{
		fmt.Println("Receive failed")
	}
	r.Body.Close()
	fmt.Println("Length=", len(result), result)
	appendToFile("domainlist.log", result)
	w.Write([]byte("OK"))
}

func main() {
	fmt.Println("Hello World")

	th := &timeHandler{format: time.RFC1123}
	http.Handle("/string", String("I'm a frayed knot."))
	http.Handle("/struct", &Struct{"Hello", ":", "Gophers!"})
	http.Handle("/time", th)
	http.Handle("/rest/gw/1.0/terminal/", String("Received terminal entity"))

	//log.Fatal(http.ListenAndServe("112.74.112.103:80", nil))
	http.Handle("/domainlist", Test("/domainlist"))
	log.Fatal(http.ListenAndServe("192.168.56.41:80", nil))
}
