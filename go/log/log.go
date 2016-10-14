package main

import (
	"fmt"
	"log"
	"os"
	"path"
	"runtime"
	"strings"
)

//http://www.crifan.com/go_language_output_info_to_log_file/

//Get Filename
func GetCurFileName() string {
	_, fullFileName, _, _ := runtime.Caller(0)
	var filenameWithSuffix string
	filenameWithSuffix = path.Base(fullFileName)

	var fileSuffix string
	fileSuffix = path.Ext(filenameWithSuffix)

	var filenameOnly string
	filenameOnly = strings.TrimSuffix(filenameWithSuffix, fileSuffix)

	return filenameOnly
}

func main() {
	var filenameOnly string
	filenameOnly = GetCurFileName()
	fmt.Println("FilenameOnly=", filenameOnly)

	var logFilename string = filenameOnly + ".log"
	fmt.Println("logFilename=", logFilename)
	logFile, err := os.OpenFile(logFilename, os.O_RDWR|os.O_CREATE, 0777)

	if err != nil {
		fmt.Printf("Open file error=%s\n", err.Error())
	}
	defer logFile.Close()

	logger := log.New(logFile, "", log.Ldate|log.Ltime|log.Lshortfile)
	logger.Print("Normal log file")
	logger.Print("Logfile name ")
	logger.Println("Logfile name ")
	logger.Println("Logfile name ")
}
