package main

import (
	"flag"
	"fmt"
)

func main() {

	DataPath := flag.String("D", "/home", "Data path")
	LogFile := flag.String("l", "/home/log.file", "Logfile name")
	NoWait := flag.Bool("w", false, "Do not wait until operation completes")

	flag.Parse()

	cmd := flag.Arg(0)

	fmt.Printf("Action: %s\n", cmd)
	fmt.Printf("DataPaht: %s\n", *DataPath)
	fmt.Printf("FileName: %s\n", *LogFile)
	fmt.Printf("NoWait: %v\n", *NoWait)

}
