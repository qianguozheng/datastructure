package main

import (
	"./stack"
	"fmt"
)

func main()  {
	var s stack.Stack
	s.Push(23)
	s.Push(77)
	s.Push(35)

	fmt.Printf("My Stack is %v\n", s)

	val := s.Pop()
	fmt.Println(val)
	val = s.Pop()
	fmt.Println(val)
	val = s.Pop()
	fmt.Println(val)

	val = s.Pop()
	fmt.Println(val)
}
