package main

import "fmt"

/////////////TODO: Need to implement print the whole fib
var fib []int
func fibonaci(value int) int {
	if value == 1 {
		//fib = append(fib, 1)
		return 1
	} else if value ==2 {
		//fib = append(fib, 1)
		return 1
	} else	{
		t := fibonaci(value-1) + fibonaci(value-2)
		//fib = append(fib, t)
		return t
	}
}

//
//func fibonaci_arr(value int) (result []int) {
//	result = make([]int, 10)
//	for i:=0; i< value; i++ {
//		println(fibonaci(i))
//		//result = append(result, fibonaci(i))
//	}
//	return result
//}


func fibnacci(value int) []int {
	x := make([]int, value)
	x[0], x[1] = 1 , 1

	for n:=2;n<value;n++{
		x[n] = x[n-1] + x[n-2]
	}
	return x
}

func main()  {
	//println(fibonaci(3))
	//fmt.Println(fib)
	//println(fibonaci(8))
	//
	////println(fibonaci_arr(5))
	//fmt.Println(fib)

	fmt.Println(fibnacci(10))
}