package main


func increase(a, b int) (A, B int) {
	if a > b {
		A = b
		B = a
	} else {
		A = a ; B=b
	}
	return A, B
}

func main()  {
	A, B := increase(5, 2)
	println(A, B)
	A, B  = increase(37, 39)
	println(A, B)
}