package main

import (
	"fmt"
	"net"
	"time"

	"./proto"
)

const (
	MaxRead = 1024
)

func sendMessage() {
	conn, err := net.Dial("tcp", "127.0.0.1:4777")

	if err != nil {
		panic("Error")
	}

	for {
		words := "{\"cmd\":\"login\",\"seqId\":\"1234321\",\"Message\":\"message\"}"
		conn.Write(proto.PacketLemon3([]byte(words), 0x34))
		fmt.Println("Send Data Already")

		rbuf := make([]byte, MaxRead+1)
		length, err := conn.Read(rbuf[0 : MaxRead+1])
		if err != nil {
			fmt.Println("Fuck reading ")
			return
		}
		rbuf[MaxRead] = 0

		readerChannel := make(chan []byte, 16)
		go reader(readerChannel)
		tmpBuffer := make([]byte, 0)
		tmpBuffer = proto.UnpackLemon3(append(tmpBuffer, rbuf[:length]...), readerChannel)

		time.Sleep(time.Second * 2)

	}
}
func reader(readerChannel chan []byte) {
	select {
	case data := <-readerChannel:
		fmt.Println("Message: ", string(data))
	}
}

func main() {
	fmt.Println("TCP Client Entity")
	/* Simple TCP Server Implementation
	if ln, err := net.Listen("tcp", ":8080"); err == nil {
		defer ln.Close()
		for {
			ln.Accept()
			fmt.Println("Receive a Message")
		}
	}
	*/

	currency := 1
	count := 1

	for i := 0; i < currency; i++ {
		go func() {
			for j := 0; j < count; j++ {
				sendMessage()
			}
		}()
	}
	select {}
}
