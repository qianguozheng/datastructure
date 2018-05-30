package stack

import "fmt"

type Stack struct {
	i int
	array [10]int
}

func (stack *Stack) Push(value int) {
	if stack.i > 10 {
		println("Stack is full")
		return
	}

	stack.array[stack.i] = value
	stack.i = stack.i + 1
}

func (stack *Stack) Pop() (value int) {
	if stack.i < 1 {
		println("No value in stack")
		value = 0
		return
	}
	stack.i = stack.i - 1
	value = stack.array[stack.i]

	return
}

func (stack Stack) String() string {
	var s string
	for i:=0;i<stack.i; i++{
		s += fmt.Sprintf("[%d:%d] ",i, stack.array[i])
	}
	return s
}

