package main

import "fmt"


func main()  {
	ch := make(chan int)
	done := make(chan int)

	go shower(ch, done)

	for i:=0; i<10; i++{
		ch <-i
	}
	done <- 1
}

func shower(c chan int, d chan int)  {
	for {
		select {
		case j := <- c:

			fmt.Printf("%d\n", j)
		case _= <-d:
			//fmt.Printf("----%d\n", t)
			//if (t == 1){
			//	close(d)
			//	close(c)
			//}

			break
		}

	}
}