package proto

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"strconv"
)

const (
//ConstHeader         = "www.hiweeds.com"
//ConstCommandLength   = 4
//ConstSaveDataLength = 4
)

type RespParam struct {
	Cmd   string `json:"cmd"`
	Code  string `json:"code"`
	SeqId string `json:"seqId"`
	Data  []string `json:"data"`
}

type VerificationData struct{
	TerminalMac string `json:"terminalMac"`
	Valid int `json:"valid"`
	AuthType int `json:"authType"`
	AuthId string `json:"authId"`
	UpRate int `json:"upRate"`
	DownRate int `json:"downRate"`
	TcpLimit int `json:"tcpLimit"`
	UdpLimit int `json:"udpLimit"`
}

type Verification struct {
	Cmd string `json:"cmd"`
	SeqId string `json:"seqId"`
	Code string `json:"code"`
	Data VerificationData `json:"data"`
}

type ReqParam struct {
	Cmd string `json:"cmd"`
	SeqId string `json:"seqId"`
	Mac string `json:"mac"`
}

type WebReadData struct {
	Ver string `json:"ver"`
	Url string `json:"url"`
	Md5 string `json:"md5"`
}
type WebRead struct {
	Cmd   string `json:"cmd"`
	Code  string `json:"code"`
	SeqId string `json:"seqId"`
	Data WebReadData `json:"data"`
}

type WebWrite struct {
	Cmd string `json:"cmd"`
	SeqId string `json:"seqId"`
	Mac string `json:"mac"`
	Ver int `json:"ver"`
	Url string `json:"url"`
	Md5 string `json:"md5"`
}

type DnsBogus struct {
	Domain string `json:"domain"`
	Host string `json:"host"`
}
type DnsBogusWrite struct {
	Cmd string `json:"cmd"`
	SeqId string `json:"seqId"`
	Mac string `json:"mac"`
	Bogus []DnsBogus `json:"bogus"`
}

var CmdKV map[string]uint32

func InitKeyValue(){
	CmdKV = make(map[string]uint32)
	CmdKV["login"] = 0x80000010
	CmdKV["heartbeat"] = 0X80000012
	CmdKV["rcl"] =0x80000011
	CmdKV["status"] =0X80000022
	CmdKV["config"] = 0x80000103
	CmdKV["web_read_resp"]=0X80000106
	CmdKV["web_write_resp"]=0X80000107
	CmdKV["resource_read_resp"]=0X80000108
	CmdKV["resource_write_resp"]=0X80000109
	CmdKV["verification"] = 0x80000013

	CmdKV["web_read_req"]=0X00000106
	CmdKV["web_write_req"]=0X00000107
	CmdKV["resource_read_req"]=0X00000108
	CmdKV["resource_write_req"]=0X00000109
	CmdKV["dns_bogus_write_req"]=0x00000111
	CmdKV["config_read_req"]=0x00000103

}

func PacketLemon3(messgage []byte, cmd uint32) []byte {
	return append(append(IntToBytes(len(messgage)), UintToBytes(cmd)...), messgage...)
}

func UnpackLemon3(buffer []byte, readerChannel chan []byte) []byte {
	//length := len(buffer)

	totalLength := buffer[0:4]
	cmdId := buffer[4:8]
	length, err := strconv.ParseInt(fmt.Sprintf("%x", totalLength), 16, 0)
	if err != nil{
		fmt.Println(err.Error())
		return nil
	}
	fmt.Printf("[FromClient]= [%d] [0x%x] [%s]\n", length, cmdId,  string(buffer[8:length+8]))
	//message := string(buffer[8:length+8])
	//if (length == int(BytesToUint(totalLength))){
	readerChannel <- buffer[8:length+8]
	//}
	return buffer[length+8:]
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
