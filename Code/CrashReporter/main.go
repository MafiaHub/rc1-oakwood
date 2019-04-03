package main

import (
	"bytes"
	"log"
	"net/http"
	"os"
	"strings"

	jsoniter "github.com/json-iterator/go"
)

type crashRequest struct {
	Name string `json:"name"`
	Dump string `json:"dump"`
	Host string `json:"host"`
}

func main() {
	log.Println("Oops! You've crashed! Sending crash report...")

	if len(os.Args) < 2 {
		log.Fatalln("Nice try :)")
		return
	}

	data := strings.Split(os.Args[1], "|")

	if len(data) != 3 {
		log.Fatalln("Well your crash report is corrupted, bye!")
		return
	}

	name := data[0]

	if name == "" {
		name = "Unknown"
	}

	host := data[1]

	if host == "" {
		host = "Not connected"
	}

	respData := crashRequest{
		Name: name,
		Host: host,
		Dump: data[2],
	}

	js, _ := jsoniter.Marshal(respData)
	req, _ := http.NewRequest("POST", "http://report.oakwood-mp.net/push", bytes.NewBuffer(js))
	req.Header.Set("Content-Type", "application/json")
	cli := &http.Client{}
	resp, rerr := cli.Do(req)

	if rerr != nil {
		log.Printf("Could not reach the crash report server:\n%v", rerr)
		return
	}
	resp.Body.Close()
}
