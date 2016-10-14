package main

import (
	"fmt"
	"time"
)

//http://www.cnblogs.com/hustcat/p/4003729.html

func chansync() {
	c := make(chan int) //Allocate a channel
	go func() {
		fmt.Println("Do something")
		for i := 0; i < 5; i++ {
			time.Sleep(time.Second * 1)
			//i = i + 1
			fmt.Println("Hello World")
		}
		c <- 1
	}()

	<-c //Wait for func to finish; discard sent
	fmt.Println("End of main")
}

func Producer(queue chan<- int) {
	for i := 0; i < 10; i++ {
		queue <- i
	}
}

func Consumer(queue <-chan int) {
	for i := 0; i < 10; i++ {
		v := <-queue
		fmt.Println("Receive:", v)
	}
}

func producer_consume_test() {
	queue := make(chan int, 1)
	go Producer(queue)
	go Consumer(queue)
	time.Sleep(1e9)
}

func multichan() {
	c1 := make(chan string)
	c2 := make(chan string)

	go func() {
		time.Sleep(time.Second * 1)
		c1 <- "one"
	}()

	go func() {
		time.Sleep(time.Second * 2)
		c2 <- "two"
	}()

	for i := 0; i < 2; i++ {
		select {
		case msg1 := <-c1:
			fmt.Println("Received", msg1)
		case msg2 := <-c2:
			fmt.Println("Received:", msg2)
		}
	}
}

func simpletest() {
	c1 := make(chan int, 3)
	c1 <- 1
	c1 <- 2
	//c1 <- 3

	//for {
	v, ok := <-c1
	fmt.Println("value: ", v, " ok:", ok)
	if !ok {
	}
	//}
}
func main() {

	//multichan() // Multi Channel test
	simpletest()
}
