package main

func average(in []float64) (result float64) {
	var sum float64
	for i:=0;i<len(in);i++{
		sum = sum + in[i]
	}
	if (len(in) > 0) {
		result = sum / float64(len(in))
	} else {
		result = 0
	}

	return result
}
func main()  {
	f := []float64{2,3,4,5,6,7,8}
	println(average(f))
}