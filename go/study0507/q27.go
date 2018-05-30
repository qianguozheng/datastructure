package main

import "fmt"

func fib(value int, ch chan int)  {
	f := make([]int, value)
	if value == 1 {
		f[0] = 1
		ch <- 1
	} else if value == 2 {
		f[0] = 1
		f[1] = 1;
		ch <- 1
		ch <- 1
	} else {
		f[0], f[1] = 1, 1
		ch <- 1
		ch <- 1
		for i:=2; i< value; i++{
			f[i] = f[i-1] + f[i-2]
			ch <- f[i]
		}
	}
}

func showert(ch chan int, quit chan bool)  {
	for {
		select{
		case j:= <- ch :
			fmt.Printf("%d\t", j)
		case _ = <-quit:
			break
		}
	}
}


func dup3(in <-chan int) (<-chan int, <-chan int, <-chan int) {
	a,b,c := make(chan int, 2), make(chan int, 2), make(chan int, 2)

	go func() {
		for {
			x := <-in
			fmt.Println("dup3 x=", x)
			a <- x
			b <- x
			c <- x //output result
		}
	}()

	return a, b, c
}

func  fibb() <-chan int {
	x := make(chan int, 2)
	a, b, out := dup3(x)
	go func() {
		x <- 0
		x <- 1
		<- a
		for {
			fmt.Println("xxxxx")
			x <- <-a + <-b
		}
	}()
	return out
}
func main()  {

	//ch := make (chan int)
	//quit := make(chan bool)
	//
	//go showert(ch, quit)
	//fib(15, ch)
	//quit <- true

	x := fibb()

	for i:=0; i<10; i++ {
		fmt.Println(<-x)
	}
}
