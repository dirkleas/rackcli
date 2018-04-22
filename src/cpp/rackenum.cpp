
/*
  rackenum.cpp - plugin/module[s] introspection

  introspect specified plugin shared library/dll and serialize plugin and
  module details as JSON

  should provide: 1) CLI, and 2) swig-compatible C wrapper
  CLI should support one or more .dylib/.dll references and return a JSON list
*/

#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>
using namespace std;

// minimal rack bits...

bool systemIsFile(std::string path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
		return false;
	return S_ISREG(statbuf.st_mode);
}

int main() {
  // just pick one, iterate on them all later
  std::string libraryFilename = string(std::getenv("HOME")) +
		"/Documents/Rack/plugins/VCV-Console/plugin.dylib";
//		"/Documents/Rack/plugins/cf/plugin.dylib";

  if (! systemIsFile(libraryFilename)) {
    std::cout << "no such plugin: " << libraryFilename << endl;
    return 0;
  }

  // dynamically load and introspect
  void *handle = dlopen(libraryFilename.c_str(), RTLD_NOW);
  if (!handle) {
    std::cout << "Failed to load library " << libraryFilename << endl
      << dlerror() << endl;
  	return 0;
  }

  // introspect and serialize as JSON
  std:cout << "[]" << endl;

  return 1;
}
