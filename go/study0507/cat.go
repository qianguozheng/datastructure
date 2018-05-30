package main

import (
	"io/ioutil"
	"fmt"
	"os"

	"bufio"
	"io"
)

func check (e error)  {
	if e != nil {
		panic(e)
	}
}
func Cat(filename string)  {
	file, err := ioutil.ReadFile(filename)
	check(err)

	fmt.Print(string(file))
}

func Cat2(filename string) error {
	f , err := os.Open(filename)
	check(err)

	buf := bufio.NewReader(f)
	var i int
	for {
		line, err := buf.ReadString('\n')

		//line = strings.TrimSpace(line)
		i++
		fmt.Printf("%d\t%s", i, line)

		if err != nil {
			if err == io.EOF {
				return nil
			}
			return err
		}
	}
	return nil
}

func main()  {
	//fmt.Println(os.Args[1])

	showLineNumber := false
	for _, v := range os.Args{
		if v == "n" {
			showLineNumber = true
		}
	}
	if showLineNumber {
		Cat2(os.Args[2])
	}
	//name := os.Args[1]
	//Cat(name)
}
