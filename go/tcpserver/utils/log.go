package utils

import (
	"path"
	"runtime"
	"strings"
)

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
