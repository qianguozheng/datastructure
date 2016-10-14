package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"strings"
)

var name string
var emptyname string = ""

func main() {
	a, b, v := "hello3456345", "world", "shit"
	fmt.Println(a, b, v)

	c := []byte(a)
	c[0] = 'n'

	d := string(c)
	fmt.Println(d)

	m := `hello world`
	fmt.Println(m)

	//Byte to INT
	byteData := []byte{0x00, 0x00, 0x03, 0xe8}
	byteBuf := bytes.NewBuffer(byteData)

	var x int32
	binary.Read(byteBuf, binary.BigEndian, &x)
	fmt.Println(x)

	fmt.Println(strings.Repeat("-", 100))

	x = 100
	byteBuf = bytes.NewBuffer([]byte{})
	binary.Write(byteBuf, binary.BigEndian, x)
	fmt.Println(byteBuf.Bytes())
}
