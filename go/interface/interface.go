package main

import "fmt"

type Speaker interface {
	Say(string)
	Listen(string) string
	Interrupt(string)
}

type WangLan struct {
	msg string
}

func (this *WangLan) Say(msg string) {
	fmt.Printf("王兰说：%s\n", msg)
}

func (this *WangLan) Listen(msg string) string {
	this.msg = msg
	return msg
}

func (this *WangLan) Interrupt(msg string) {
	this.Say(msg)
}

type JiangLou struct {
	msg string
}

func (this *JiangLou) Say(msg string) {
	fmt.Printf("Jianglou say：%s\n", msg)
}

func (this *JiangLou) Listen(msg string) string {
	this.msg = msg
	return msg
}

func (this *JiangLou) Interrupt(msg string) {
	this.Say(msg)
}

type Html []interface{}

func test() {
	html := make(Html, 5)
	html[0] = "div"
	html[1] = "span"
	html[2] = []byte("script")
	html[3] = "style"
	html[4] = "head"

	for index, element := range html {
		if value, ok := element.(string); ok {
			fmt.Printf("Html[%d] is a string and its value is %s\n", index, value)

		} else if value, ok := element.([]byte); ok {
			fmt.Printf("html[%d] is a []byte and its value is %s\n", index, string(value))
		}
	}

	for index, element := range html {
		switch value := element.(type) {
		case int:
			fmt.Printf("%d - type error", index)
		case string:
			fmt.Printf("%d - %s", index, value)
		case []byte:
			fmt.Printf("%d - %s", index, string(value))
		default:
			fmt.Printf("Unknown tye")
		}
	}
}

type I interface {
	Get() int
	Put(int)
}
type S struct {
	i int
}

func (p *S) Get() int {
	return p.i
}

func (p *S) Put(t int) {
	p.i = t
}
func (p *S) Reset() {
	p.i = 0
}

func f(p I) {
	fmt.Println(p.Get())
	p.Put(1)
	fmt.Println(p.Get())

}
func main() {
	var s S

	f(&s)

	/*wl := &WangLan{}
	jl := &JiangLou{}

	var person Speaker
	person = wl
	person.Say("Fuck YOu")
	person = jl
	person.Say("Fucccck you too")
	person.Interrupt("Stop, ass")

	test()
	*/
}
