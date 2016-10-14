package main

import (
	"fmt"
	"net"
)

func echoclient() {
	fmt.Println("Echo Client")
	conn, err := net.Dial("tcp", "127.0.0.1:8053")

	if err != nil {
		panic("Error")
	}

	buf := make([]byte, 1024)
	conn.Write([]byte("Hello World66\n"))
	length, err := conn.Read(buf)
	if err != nil {
		return
	}
	fmt.Println("Length:", length)
	fmt.Println(string(buf))

}
func main() {
	echoclient()
	//select {}
}
