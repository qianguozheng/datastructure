package proto

import (
	"bytes"
	"encoding/binary"
	"fmt"
)

/*
typedef struct _Json_Command_Header_ {
	uint32_t total_length;
	int32_t command_id;
} Json_Command_Header;
*/

const (
	ConstHeader         = "www.hiweeds.com"
	ConstHeaderLength   = 15
	ConstSaveDataLength = 4
)

func Packet(messgage []byte) []byte {
	return append(append([]byte(ConstHeader), IntToBytes(len(messgage))...), messgage...)
}
func PacketLemon3(message []byte, cmdId int) []byte {
	return append(append(IntToBytes(cmdId), IntToBytes(len(message))...), message...)
}
func Unpack(buffer []byte, readerChannel chan []byte) []byte {
	length := len(buffer)

	var i int

	for i = 0; i < length; i++ {
		if length < i+len(ConstHeader)+ConstSaveDataLength {
			break
		}

		if string(buffer[i:i+ConstHeaderLength]) == ConstHeader {

			messageLength := BytesToInt(buffer[i+ConstHeaderLength : i+ConstHeaderLength+ConstSaveDataLength])

			fmt.Println("UNPACK Length: ", messageLength)

			if length < i+ConstHeaderLength+ConstSaveDataLength+messageLength {
				break
			}
			data := buffer[i+ConstHeaderLength+ConstSaveDataLength : i+ConstHeaderLength+ConstSaveDataLength+messageLength]
			readerChannel <- data

			i += ConstHeaderLength + ConstSaveDataLength + messageLength - 1
		}

	}

	if i == length {
		return make([]byte, 0)
	}
	return buffer[i:]
}

const (
	//ConstHeader         = "www.hiweeds.com"
	//ConstHeaderLength     = 15
	ConstSaveHeaderLength = 8
)

func UnpackLemon3(buffer []byte, readerChannel chan []byte) []byte {
	length := len(buffer)

	/*
	   typedef struct _Json_Command_Header_ {
	   	uint32_t total_length;
	   	int32_t command_id;
	   } Json_Command_Header;
	*/

	var i int

	for i = 0; i < length; i++ {
		if length < 8 {
			break
		}

		dataLength := BytesToUint(buffer[:4])
		commandId := BytesToUint(buffer[4:8])
		data := string(buffer[i+8:])
		if int(dataLength) != len(data) {
			fmt.Println("dataLength=", dataLength, "length=", len(data))
			return nil
		}
		fmt.Print("%x", commandId)
		fmt.Println("data=", data)
		readerChannel <- buffer[i+8:]
		/*
			if string(buffer[i:i+ConstHeaderLength]) == ConstHeader {

				messageLength := BytesToInt(buffer[i+ConstHeaderLength : i+ConstHeaderLength+ConstSaveDataLength])

				fmt.Println("UNPACK Length: ", messageLength)

				if length < i+ConstHeaderLength+ConstSaveDataLength+messageLength {
					break
				}
				data := buffer[i+ConstHeaderLength+ConstSaveDataLength : i+ConstHeaderLength+ConstSaveDataLength+messageLength]
				readerChannel <- data

				i += ConstHeaderLength + ConstSaveDataLength + messageLength - 1
			}
		*/
	}

	if i == length {
		return make([]byte, 0)
	}
	return buffer[i:]
}

func IntToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

func BytesToInt(b []byte) int {
	bytesBuffer := bytes.NewBuffer(b)

	var x int32
	binary.Read(bytesBuffer, binary.BigEndian, &x)
	return int(x)
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
	return uint32(x)
}
