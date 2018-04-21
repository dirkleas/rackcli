
// rackcli.go - vcv rack utility scaffolding

package main

import ("fmt"; "strings"; "os"; "path/filepath")

func main() {
  fmt.Println("VCV Rack rulz! rackcli scaffolding on the way, introspecting for now...")

  if root := os.Getenv("RACK_HOME"); root != "" {
    err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
      if strings.HasSuffix(path, ".dylib") { // introspect each plugin/module(s)
        fmt.Printf("%+v\n", path)
      }
      return nil
    })
    if err != nil {
      fmt.Println("error walking rack installed rack plugins")
    }
  } else {
    fmt.Println("missing RACK_HOME")
  }
}
