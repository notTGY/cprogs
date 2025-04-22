package main

import (
  "fmt"
  "io/fs"
  "log"
)

func main() {
  data, err := fs.ReadFile( "input.txt")
  if err != nil {
    log.Fatal(err)
  }
  fmt.Printf("%v\n", data)
  return
}
