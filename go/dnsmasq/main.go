package main

import (

	"net/http"
	"fmt"
)

func hello(w http.ResponseWriter, req *http.Request){
	w.Write([]byte("Hello hello"))
}
func say(w http.ResponseWriter, req *http.Request){
	w.Write([]byte("World world..."))
}

func main()  {
	//http.HandleFunc("/hello", hello)
	//http.Handle("/handle", http.HandlerFunc(say))
	//http.ListenAndServe(":80", nil)
	//fmt.Println("After server")
	//
	//select {
	//
	//}
	http.Handle("/", http.StripPrefix("/",
		http.FileServer(http.Dir("/home/weeds/debug"))))
	err := http.ListenAndServe(":80", nil)
	if err != nil {
		fmt.Println("Error", err.Error())
	}
}
