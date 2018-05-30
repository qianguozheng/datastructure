package main

import "fmt"

func bubbleSort(in []int) (out []int) {
	swapped := true
	for (swapped) {
		swapped = false
		for i:=1;i<len(in)-1;i++{
			if in[i-1] > in[i] {
				tmp := in[i]
				in[i] = in[i-1]
				in[i-1] = tmp
				swapped = true
			}
		}
	}

	out = make([]int, len(in))
	copy(out, in)
	return out
}
func main()  {
	x := []int {3,1,6,3,4,9}
	t := bubbleSort(x)
	fmt.Println(t)
}
