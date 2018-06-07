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

	COMMAND_LOGIN_REQ  = 0x00000010
	COMMAND_LOGIN_RESP = 0x80000010
	// HEART BEAT COMMAND
	COMMAND_HEARTBEAT_REQ  = 0x00000012
	COMMAND_HEARTBEAT_RESP = 0X80000012
	// REBOOT COMMAND
	COMMAND_REBOOT_REQ  = 0x00000100
	COMMAND_REBOOT_RESP = 0X80000100

	// CONFIG READ
	COMMAND_CONFIG_READ_REQ  = 0x00000103
	COMMAND_CONFIG_READ_RESP = 0x80000103

	//UPGRADE COMMAND
	COMMAND_UPGRADE_REQ  = 0x00000102
	COMMAND_UPGRADE_RESP = 0X80000102
	// CC CONFIG COMMAND
	COMMAND_CC_READ_REQ   = 0x00000004
	COMMAND_CC_READ_RESP  = 0X80000004
	COMMAND_CC_WRITE_REQ  = 0x00000104
	COMMAND_CC_WRITE_RESP = 0X80000104

	// WAN WRITE
	COMMAND_WAN_WRITE_REQ  = 0x00000105
	COMMAND_WAN_WRITE_RESP = 0X80000105

	// MODE COMMAND
	COMMAND_MODE_READ_REQ   = 0x00000006
	COMMAND_MODE_READ_RESP  = 0X80000006
	COMMAND_MODE_WRITE_REQ  = 0x00000007
	COMMAND_MODE_WRITE_RESP = 0X80000007

	// VERIFICATION COMMAND
	COMMAND_VERIFICATION_REQ  = 0x00000013
	COMMAND_VERIFICATION_RESP = 0X80000013
	// NOTIFICATION COMMAND
	COMMAND_NOTIFICATION_REQ  = 0x00000101
	COMMAND_NOTIFICATION_RESP = 0X80000101

	//WEB COMMAND
	COMMAND_WEB_READ_REQ   = 0x00000106
	COMMAND_WEB_READ_RESP  = 0X80000106
	COMMAND_WEB_WRITE_REQ  = 0x00000107
	COMMAND_WEB_WRITE_RESP = 0X80000107

	//Resource COMMAND
	COMMAND_RESOURCE_READ_REQ   = 0x00000025
	COMMAND_RESOURCE_READ_RESP  = 0X80000025
	COMMAND_RESOURCE_WRITE_REQ  = 0x00000026
	COMMAND_RESOURCE_WRITE_RESP = 0X80000026

	//RCL
	COMMAND_RCL_REQ  = 0x00000011
	COMMAND_RCL_RESP = 0x80000011
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

	//var i int
	fmt.Println("Read length=", length)
	//for i = 0; i < length; i++ {
	//if length < 8 {
	//	break
	//}

	commandId := BytesToUint(buffer[:4])
	dataLength := BytesToUint(buffer[4:8])
	data := string(buffer[8:])

	fmt.Println(dataLength, commandId, data)
	if int(dataLength) != len(data) {
		fmt.Println("dataLength=", dataLength, "length=", len(data))
		return nil
	}
	fmt.Print("%x", commandId)
	fmt.Println("data=", data)
	readerChannel <- buffer[8:]

	return make([]byte, 0)

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
