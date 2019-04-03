package main

import (
	"bytes"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"

	"github.com/asdine/storm"
	jsoniter "github.com/json-iterator/go"
)

var (
	reports        *storm.DB
	channelWebhook string
)

type crashReport struct {
	Name     string `json:"name"`
	Dump     string `json:"dump"`
	HostName string `json:"host"`
}

type webhookRequest struct {
	URL      string `json:"url"`
	Content  string `json:"content"`
	Username string `json:"username"`
	Avatar   string `json:"avatar_url"`
}

type storedReport struct {
	ID   int `storm:"id,increment"`
	Name string
	Host string
	Dump string
}

func handleCrashReport(w http.ResponseWriter, r *http.Request) {
	var data crashReport
	decoder := jsoniter.NewDecoder(r.Body)
	err := decoder.Decode(&data)

	if err != nil {
		log.Printf("Invalid request received from %s!\n", r.RemoteAddr)
		return
	}

	log.Printf("New crash report has been received from %s@%s!\n", data.Name, r.RemoteAddr)

	rep := storedReport{
		Name: data.Name,
		Host: data.HostName,
		Dump: data.Dump,
	}

	reports.Save(&rep)
	sendReportLink(rep)
}

func sendReportLink(data storedReport) {
	respData := webhookRequest{
		URL:      channelWebhook,
		Content:  fmt.Sprintf("\nCrash has been detected for user **%s** playing at **%s**\nCheck http://oakmaster.madaraszd.net:8001/report/%d/ for more information!", data.Name, data.Host, data.ID),
		Username: "Crash Reporter",
		Avatar:   "https://cdn.discordapp.com/attachments/233249310727340032/532897486751268874/MafiaHub.png",
	}

	js, _ := jsoniter.Marshal(respData)
	req, err := http.NewRequest("POST", channelWebhook, bytes.NewBuffer(js))
	if err != nil {
		log.Printf("Could not create web request: %v\n", err)
		return
	}
	req.Header.Set("Content-Type", "application/json")
	cli := &http.Client{}
	resp, rerr := cli.Do(req)

	if rerr != nil {
		log.Printf("Could not reach Discord servers:\n%v", rerr)
		return
	}
	resp.Body.Close()
}

func showReport(w http.ResponseWriter, r *http.Request) {
	id := string([]byte(strings.TrimPrefix(r.URL.Path, "/report/"))[:1])
	log.Printf("Fetching report %s ...\n", id)

	var rep storedReport
	idconv, _ := strconv.Atoi(id)
	err := reports.One("ID", idconv, &rep)
	if err != nil {
		log.Printf("Could not open report '%s': %v\n", id, err)
		w.WriteHeader(404)
		w.Write([]byte("Report not found!"))
		return
	}

	w.WriteHeader(200)
	w.Write([]byte(rep.Dump))
}

func main() {
	var err error
	reports, err = storm.Open("reports.db")

	if err != nil {
		log.Fatalf("Could not open database.\n")
		return
	}

	defer reports.Close()

	channelWebhook = os.Args[1]
	http.HandleFunc("/push", handleCrashReport)
	http.HandleFunc("/report/", showReport)

	fmt.Printf("Starting crash report server...\n")
	if err := http.ListenAndServe(":8001", nil); err != nil {
		log.Fatal(err)
	}
}
