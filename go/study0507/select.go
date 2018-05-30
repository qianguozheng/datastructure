package main

import "fmt"

func avg(val []float64) (ret float64) {

	ret = 0.0
	for k:=0; k < len(val); k++ {
		ret = ret + val[k]
	}
	ret = ret / float64(len(val))
	return ret
}

func main()  {
	//var f [] float64
	//f =append(f, 1)
	//f =append(f, 2)
	//f =append(f, 3)
	//f =append(f, 4)
	//f =append(f, 5)
	//println(avg(f))

	//f := []float64{1,2,3,4,5}
	//println(avg(f))

	//var ss []string
	//fmt.Printf("length:%v \t addr:%p\t isnil %v\n", len(ss),
	//	ss, ss== nil)
	//
	//print("func print", ss)
	//for i:=0;i<10;i++{
	//	ss = append(ss, fmt.Sprintf("s%d",i))
	//}
	//fmt.Printf("[ local print ]\t:\tlength:%v\taddr:%p\tisnil:%v\n",
	//	len(ss), ss, ss==nil)
	//
	//print("after append", ss)
	//
	//index := 5
	//ss = append(ss[:index], ss[index+1:]...)
	//print("after delete", ss)
	//
	//rear := append([]string{}, ss[index:]...)
	//ss = append(ss[0:index], "inserted")
	//ss = append(ss, rear...)
	//print("after insert", ss)

	//copy
	//var sa = make([]string, 0)
	//for i:=0;i<10;i++{
	//	sa = append(sa, fmt.Sprintf("%v", i))
	//}
	//var da = make([]string, 0, 10)
	//var cc =0
	//cc = copy(da, sa)
	//
	//fmt.Printf("copy to da(len=%d)\tcopied=%d\t%v\n", len(da), cc, da)
	//da = make([]string, 5)
	//cc = copy(da, sa)
	//fmt.Printf("copy to da(len=%d)\tcopied=%d\t%v\n", len(da), cc, da)
	//da = make([]string, 10)
	//cc = copy(da, sa)
	//fmt.Printf("copy to da(len=%d)\tcopied=%d\t%v\n", len(da), cc, da)
	//
	//slice1  := da[0:4]
	//slice1[0] = "100"
	//fmt.Printf("slice1=%v", slice1)
	//fmt.Printf("da=%v", da)

	data := make([]int, 10, 20)
	data[0] = 1
	data[1] = 2

	dataappend := make([]int, 12, 30)
	dataappend[0] = 1
	dataappend[1] = 2

	result := append(data, dataappend...)
	fmt.Println(len(result))
	result[0] = 99
	result[11] = 98

	//dataappend[33] = 96

	fmt.Println("length:", len(data), "cap:", cap(data), ":", data)
	fmt.Println("result length:", len(result), "cap:", cap(result), ":", result)
	fmt.Println("length:", len(dataappend), "cap:", cap(dataappend), ":", dataappend)
}

func print(msg string, ss []string)  {
	fmt.Printf("[%20s]\t:length:%v\taddr:%p\tisnil:%v\tcontent:%v\n",
		msg, len(ss), ss, ss== nil, ss)
	fmt.Println()
}
