package main

import (
	"fmt"
	"net/http"
)

func test() {
	resp, err := http.Get("http://hiweeds.net")
	if err != nil {
		return
	}
	all := make([]byte, 0)
	data := make([]byte, 10240)
	for {
		_, err := resp.Body.Read(data)
		all = append(all, data...)
		if err != nil {
			fmt.Println(err.Error())
			break
		}
		fmt.Println("Read==-------------------------------------------")

	}

	//fmt.Println(string(all[:len(all)]))

}

func main() {
	test()
}
