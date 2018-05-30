package main

func variableparam(value ... int)  {
	println("Length=",len(value))
	for i:=0;i<len(value);i++{
		println(value[i])
	}
}

func main()  {
	variableparam(1,3,6)
	variableparam(9,4,34,6,7,8,8,89)
}