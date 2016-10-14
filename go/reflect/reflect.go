package main

import (
	"fmt"
	"reflect"
)

type lx interface {
	SayHi()
}

type User struct {
	Name string
	Age  int64
	Sex  string
}

func (u *User) SayHi() {
	fmt.Println("Hello World")
}

func main() {
	user := User{"zhangsan", 25, "male"}
	FillStruct(user)
}

func FillStruct(obj interface{}) {
	t := reflect.TypeOf(obj)
	fmt.Println(t.Name())
	fmt.Println(t.Kind().String())
	fmt.Println(t.PkgPath())
	fmt.Println(t.String())
	fmt.Println(t.Size())
	fmt.Println(t.Align())
	fmt.Println(t.FieldAlign())

	var u User
	fmt.Println(t.AssignableTo(reflect.TypeOf(u)))
	fmt.Println(t.ConvertibleTo(reflect.TypeOf(u)))

	fmt.Println(t.NumField)
	fmt.Println(t.Field(0).Name)
	fmt.Println(t.FieldByName("Age"))
	fmt.Println(t.FieldByIndex([]int{0}))
}
