package main

import (
	"fmt"
	"reflect"
)

type Maxer interface {
	Length() int
	Bigger(i, j int) bool
	Swap1(i, j int)
	Get (i int) interface{}
}

type Xi []int
type Xf []float32

func (x Xi) Length() int {
	return len(x)
}

func (x Xi) Bigger(i, j int) bool {
	return x[i] > x[j]
}

func (x Xi) Swap1(i, j int) {
	x[i] ,x[j] = x[j], x[i]
}

func (x Xi) Get(i int) interface {} {
	return x[i]
}


func (x Xf) Length() int {
	return len(x)
}

func (x Xf) Bigger(i, j int) bool {
	return x[i] > x[j]
}

func (x Xf) Swap1(i, j int) {
	x[i] ,x[j] = x[j], x[i]
}

func (x Xf) Get(i int) interface {} {
	return x[i]
}

func MaxAll(m Maxer) interface{} {
	max := m.Get(0)
	for i:=0; i<= m.Length()-2; i++ {

		//Reflect
		switch m.Get(i).(type) {
		case int:
			if reflect.ValueOf(m.Get(i)).Int() > reflect.ValueOf(m.Get(i+1)).Int() {
				max = m.Get(i)
			} else {
				max = m.Get(i+1)
			}
		case float32:
			if reflect.ValueOf(m.Get(i)).Float() > reflect.ValueOf(m.Get(i+1)).Float() {
				max = m.Get(i)
			} else {
				max = m.Get(i+1)
			}
		}
	}

	return max
}


func main()  {
	test := Xi{2,3,4,56,87,1}
	tt := Xf{2.0,4.9,1.2,4.5,9.8}

	fmt.Printf("%v\n", MaxAll(test))
	fmt.Printf("%v\n", test)
	fmt.Printf("%v\n", MaxAll(tt))
	fmt.Printf("%v\n", tt)
}