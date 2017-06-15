package proto

import (
	"bytes"
	"encoding/binary"
	"fmt"
)

const (
//ConstHeader         = "www.hiweeds.com"
//ConstCommandLength   = 4
//ConstSaveDataLength = 4
)

func PacketLemon3(messgage []byte, cmd uint32) []byte {
	return append(append(UintToBytes(cmd), IntToBytes(len(messgage))...), messgage...)
}

func UnpackLemon3(buffer []byte, readerChannel chan []byte) []byte {
	//length := len(buffer)

	cmdId := buffer[0:4]
	totalLength := buffer[4:8]

	fmt.Println("cmdId=", cmdId, " totalLength=", totalLength, " message=", string(buffer[8:]))
	//CmdId is a valid command ?
	message := string(buffer[8:])
	if (len(message) == int(BytesToUint(totalLength))){
		readerChannel <- buffer[len(message)+8:]
	}
	return buffer[len(message)+8:]
}

func UintToBytes(n uint32) []byte {
	x := uint32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

func BytesToUint(b []byte) uint32 {
	bytesBuffer := bytes.NewBuffer(b)

	var x uint32
	binary.Read(bytesBuffer, binary.BigEndian, &x)
	return x
}
