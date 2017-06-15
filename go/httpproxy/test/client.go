package client

type ClientRequest struct{
    url string
    method string
    version string

    domain map[string]string //Key: value --> Domain:IP
    content []byte
}

