package main

import "fmt"

func plusTwo() func(int)int {
	p := func(x int) int {
		return x+2
	}
	return p
}

func plusX(x int) func(int)int {
	p := func(y int) int {
		return y+x
	}
	return p
}

func main()  {
	p := plusTwo()
	fmt.Printf("%v\n", p(2))

	px := plusX(7)
	fmt.Printf("%v\n", px(4))
}
