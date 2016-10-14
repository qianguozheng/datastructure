package server

import (
	"encoding/json"
	"fmt"
	"net"

	"../proto"
)

//http://www.01happy.com/golang-tcp-socket-adhere/

const (
	MaxRead = 1024 * 1024 //1MB
)

//Listen to socket and return *net.TCPListener
func server(host string) *net.TCPListener {

	server, err := net.ResolveTCPAddr("tcp", host)
	checkError(err, "Resolving address:port failed: "+host)

	listener, err := net.ListenTCP("tcp", server)
	checkError(err, "ListenTCP: ")

	println("Listening to: ", listener.Addr().String())
	return listener
}

// TCP Server startup
func Main() {

	HostAndPort := "localhost:4777"
	listener := server(HostAndPort)
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		checkError(err, "Accept: ")

		go connectionHandler(conn)
	}
}

// Handle Each client request
func connectionHandler(conn net.Conn) {
	connFrom := conn.RemoteAddr().String()
	println("Connection from: ", connFrom)

	m := func(conn net.Conn) {
		err := conn.Close()
		fmt.Printf("Closing connection: %s\n", connFrom)
		checkError(err, "Close:")
	}
	defer m(conn)

	//talktoclients(conn)

	for {
		ibuf := make([]byte, MaxRead+1)

		length, err := conn.Read(ibuf[0 : MaxRead+1])
		//checkError(err, "Read From client failed")
		if err != nil {
			fmt.Printf("ERROR %s\n", err.Error())
			return
		}
		ibuf[MaxRead] = 0

		readerChannel := make(chan []byte, 16)

		//reader get the parsed message from
		go reader(readerChannel, conn)

		//TCP粘包问题需要解决
		tmpBuffer := make([]byte, 0)
		for {
			tmpBuffer = proto.Unpack(append(tmpBuffer, ibuf[:length]...), readerChannel)
			if len(tmpBuffer) > 0 {
				tmpBuffer = proto.Unpack(append(tmpBuffer, ibuf[:length]...), readerChannel)
			} else {
				break
			}
		}

		fmt.Println("Ping!")
	}
}

func handleMsg(msg []byte) string {
	var dat map[string]interface{}
	if err := json.Unmarshal(msg, &dat); err == nil {
		fmt.Println("==============json str 转map=======================")
		fmt.Println(dat)
		fmt.Println("Id=", dat["Id"])
		fmt.Println("Name=", dat["Name"])
		fmt.Println("Message=", dat["Message"])
	}
	return "{\"CMD\":\"OK\"}"
}

func talktoclients(to net.Conn, msg string) {
	//data := "Hello Client"
	wrote, err := to.Write(proto.Packet([]byte(msg)))
	checkError(err, "Write: wrote "+string(wrote)+" bytes.")
}

func reader(readerChannel chan []byte, conn net.Conn) {
	for {
		select {
		case data := <-readerChannel:
			fmt.Println("Message: ", string(data))
			msg := handleMsg(data)
			talktoclients(conn, msg)
		}
	}
}

func checkError(error error, info string) {
	if error != nil {
		panic("ERROR: " + info + " " + error.Error())
	}
}
