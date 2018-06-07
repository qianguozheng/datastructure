package server

import (
	"encoding/json"
	"fmt"
	"net"
	"time"

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

	HostAndPort := "192.168.0.51:37001"
	listener := server(HostAndPort)
	defer listener.Close()

	proto.InitKeyValue()
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

	//talktoclients(conn, "{\"result\":\"ok\"}")

	for {
		ibuf := make([]byte, MaxRead+1)

		length, err := conn.Read(ibuf[0 : MaxRead+1])
		//checkError(err, "Read From client failed")
		if err != nil {
			fmt.Printf("ERROR %s\n", err.Error())
			return
		}
		ibuf[MaxRead] = 0
		fmt.Println("length=", length)

		readerChannel := make(chan []byte, 16)

		//reader get the parsed message from
		go reader(readerChannel, conn)

		//TCP粘包问题需要解决
		tmpBuffer := make([]byte, 0)
		tmpBuffer = append(tmpBuffer, ibuf[:length]...)
		for {
			//tmpBuffer = proto.Unpack(append(tmpBuffer, ibuf[:length]...), readerChannel)
<<<<<<< Updated upstream
			tmpBuffer = proto.UnpackLemon3(tmpBuffer, readerChannel)
			if len(tmpBuffer) > 0 {
				//tmpBuffer = proto.Unpack(append(tmpBuffer, ibuf[:length]...), readerChannel)
				//fmt.Println("length=", length)
				//fmt.Println("tmpBuffer=", len(tmpBuffer))
				tmpBuffer = proto.UnpackLemon3(tmpBuffer, readerChannel)
=======
			tmpBuffer = proto.UnpackLemon3(append(tmpBuffer, ibuf[:length]...), readerChannel)
			if len(tmpBuffer) > 0 {
				//tmpBuffer = proto.Unpack(append(tmpBuffer, ibuf[:length]...), readerChannel)
				tmpBuffer = proto.UnpackLemon3(append(tmpBuffer, ibuf[:length]...), readerChannel)
>>>>>>> Stashed changes
			} else {
				//fmt.Println("Stop UnPackLemon3")
				break
			}
		}

		//fmt.Println("Ping!")
	}
}


func handleMsg(msg []byte) (string , uint32) {
	var dat map[string]interface{}
	//var dat proto.RespParam
	if err := json.Unmarshal(msg, &dat); err == nil {
		//fmt.Println("==============json str 转map=======================")
		//fmt.Println(dat)
		//fmt.Println("cmd=", dat["cmd"])
		//fmt.Println("Name=", dat["Name"])
		//fmt.Println("Message=", dat["Message"])

		if dat["cmd"] == "verification" {
			vdata := proto.VerificationData{
				TerminalMac:dat["terminalMac"].(string),
				Valid:1441,
				AuthType: 0,
				AuthId :"13538273761",
				UpRate : 100,
				DownRate :101,
				TcpLimit :222,
				UdpLimit :223,
			}
			verify := proto.Verification{
				Cmd: dat["cmd"].(string),
				SeqId:dat["seqId"].(string),
				Code :"000",
				Data : vdata,
			}
			result, err := json.Marshal(verify)
			if (err == nil){
				fmt.Println("verification==", string(result))
				return string(result),proto.CmdKV[dat["cmd"].(string)]
			}
		}

		if dat["cmd"].(string) == "rcl" {
			fmt.Println("Rcl request")
			//MTK
			return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.8.200\",\"leaseTime\":120,\"startIp\":\"192.168.8.100\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.8.1\",\"mask\":\"255.255.255.0\"},\"md5\":\"79a8836d586f60d998662169b4ff0f48\",\"mode\":1,\"name\":\"-MagicWiFi-58D1-MTK\",\"qos\":{\"downRate\":1024,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":10240,\"port\":0,\"up\":2048}]},\"rfs\":[{\"frequency\":0,\"mode\":0,\"power\":100,\"type\":\"2.4\"}],\"ssids\":[{\"name\":\"-MagicWiFi-_58D0\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/captive/index.html\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"},{\"endIp\":\"  192.168.70.43\",\"startIp\":\"192.168.70.43  \"},{\"endIp\":\"183.238.95.110\",\"startIp\":\"183.238.95.101\"}],\"autoPortalStop\":{\"iOSEnable\":1,\"androidEnable\":1,\"ios\":{\"userAgent\":[\"captivenetworksupport\"]},\"android\":{\"uri\":[\"/generate_204\"],\"host\":[\"vivo.com.cn\"]}},\"auditEnable\":1},\"seqId\":\"87e41fcbfd95430e8f05f0f8ebf53273\"}"), proto.CmdKV[dat["cmd"].(string)]

			//x86
			//return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.60.127\",\"leaseTime\":120,\"startIp\":\"192.168.48.200\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.48.1\",\"mask\":\"255.255.240.0\"},\"md5\":\"70a8836d586f60d998562169b4ff0f49\",\"mode\":1,\"name\":\"Richard-X86\",\"qos\":{\"downRate\":512,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":0,\"port\":0,\"up\":0},{\"down\":0,\"port\":1,\"up\":0},{\"down\":0,\"port\":2,\"up\":0},{\"down\":0,\"port\":3,\"up\":0},{\"down\":0,\"port\":4,\"up\":0}]},\"ssids\":[{\"name\":\"-MagicWiFi-invalid\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/captive/index.html\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"}],\"autoPortalStop\":{\"iOSEnable\":1,\"androidEnable\":1,\"ios\":{\"userAgent\":[\"captivenetworksupport\",\"helloworld\",\"applexxxx\"],\"uri\":[\"/genrate_204\",\"/qbprobenet.txt\",\"/checknetwork\"],\"host\":[\"test.com\"]},\"android\":{\"uri\":[\"/generate_204\",\"/genrate_2004\",\"/qbprobenet.txt\"],\"host\":[\"vivo.com.cn\",\"clients3.google.com\",\"holyshit.com\"],\"userAgent\":[\"androidxxxx\",\"fuckyou\"]}}}}"), proto.CmdKV[dat["cmd"].(string)]
			//return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.60.127\",\"leaseTime\":120,\"startIp\":\"192.168.48.200\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.48.1\",\"mask\":\"d255.255.240.0\"},\"md5\":\"70a8836d586f60d998562169b4ff0f48\",\"mode\":1,\"name\":\"Richard-X86\",\"qos\":{\"downRate\":512,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":0,\"port\":0,\"up\":0},{\"down\":0,\"port\":1,\"up\":0},{\"down\":0,\"port\":2,\"up\":0},{\"down\":0,\"port\":3,\"up\":0},{\"down\":0,\"port\":4,\"up\":0}]},\"ssids\":[{\"name\":\"-MagicWiFi-invalid\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/captive/index.html\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"}],\"auditEnable\":1,\"autoPortalStop\":{\"iOSEnable\":0,\"androidEnable\":0,\"ios\":{\"userAgent\":[\"captivenetworksupport\"]},\"android\":{\"uri\":[\"/generate_204\",\"/qbprobenet.txt\"],\"host\":[\"vivo.com.cn\",\"clients3.google.com\"]}}}}"), proto.CmdKV[dat["cmd"].(string)]

			//AR9344
			//return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.8.200\",\"leaseTime\":120,\"startIp\":\"192.168.8.100\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.8.1\",\"mask\":\"255.255.255.0\"},\"md5\":\"70a8836d586f60d998562169b4ff0f49\",\"mode\":1,\"name\":\"-MagicWiFi-518\",\"qos\":{\"downRate\":1024,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":10240,\"port\":0,\"up\":2048}]},\"rfs\":[{\"frequency\":10,\"mode\":0,\"power\":30,\"type\":\"2.4\"}],\"ssids\":[{\"name\":\"-MagicWiFi-518\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/portal/\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"},{\"endIp\":\"  192.168.70.43\",\"startIp\":\"192.168.70.43  \"},{\"endIp\":\"183.238.95.110\",\"startIp\":\"183.238.95.101\"}],\"autoPortalStop\":{\"iOSEnable\":1,\"androidEnable\":1,\"ios\":{\"userAgent\":[\"captivenetworksupport\",\"helloworld\",\"applexxxx\"],\"uri\":[\"/genrate_204\",\"/qbprobenet.txt\",\"/checknetwork\"],\"host\":[\"test.com\"]},\"android\":{\"uri\":[\"/generate_204\",\"/genrate_2004\",\"/qbprobenet.txt\"],\"host\":[\"vivo.com.cn\",\"clients3.google.com\",\"holyshit.com\"],\"userAgent\":[\"androidxxxx\",\"fuckyou\"]}}},\"seqId\":\"306ff4d0c3a94923a646df3d53d7dbac\"}"),proto.CmdKV[dat["cmd"].(string)]
			//return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.8.200\",\"leaseTime\":120,\"startIp\":\"192.168.8.100\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.8.1\",\"mask\":\"255.255.255.0\"},\"md5\":\"70a8836d586f60d998562169b4ff0f49\",\"mode\":1,\"name\":\"-MagicWiFi-518\",\"qos\":{\"downRate\":1024,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":10240,\"port\":0,\"up\":2048}]},\"rfs\":[{\"frequency\":10,\"mode\":0,\"power\":30,\"type\":\"2.4\"}],\"ssids\":[{\"name\":\"-MagicWiFi-518\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/portal/\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"},{\"endIp\":\"  192.168.70.43\",\"startIp\":\"192.168.70.43  \"},{\"endIp\":\"183.238.95.110\",\"startIp\":\"183.238.95.101\"}],\"autoPortalStop\":{\"iOSEnable\":0,\"androidEnable\":0,\"ios\":{\"userAgent\":[\"captivenetworksupport\"]},\"android\":{\"uri\":[\"/generate_204\",\"/qbprobe/netprobe.txt\"],\"host\":[\"vivo.com.cn\",\"clients3.google.com\"]}}},\"seqId\":\"306ff4d0c3a94923a646df3d53d7dbac\"}"),proto.CmdKV[dat["cmd"].(string)]

			//AR9341
			//return string("{\"cmd\":\"rcl\",\"code\":\"000\",\"data\":{\"dhcp\":{\"endIp\":\"192.168.8.200\",\"leaseTime\":120,\"startIp\":\"192.168.8.100\"},\"httpProxy\":{\"enable\":false,\"regex\":\"\",\"replacement\":\"\"},\"lan\":{\"ip\":\"192.168.8.1\",\"mask\":\"255.255.255.0\"},\"md5\":\"442d695ae1dce3a1f7e2974f9b110ff0\",\"mode\":1,\"name\":\"QA-TEST-AP-9341\",\"qos\":{\"downRate\":1024,\"tcpLimit\":200,\"udpLimit\":60,\"upRate\":128,\"wans\":[{\"down\":10240,\"port\":0,\"up\":2048}]},\"rfs\":[{\"frequency\":11,\"mode\":0,\"power\":20,\"type\":\"2.4\"}],\"ssids\":[{\"name\":\"-MagicWiFi-QA-9341\",\"password\":\"\",\"port\":0,\"url\":\"http://magicwifi.com.cn/portal/\"}],\"trustDomains\":[\"alicdn.com\",\"alipaydns.com\",\"hi-wifi.cn\",\"sasdk.cs0309.3g.qq.com\",\"wx.tenpay.com\",\"tbcache.com\",\"api.apkplug.com\",\"alog.umeng.com\",\"alipay.com\",\"pay.heepay.com\",\"wx.tencent.cn\",\"magicwifi.com.cn\",\"sasdk.3g.qq.com\",\"mp.weixin.qq.com\",\"zhongxin.junka.com\",\"rippletek.com\",\"alipayobjects.com\"],\"trustIps\":[{\"endIp\":\"183.232.2.4\",\"startIp\":\"183.232.2.4\"},{\"endIp\":\"121.201.55.228\",\"startIp\":\"121.201.55.153\"},{\"endIp\":\"123.206.2.126\",\"startIp\":\"123.206.2.126\"},{\"endIp\":\"123.206.2.188\",\"startIp\":\"123.206.2.188\"}],\"autoPortalStop\":{\"iOSEnable\":0,\"androidEnable\":0,\"ios\":{\"userAgent\":[\"captivenetworksupport\",\"helloworld\",\"applexxxx\"],\"uri\":[\"/genrate_204\",\"/qbprobenet.txt\",\"/checknetwork\"],\"host\":[\"test.com\"]},\"android\":{\"uri\":[\"/generate_204\",\"/genrate_2004\",\"/qbprobenet.txt\"],\"host\":[\"vivo.com.cn\",\"clients3.google.com\",\"holyshit.com\"],\"userAgent\":[\"androidxxxx\",\"fuckyou\"]}}},\"seqId\":\"69a629b9e1ae4a1a933e0d4e851040d5\"}"), proto.CmdKV[dat["cmd"].(string)]
		}

		if dat["cmd"].(string) != "" {
			if dat["cmd"].(string) == "web_read_resp" ||
				dat["cmd"].(string) == "resource_read_resp"||
				dat["cmd"].(string) == "web_write_resp" ||
				dat["cmd"].(string) == "resource_write_resp" {
				//return "",0
			}
			login := proto.RespParam {
				Cmd: dat["cmd"].(string),
				SeqId:dat["seqId"].(string),
				Code :"000",
				Data: []string{"test"},
			}
			result , err := json.Marshal(login)
			if (err == nil){
				fmt.Println("[ToClient]=", string(result))
				fmt.Println("=============================================")
				return string(result), proto.CmdKV[dat["cmd"].(string)]
			}
			//fmt.Println("result=", err.Error())
		}



	}
	return "{\"CMD\":\"OK\"}", 0
}

<<<<<<< Updated upstream
func talktoclients(to net.Conn, msg string, cmdId uint32) {
	//data := "Hello Client"
	wrote, err := to.Write(proto.PacketLemon3([]byte(msg), cmdId))
=======
var count int

func talktoclients(to net.Conn, msg string) {
	//data := "Hello Client"
	//wrote, err := to.Write(proto.Packet([]byte(msg)))
	fmt.Println("Got here", count)
	count = count + 1
	time.Sleep(time.Second * 2)
	wrote, err := to.Write(proto.PacketLemon3([]byte(msg), 0x03))
>>>>>>> Stashed changes
	checkError(err, "Write: wrote "+string(wrote)+" bytes.")
	if (cmdId == proto.CmdKV["heartbeat"]){
		//words := "{\"cmd\":\"login\",\"seqId\":\"1234321\",\"Message\":\"message\"}"
		//to.Write(proto.PacketLemon3([]byte(words), ))
/*
		webReadReq := proto.ReqParam{
			Cmd :"web_read_req",
			SeqId: "1234567890123456789012",
			Mac: "1234567890ad",
		}
		result, err := json.Marshal(webReadReq)
		if err == nil{
			fmt.Println("Send web_read_req")
			to.Write(proto.PacketLemon3(result, proto.CmdKV["web_read_req"]))
		}else{
			fmt.Println(err.Error())
		}

		webWriteReq := proto.WebWrite{
			Cmd:"web_write_req",
			SeqId:"1234567890123456789012",
			Mac:"123456789012",
			Ver: 190029,
			Url:"http://r.magicwifi.com.cn/v3/portal/tar/50.tar",
			Md5:"e63dca553bd36c82085a8a4811c9b76f",
		}
		result, err = json.Marshal(webWriteReq)
		if err == nil{
			to.Write(proto.PacketLemon3(result, proto.CmdKV["web_write_req"]))
		}
		//Resource READ/WRITE
		resouceReadReq := proto.ReqParam{
			Cmd :"resource_read_req",
			SeqId: "1234567890123456789012",
			Mac: "1234567890ad",
		}
		result, err = json.Marshal(resouceReadReq)
		if err == nil{
			to.Write(proto.PacketLemon3(result, proto.CmdKV["resource_read_req"]))
		}

		resourceWriteReq := proto.WebWrite{
			Cmd:"resource_write_req",
			SeqId:"1234567890123456789012",
			Mac:"123456789012",
			Ver: 190087,
			Url:"http://r.magicwifi.com.cn/video_sync/files_309.txt",
			Md5:"155e0ac4800beef0064806b8c2b600cc",
		}
		result, err = json.Marshal(resourceWriteReq)
		if err == nil{
			to.Write(proto.PacketLemon3(result, proto.CmdKV["resource_write_req"]))
		}

		//bogus := proto.DnsBogus{
		//	Domain:"www.test.com",
		//	Host:"127.0.0.1",
		//}
		//bogus1 := proto.DnsBogus{
		//	Domain:"www.shit.com",
		//	Host:"127.0.0.1",
		//}
		dnsBogusWriteReq := proto.DnsBogusWrite{
			Cmd:"dns_bogus_write_req",
			SeqId:"1234567890123456789012",
			Mac:"123456789012",
			//Bogus: []proto.DnsBogus{bogus, bogus1},
			Bogus: []proto.DnsBogus{},
		}
		result, err = json.Marshal(dnsBogusWriteReq)
		if err == nil{
			to.Write(proto.PacketLemon3(result, proto.CmdKV["dns_bogus_write_req"]))
		}

		configRead := proto.ReqParam{
			Cmd :"config_read_req",
			SeqId: "1234567890123456789012",
			Mac: "1234567890ad",
		}
		result, err = json.Marshal(configRead)
		if err == nil{
			to.Write(proto.PacketLemon3(result, proto.CmdKV["config_read_req"]))
		}
		*/
	}
}

func reader(readerChannel chan []byte, conn net.Conn) {
	for {
		select {
		case data := <-readerChannel:
			//fmt.Println("Message: ", string(data))
			msg, cmdId := handleMsg(data)
			if cmdId == 0 {
				continue
			}
			talktoclients(conn, msg, cmdId)
		}
	}
}

func checkError(error error, info string) {
	if error != nil {
		panic("ERROR: " + info + " " + error.Error())
	}
}
