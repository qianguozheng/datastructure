package main

import (
	"flag"
	"fmt"
	"os"
	"net"
	"time"
	"strconv"
)
///参考： https://studygolang.com/articles/6733
var (
	count 		int
	timeout 	int64
	size 		int
	neverstop 	bool
)

func main()  {
	flag.Int64Var(&timeout, "w", 1000, "等待回复的超时时间（毫秒）")
	flag.IntVar(&count, "n", 4, "要发送的回显请求数")
	flag.IntVar(&size, "l", 32, "要发送缓存区大小")
	flag.BoolVar(&neverstop, "t", false, "Ping指定主机，直到停止")

	flag.Parse()

	args := flag.Args()
	if len(args) < 1 {
		fmt.Println("Usage: ", os.Args[0], "host")
		flag.PrintDefaults()
		flag.Usage()
		os.Exit(1)
	}

	ch := make(chan int)
	//argsmap := map[string]interface{}{}
	//
	//argsmap["w"] = timeout
	//argsmap["n"] = count
	//argsmap["l"] = size
	//argsmap["t"] = neverstop

	for _, host := range args {
		go Ping(host, ch)
	}

	for i:=0; i<len(args); i++{
		<-ch
	}

	os.Exit(0)
}

func Ping(host string, c chan int)  {
	cname, _ := net.LookupCNAME(host)
	startTime := time.Now()
	conn, err := net.DialTimeout("ip4:icmp", host, time.Duration(timeout*1000*1000))
	if err != nil {
		fmt.Println(err.Error())
		os.Exit(1)
	}

	ip := conn.RemoteAddr()
	fmt.Println("Ping " + cname + " [" + ip.String() + "]具有32字节数据：" )

	var seq int16 = 1
	id0, id1 := genIdentifier(host)
	const ECHO_REQUEST_HEAD_LEN = 8

	sendN, recvN, lostN := 0, 0, 0
	shortT , longT := -1, -1
	sumT := 0

	for count > 0 || neverstop {
		sendN++
		var msg[]byte = make([]byte, size+ECHO_REQUEST_HEAD_LEN)
		msg[0] = 8  //Echo
		msg[1] = 0  //Code 0
		msg[2] = 0  //checksum
		msg[3] = 0  //checksum
		msg[4], msg[5] = id0, id1 //identifier[0], identifier[1]
		msg[6], msg[7] = genSequence(seq) //sequence[0] sequence[1]

		length := size + ECHO_REQUEST_HEAD_LEN
		check := checkSum(msg[0:length])
		msg[2] = byte(check>>8)
		msg[3] = byte(check & 255)

		conn, err = net.DialTimeout("ip:icmp", host, time.Duration(timeout*1000*1000))
		checkError(err)
		defer conn.Close()

		startTime = time.Now()
		conn.SetDeadline(startTime.Add(time.Duration(timeout*1000*1000)))
		_, err = conn.Write(msg[0:length])

		const ECHO_REPLY_HEAD_LEN = 20

		var receive []byte = make([]byte, ECHO_REPLY_HEAD_LEN+length)
		_, err = conn.Read(receive)
		//_ = n

		var endDuration int = int(int64(time.Since(startTime))/(1000*1000))

		sumT += endDuration
		time.Sleep(1000*1000*1000)

		if err != nil ||
			receive[ECHO_REPLY_HEAD_LEN+4] != msg[4] ||
			receive[ECHO_REPLY_HEAD_LEN+5] != msg[5] ||
			receive[ECHO_REPLY_HEAD_LEN+6] != msg[6] ||
			receive[ECHO_REPLY_HEAD_LEN+7] != msg[7] ||
			endDuration >= int(timeout) ||
			receive[ECHO_REPLY_HEAD_LEN] == 11 {
			lostN++
			fmt.Println("对 " + cname + "[" + ip.String() + "]" + " 的请求超时。")
		} else {
			if shortT == -1 {
				shortT = endDuration
			} else if shortT > endDuration {
				shortT = endDuration
			}
			if longT == -1 {
				longT = endDuration
			} else if longT < endDuration {
				longT = endDuration
			}
			recvN++
			ttl := int(receive[8])
			//			fmt.Println(ttl)
			fmt.Println("来自 " + cname + "[" + ip.String() + "]" + " 的回复: 字节="+ strconv.Itoa(size) +" 时间=" + strconv.Itoa(endDuration) + "ms TTL=" + strconv.Itoa(ttl))
		}

		seq++
		count--
	}
	stat(ip.String(), sendN, lostN, recvN, shortT, longT, sumT)
	c <- 1

}

func genIdentifier(host string) (byte, byte) {
	return host[0], host[1]
}

func genSequence(v int16) (byte, byte) {
	ret1 := byte(v >> 8)
	ret2 := byte(v & 255)
	return ret1, ret2
}

func stat(ip string, sendN int, lostN int, recvN int, shortT int, longT int, sumT int) {
	fmt.Println()
	fmt.Println(ip, " 的Ping统计信息：")
	fmt.Printf("\t数据包： 已发送=%d, 已接受=%d, 丢失=%d(%d%%丢失)，\n",
		sendN, recvN, lostN, int(lostN*100/sendN))
	if recvN != 0 {
		fmt.Printf("\t最短=%dms, 最长=%dms, 平均=%dms\n", shortT, longT, sumT/sendN)
	}
}

func checkSum(msg []byte) uint16  {
	sum := 0
	length := len(msg)
	for i:=0; i<length-1; i+=2 {
		sum += int(msg[i])*256 + int(msg[i+1])
	}
	if length % 2 == 1 {
		sum += int(msg[length-1]) * 256//notice here why *256?
	}

	sum = (sum >> 16) + (sum & 0xffff)
	sum += (sum >> 16)

	var answer uint16 = uint16(^sum)
	return answer
}

func checkError(err error)  {
	if err != nil {
		fmt.Fprintf(os.Stderr, "Fatal error : %s", err.Error())
		os.Exit(1)
	}
}
