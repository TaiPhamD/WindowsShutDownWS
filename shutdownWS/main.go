package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"syscall"
	"unsafe"

	"github.com/kardianos/service"
)

type shutdownHandler struct{}
type program struct{}

var ShutDownFunc *syscall.Proc
var RestartFunc *syscall.Proc

var logger service.Logger
var rootDir string

type AuthStruct struct {
	Password   *string
	Mode       *uint32
	Ingredient *string
}

type BootItem struct {
	OS     string
	BootID uint16
}

type ConfigData struct {
	Password string
	Port     string
	BootDict []BootItem
}

var myConfig ConfigData

func main() {

	//Get file path from where the exe is launched
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
	log.Print(dir)
	rootDir = dir
	//set up log file
	filelog, errlog := os.OpenFile(dir+"\\info.log", os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
	if errlog != nil {
		log.Fatal(errlog)
	}
	defer filelog.Close()
	log.SetOutput(filelog)

	svcConfig := &service.Config{
		Name:        "ShutdownWebService",
		DisplayName: "ShutdownWebService",
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

}

func (p *program) Stop(s service.Service) error {
	log.Print("Stopped Shutdown service\n")
	// Stop should not block. Return with a few seconds.
	return nil
}

func (p *program) Start(s service.Service) error {
	// Start should not block. Do the actual work async.
	go p.run()
	return nil
}

func (p *program) run() {
	//Read in configuration file
	log.Print("Started Shutdown service from: " + rootDir + "\n")
	jsonFile, err := os.Open(rootDir + "\\config.json")
	if err != nil {
		log.Fatal(err)
	}
	// defer the closing of our jsonFile so that we can parse it later on
	defer jsonFile.Close()
	byteValue, _ := ioutil.ReadAll(jsonFile)
	json.Unmarshal(byteValue, &myConfig)

	//log.Print("My password is:", Mypassword)

	// Load DLL function for windows api
	shutdown := syscall.MustLoadDLL("shutdownDLL")
	defer shutdown.Release()
	ShutDownFunc = shutdown.MustFindProc("MySystemShutdown")
	RestartFunc = shutdown.MustFindProc("MySystemRestart")
	log.Print("Using cert at : " + rootDir + "\\cert.pem")
	log.Print("Hosting service on port: " + myConfig.Port)
	err = http.ListenAndServeTLS(":"+myConfig.Port, rootDir+"\\cert.pem", rootDir+"\\key.pem", shutdownHandler{})
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}

}

func (h shutdownHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {

	decoder := json.NewDecoder(r.Body)
	var jsonAuth AuthStruct
	err := decoder.Decode(&jsonAuth)
	if err != nil {
		log.Print("error decoding JSON\n")
		log.Print(r.Body)
		return
	}

	if *(jsonAuth.Password) != myConfig.Password {
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
		log.Print("sucessfully executed shutdown api!")
		fmt.Fprintf(w, "successfully shutdown!\n")
		return
	} else {

		if jsonAuth.Ingredient == nil {
			log.Print("Error: trying to restart but ingredient from JSON body is missing")
			return
		}

		if myConfig.BootDict == nil {
			log.Print("Error: config.json does not contain boot dictionary")
		}
		var bootMode uint16
		for _, s := range myConfig.BootDict {
			//log.Print(s.OS)
			//log.Print(s.BootID)
			if strings.ToUpper(s.OS) == strings.ToUpper(*(jsonAuth.Ingredient)) {
				bootMode = s.BootID
				RestartFunc.Call(uintptr(unsafe.Pointer(&bootMode)))
				log.Print("sucessfully executed restart api!")
				return
			}
		}

	}

}
