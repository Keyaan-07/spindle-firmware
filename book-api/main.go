// A small API made in go lang to download files on a custom e-reader module via WiFi!

package main

import (
	"fmt"
	"net/http"
	"os"
)

func main() {
	http.HandleFunc("/book.txt", func(w http.ResponseWriter, x *http.Request) {
		data, error := os.ReadFile("book.txt")
		if error != nil {
			http.Error(w, "File not found", http.StatusNotFound)
			return
		}

		w.Header().Set("Content-Type", "text/plain")
		w.Write(data)

		fmt.Println("Sent Book data to: ", x.RemoteAddr)
	})

	fmt.Println("API running on port 8080")
	http.ListenAndServe(":8080", nil)
}
