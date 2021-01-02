package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"syscall"

	"github.com/kardianos/service"
)

type shutdownHandler struct{}
type program struct{}

var ShutDownFunc interface{}
var RestartFunc interface{}

var Mypassword string
var MyPort string
var logger service.Logger

type auth_struct struct {
	Password   *string
	Mode       *uint32
	Ingredient *string
}

func (p *program) Start(s service.Service) error {
	// Start should not block. Do the actual work async.
	go p.run()
	return nil
}
func (p *program) run() {
	//Get file path from where the exe is launched
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
	log.Print(dir)

	//set up log file
	filelog, errlog := os.OpenFile(dir+"\\info.log", os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if errlog != nil {
		log.Fatal(errlog)
	}

	defer filelog.Close()

	log.SetOutput(filelog)

	//Read in configuration file
	file, err := os.Open(dir + "\\config.txt")
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(file)
	scanner.Scan()
	Mypassword = scanner.Text()
	scanner.Scan()
	MyPort = scanner.Text()
	file.Close()

	//log.Print("My password is:", Mypassword)

	// Load DLL function for windows api
	shutdown := syscall.MustLoadDLL("shutdownDLL")
	defer shutdown.Release()
	ShutDownFunc = shutdown.MustFindProc("MySystemShutdown")
	RestartFunc = shutdown.MustFindProc("MySystemRestart")

	err = http.ListenAndServe(":"+MyPort, shutdownHandler{})
}
func (p *program) Stop(s service.Service) error {
	// Stop should not block. Return with a few seconds.
	return nil
}

func (h shutdownHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {

	decoder := json.NewDecoder(r.Body)
	var jsonAuth auth_struct
	err := decoder.Decode(&jsonAuth)
	if err != nil {
		log.Print("error decoding JSON\n")
		log.Print(r.Body)
		return
	}

	if *(jsonAuth.Password) != Mypassword {
		log.Print("Password from JSON doesn't match\n")
		return
	}
	//log.Print("calling shutdown mode: ", jsonAuth.Mode)
	//log.Print("Ingredient : ", jsonAuth.Ingredient)
	//Call the right shutdown mode based on ingredient and mode

	var effectiveMode = *(jsonAuth.Mode)
	if effectiveMode == 0 {
		r1, _, err := ShutDownFunc.Call()
		if r1 != 1 {
			log.Print("Failed to initiate shutdown:", err)
		}
		fmt.Fprintf(w, "successfully shutdown!\n")
	}

	log.Print("sucessfully executed api!")
}

func main() {

	//config.txt first line is password
	// second line is web service port

	svcConfig := &service.Config{
		Name:        "ShutDownWSListener",
		DisplayName: "ShutDownWSListener",
		Description: "Shutdown Webservice Listener",
	}

	prg := &program{}
	s, err := service.New(prg, svcConfig)
	if err != nil {
		log.Fatal(err)
	}
	logger, err = s.Logger(nil)
	if err != nil {
		log.Fatal(err)
	}
	err = s.Run()
	if err != nil {
		logger.Error(err)
	}

	log.Print("Started service\n")
}