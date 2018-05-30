package main

import "fmt"

type Xi []int
type Xs []string

type Sorter interface{
	Len() int
	Less(i int, j int) bool
	Swap(i, j int)
}

func (x Xi) Len() int {
	return len(x)
}

func (x Xi) Swap(i, j int) {
	x[i], x[j] = x[j], x[i]
}

func (x Xi) Less(i, j int) bool {
	return x[j] < x[i]
}

func (x Xs) Len() int {
	return len(x)
}

func (x Xs) Swap(i, j int) {
	x[i], x[j] = x[j], x[i]
}

func (x Xs) Less(i, j int) bool {
	return x[j] < x[i]
}


func Sort(x Sorter) {
	for i:=0; i< x.Len()-1;i++{
		for j:=i+1; j< x.Len(); j++{
			if x.Less(i, j) {
				x.Swap(i, j)
			}
		}
	}
}

func main()  {
	ints := Xi{44,67,3,117,56,23,89}
	strings := Xs {"nut", "ape", "elephant", "zoo", "go"}

	Sort(ints)
	fmt.Printf("%v\n", ints)
	Sort(strings)
	fmt.Printf("%v\n", strings)


}