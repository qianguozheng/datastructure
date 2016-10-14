package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
)

var inputReader *bufio.Reader
var input string
var err error

func count(all string) {
	char := len(all)
	linearr := strings.Split(all, "\n")
	lines := len(linearr)
	word := 0
	for _, v := range linearr {
		word += len(strings.Fields(v))
	}

	fmt.Printf("Lines:%d, word:%d, char:%d\n", lines, word, char)
}

func uniq(sample []string) {
	newslice := make([]string, 0)

	for _, v := range sample {
		if len(newslice) <= 0 {
			newslice = append(newslice, v)
		}
		for ii, vv := range newslice {

			if 0 == strings.Compare(v, vv) {
				break
			} else if len(newslice) == ii+1 {
				newslice = append(newslice, v)
			}
		}
	}

	for _, v := range newslice {
		fmt.Printf(" %s ", v)
	}
	fmt.Println()
}

func quine() {
	file, err := os.Open("./count.go")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	chunks := make([]byte, 0)
	buf := make([]byte, 1024)
	for {
		n, err := file.Read(buf)
		if err != nil && err != io.EOF {
			panic(err)
		}
		if 0 == n {
			break
		}

		chunks = append(chunks, buf[:n]...)
	}

	fmt.Println(string(chunks))
}

func Handle(conn net.Conn) {

	defer conn.Close()
	//for {
	data := make([]byte, 0)
	buf := make([]byte, 1024)

	for {
		n, err := conn.Read(buf)

		fmt.Printf("length=%d\n", n)

		if err != nil && err != io.EOF {
			return
		}
		data = append(data, buf[:n]...)

		if strings.Contains(string(buf), "\n") {
			break
		}
	}

	cmd := strings.Split(string(data), "\n")
	fmt.Println("String:", cmd[0])
	conn.Write([]byte(cmd[0]))
	//}
}
func echoserver() {
	addr, err := net.ResolveTCPAddr("tcp", ":8053")
	if err != nil {
		return
	}
	listen, err := net.ListenTCP("tcp", addr)
	if err != nil {
		return
	}

	for {
		conn, err := listen.Accept()
		if err != nil {
			return
		}
		go Handle(conn)
	}
}
func main() {
	/*
		inputReader = bufio.NewReader(os.Stdin)
		fmt.Println("Please enter some input:")
		input, err = inputReader.ReadString('S')

		if err == nil {
			fmt.Printf("The input was: %s\n", input)
		}

		count(input)
	*/
	/*
		    var sample = []string{"a", "b", "a", "c", "d", "c"}
			uniq(sample)
	*/

	//quine()

	echoserver()
}
