package main

import "fmt"
import "bytes"

type S struct {
	Str string
	Int int
}

func test() {
	d := S{"Hello World", 26}
	if (d == S{"Hello World", 26}) {
		fmt.Println(true)
	} else {
		fmt.Println(false)
	}
}

func slicetest() {
	s := []byte("golang")
	subslice1 := []byte("go")
	subslice2 := []byte("Go")
	fmt.Println(bytes.Contains(s, subslice1))
	fmt.Println(bytes.Contains(s, subslice2))
}

func slicecount() {
	s := []byte("banana")
	sep1 := []byte("ban")
	sep2 := []byte("na")
	sep3 := []byte("a")

	fmt.Println(bytes.Count(s, sep1))
	fmt.Println(bytes.Count(s, sep2))
	fmt.Println(bytes.Count(s, sep3))

}

func maptest() {
	s := []byte("各位上午好")
	m := func(r rune) rune {
		if r == '上' {
			r = '下'
		}
		return r
	}

	fmt.Println(string(s))
	fmt.Println(string(bytes.Map(m, s)))
}
func main() {

	slicetest()
	slicecount()
	maptest()
}
