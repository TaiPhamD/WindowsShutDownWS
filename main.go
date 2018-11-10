package main

import (
	"fmt"
	"log"
	"net/http"
	"syscall"
	"os"
	"bufio"
	"github.com/kardianos/service"
)

type shutdownHandler struct{}

var Mypassword string
var MyPort string

func (h shutdownHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {	

	_, pass, _ := r.BasicAuth()
	fmt.Println(pass)
	if(pass != Mypassword){
		fmt.Println("Password Doesnt match\n")
		return
	}
	//compile the shutdownDLL from here:
	//and place it in c:\Windows\system32
	shutdown := syscall.MustLoadDLL("shutdownDLL")
	defer shutdown.Release()
	funcshut := shutdown.MustFindProc("MySystemShutdown")
	r1,_,err := funcshut.Call()
	if r1 != 1 {
		fmt.Println("Failed to initiate shutdown:", err)
	}
	fmt.Fprintf(w, "hello, you've hit %s\n", r.URL.Path)
	
}

func main() {

	//config.txt first line is password
	// second line is web service port
	file, err := os.Open("config.txt")
    if err != nil {
        log.Fatal(err)
	}
	scanner := bufio.NewScanner(file)
	scanner.Scan()
	Mypassword = scanner.Text()
	scanner.Scan()
	MyPort = scanner.Text()
	file.Close()

	fmt.Println("My password is:",Mypassword)



	err = http.ListenAndServe(":"+MyPort, shutdownHandler{})
	log.Fatal(err)
}