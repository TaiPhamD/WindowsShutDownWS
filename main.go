package main

import (
	"bufio"
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

var Mypassword string
var MyPort string
var logger service.Logger

func (p *program) Start(s service.Service) error {
	// Start should not block. Do the actual work async.
	go p.run()
	return nil
}
func (p *program) run() {

	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(dir)

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

	fmt.Println("My password is:", Mypassword)
	err = http.ListenAndServe(":"+MyPort, shutdownHandler{})
}
func (p *program) Stop(s service.Service) error {
	// Stop should not block. Return with a few seconds.
	return nil
}

func (h shutdownHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {

	_, pass, _ := r.BasicAuth()
	fmt.Println(pass)
	if pass != Mypassword {
		fmt.Println("Password Doesnt match\n")
		return
	}
	//compile the shutdownDLL from here:
	//and place it in c:\Windows\system32
	shutdown := syscall.MustLoadDLL("shutdownDLL")
	defer shutdown.Release()
	funcshut := shutdown.MustFindProc("MySystemShutdown")
	r1, _, err := funcshut.Call()
	if r1 != 1 {
		fmt.Println("Failed to initiate shutdown:", err)
	}
	fmt.Fprintf(w, "hello, you've hit %s\n", r.URL.Path)

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
}
