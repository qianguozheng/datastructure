package server

import "strings"
import "./client"

type ServerResponse struct{
    url string
    code int
    version string //HTTP/1.1 or HTTP/1.0 or HTTP/2
    content_length int
    content_encoding string
    content []byte
}

func parseRequest(req []byte) client.clientRequest {
    var cliReq client.ClientRequest

    //TODO: support binary request data 
    data := string(req[:len[req]]) 

    reqArray := strings.Split(data, "\r\n")
    for i, v := range reqArray{
        fmt.Println("Request:", v)
        if v.HasPrefix("Host: "){
            f := strings.Fields(v)
            cliReq.domain = f[1] 
        }
        if i == 0 {
            f := strings.Fields(v)
            cliReq.method = f[0]
            cliReq.url = f[1]
            cliReq.version = f[2]
        }
    }

    //Get http request content
    conArr := strings.Split(data, "\r\n\r\n")
    if len(conArr[1]) > 0 {
        cliReq.content = conArr[1]
    }

    return cliReq
}

func Server(addr string, port int){
    
}

