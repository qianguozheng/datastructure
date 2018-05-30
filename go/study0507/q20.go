package main

import (
	"container/list"
	"fmt"
)

func main()  {
	l := list.New()
	l.Init()
	l.PushBack(1)
	l.PushBack(4)
	l.PushBack(0)
	l.PushBack(9)

	for e:=l.Front(); e != nil ; e = e.Next(){
		fmt.Println(e.Value)

	}
}