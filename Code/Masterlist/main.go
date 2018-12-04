package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
	"strconv"
	"sync"
	"time"
)

const defaultTimeout int64 = 120

// Response is an HTTP response sent to the client
type Response struct {
	Error   bool        `json:"error"`
	Message string      `json:"message"`
	Servers interface{} `json:"servers"`
}

// Server represents the server metadata
type Server struct {
	Name              string `json:"name"`
	Players           int    `json:"players"`
	MaxPlayers        int    `json:"maxPlayers"`
	PasswordProtected bool   `json:"pass"`
	ServerHost        string `json:"host"`
	ServerPort        string `json:"port"`
	updatedAt         int64
}

var servers map[string]*Server
var mutex *sync.Mutex

func worker() {
	for {
		mutex.Lock()
		currentTime := time.Now().Unix()

		for k, v := range servers {
			if currentTime > v.updatedAt {
				delete(servers, k)
				log.Printf("Removing server entry %s:%s with name '%s'\n", v.ServerHost, v.ServerPort, v.Name)
			}
		}
		mutex.Unlock()

		time.Sleep(time.Second)
	}
}

func main() {
	servers = make(map[string]*Server)
	mutex = &sync.Mutex{}

	http.HandleFunc("/push", AddServer)
	http.HandleFunc("/fetch", GetServers)

	go worker()

	fmt.Printf("started server at 8000\n")

	if err := http.ListenAndServe(":8000", nil); err != nil {
		panic(err)
	}
}

// AddServer adds a server to the list
func AddServer(writer http.ResponseWriter, request *http.Request) {
	writer.Header().Set("Content-Type", "application/json")

	var server *Server

	decoder := json.NewDecoder(request.Body)
	err := decoder.Decode(&server)

	if err != nil {
		writer.WriteHeader(400)
		json.NewEncoder(writer).Encode(Response{true, "Could not parse body", nil})
		return
	}

	if server.Name == "" {
		server.Name = "Default oakwood server"
	}

	if server.MaxPlayers == 0 || server.ServerPort == "" {
		writer.WriteHeader(400)
		json.NewEncoder(writer).Encode(Response{true, "Required fields are missing", nil})
		return
	}

	host := request.Header.Get("X-Real-IP")
	serverID := host

	if server.ServerHost == "" {
		server.ServerHost = host
	}

	checkAddr := server.ServerHost

	// Alias servers running under OpenVPN tunnel to its real client.
	if server.ServerHost == "madaraszd.net@openvpn" {
		checkAddr = "10.8.0.6"
		server.ServerHost = "madaraszd.net"
	}

	port, _ := strconv.Atoi(server.ServerPort)
	tcpConn, tcpErr := net.DialTimeout("tcp", fmt.Sprintf("%s:%d", checkAddr, port), time.Second*4.0)

	if tcpErr != nil {
		writer.WriteHeader(400)
		json.NewEncoder(writer).Encode(Response{true, "Your server is not visible to the masterlist!", nil})
		log.Printf("Server %s:%s with name '%s' is not visible!\n", checkAddr, server.ServerPort, server.Name)
		return
	}

	tcpConn.Close()

	server.updatedAt = time.Now().Unix() + defaultTimeout

	mutex.Lock()
	servers[serverID] = server
	mutex.Unlock()

	log.Printf("Server %s:%s with name '%s' was updated successfully!\n", host, server.ServerPort, server.Name)

	json.NewEncoder(writer).Encode(Response{false, "", nil})
}

// GetServers retrieves all servers from the list
func GetServers(writer http.ResponseWriter, _ *http.Request) {
	writer.Header().Set("Content-Type", "application/json")
	writer.Header().Set("Access-Control-Allow-Origin", "*")
	writer.Header().Set("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept")

	serverList := []*Server{}

	mutex.Lock()
	for _, v := range servers {
		serverList = append(serverList, v)
	}
	mutex.Unlock()

	json.NewEncoder(writer).Encode(Response{false, "", serverList})
}
