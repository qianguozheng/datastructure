package main

import "fmt"

/*内存中的存储方式分为两种：大端序和小端序。
大端序的存储方式是 高位字节存储在低地址上 - Big
小端序的存储方式是 高位字节存储在高地址上 - Little
*/
func IsBigEndian() bool {
	var i int32 = 0x12345678
	var b byte = byte(i)
	if b == 0x12 {
		return true
	}
	return false
}

//截断的规则是：保留低地址上的数据，丢弃多余的高地址上的数据
func IsLittleEndian() bool {
	var i int32 = 0x12345678
	var b byte = byte(i)
	if b == 0x78 {
		return true
	}
	return false
}
func main() {
	fmt.Println(IsBigEndian())
	fmt.Println(IsLittleEndian())
}
