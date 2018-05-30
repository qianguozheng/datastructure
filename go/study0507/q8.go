package main

import "fmt"

//FILO
type Stack struct {
	Len int
	Cap int
	Array [10]int
}

func (stk *Stack) push(value int)  {
	index := stk.Len

	if stk.Len > 9 {
		println("Capacity is 10, no more space")
		return
	}

	stk.Array[index] = value
	stk.Len = index +1
}

func (stk *Stack) pop() (value int) {
	if stk.Len < 1 {
		println("No value in stack")

	}
	value = stk.Array[stk.Len-1]
	stk.Len = stk.Len - 1
	return value
}

func (stk *Stack) len() (length int) {
	length = stk.Len
	return length
}

func (stk *Stack) String() string {
	//var s []string
	//s = make([]string, stk.Len)
	//for i:=0; i<stk.Len; i++{
	//	s = append(s, fmt.Sprintf("[%d:%d]", i, stk.Array[i]))
	//}
	//fmt.Println(s)
	var s string
	for i:=0;i<stk.Len;i++ {
		s += fmt.Sprintf("[%d:%d]", i, stk.Array[i])
	}
	return s
}


func main()  {
	var s Stack
	//s := new(Stack)
	println("len=", s.Len)
	s.push(3)
	println("len=", s.Len)
	s.push(4)
	println("len=", s.Len)
	s.push(1)

	fmt.Printf("My stack %v\n", &s)
	println("len=", s.Len)
	println("pop=", s.pop())
	println("len=", s.Len)
	println("pop=", s.pop())
	println("len=", s.Len)
	println("pop=", s.pop())
}