package main

import "fmt"

////TODO： make it work at string list
func test( f func(int) int, t []int) []int {
	x := make([]int, 0)
	for k:=0;k<len(t);k++ {
		x = append(x, f(t[k]))
	}

	return x
}

func sq(x int) int {
	return x*x
}

func teststr( f func(string) string, t []string) []string {
	x := make([]string, 0)
	for k:=0;k<len(t);k++ {
		x = append(x, f(t[k]))
	}

	return x
}

func sqstr(x string) string{
	return x+"sq"
}


func main()  {
	x:= []int{2,3,4,5}
	ret := test(sq, x)

	fmt.Println(ret)

	xx:= []string{"aaa","bbb","ccc","ddd"}
	retstr := teststr(sqstr, xx)
	fmt.Println(retstr)
}
