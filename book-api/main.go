// A small API made in go lang to download files on a custom e-reader module via WiFi!

package main

import (
	"fmt"
	"net/http"
	"os"
	"strings"
)

var port = 8080

func main() {
	http.HandleFunc("/library", func(w http.ResponseWriter, r *http.Request) {
		files, error := os.ReadDir(".")
		if error != nil {
			http.Error(w, "error reading directory", http.StatusInternalServerError)
			return
		}

		var bookList []string
		for _, f := range files {
			if !f.IsDir() && strings.HasSuffix(f.Name(), ".txt") {
				bookList = append(bookList, f.Name())
			}
		}

		w.Header().Set("Content-Type", "text/plain")
		w.Write([]byte(strings.Join(bookList, ",")))

	})

	http.HandleFunc("/download", func(w http.ResponseWriter, r *http.Request) {
		bookName := r.URL.Query().Get("bookname")
		if strings.Contains(bookName, "/") || strings.Contains(bookName, "\\") {
			http.Error(w, "improper file name.", http.StatusInternalServerError)
			return
		}

		data, error := os.ReadFile(bookName)

		if error != nil {
			http.Error(w, "file not found", http.StatusNotFound)
			return
		}

		w.Header().Set("Content-Type", "text/plain")
		w.Write(data)
		fmt.Println("Sent", bookName, "to", r.RemoteAddr)
	})

	fmt.Println("API running on port", port)
	http.ListenAndServe(":8080", nil)
}
