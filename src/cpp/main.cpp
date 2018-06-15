
/*
	main.cpp - VCV Rack src/main.cpp replacemet for JSON catalog h4x.
	howto: clone rack, replace src/main.cpp, make as usual, then rename Rack to r4xh4x
	todo: clean up/simplify as time allows
*/

#include "util/common.hpp"
#include "app.hpp"
#include "plugin.hpp"
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#if ARCH_WIN
	#include <windows.h>
	#include <direct.h>
	#define mkdir(_dir, _perms) _mkdir(_dir)
#else
	#include <dlfcn.h>
#endif

using namespace rack;

std::string gLog = "";

static bool loadPlugin(std::string path) {
	std::string libraryFilename;
#if ARCH_LIN
	libraryFilename = path + "/" + "plugin.so";
#elif ARCH_WIN
	libraryFilename = path + "/" + "plugin.dll";
#elif ARCH_MAC
	libraryFilename = path + "/" + "plugin.dylib";
#endif

	if (!systemIsFile(libraryFilename)) {
		// std::cout << "Plugin file " << libraryFilename << " does not exist" << std::endl;
		gLog += "Plugin file " + libraryFilename + " does not exist";
		return 1;
	}

#if ARCH_WIN
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	HINSTANCE handle = LoadLibrary(libraryFilename.c_str());
	SetErrorMode(0);
	if (!handle) {
		int error = GetLastError();
		// std::cout << "Failed to load library " << libraryFilename << ": code " << error << std::endl;
		gLog += "Failed to load library " + libraryFilename + ": code " + std::to_string(error);
		return 1;
	}
#else
	void *handle = dlopen(libraryFilename.c_str(), RTLD_NOW);
	if (!handle) {
		// std::cout << "Failed to load library " << libraryFilename << ": code " << dlerror() << std::endl;
		gLog += "Failed to load library " + libraryFilename + ": code " + dlerror();
		return 1;
	}
#endif

	typedef void (*InitCallback)(Plugin *);
	InitCallback initCallback;
#if ARCH_WIN
	initCallback = (InitCallback) GetProcAddress(handle, "init");
#else
	initCallback = (InitCallback) dlsym(handle, "init");
#endif
	if (!initCallback) {
		// std::cout << "Failed to read init() symbol in " << libraryFilename << std::endl;
		gLog += "Failed to read init() symbol in " + libraryFilename;
		return false;
	}

	Plugin *plugin = new Plugin();
	plugin->path = path;
	plugin->handle = handle;
	initCallback(plugin);
	gPlugins.push_back(plugin);

	return true;
}

static void loadPlugins(std::string path) {
	int plugins = 0;
	std::string message;
	Plugin *corePlugin = new Plugin();
	init(corePlugin);
	gPlugins.push_back(corePlugin);
	for (std::string pluginPath : systemListEntries(path)) {
		if (!systemIsDirectory(pluginPath)) {
			message += pluginPath + " is an invalid plugin path";
			continue;
		}
		if (!loadPlugin(pluginPath)) {
			message += stringf("Could not load plugin %s\n", pluginPath.c_str());
		}
		plugins++;
	}
	if (!message.empty()) {
		message += "See log for details.";
	}
	else if (!plugins) message += "No pugins in " + path;
	gLog += message;
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout << "usage: " << argv[0] << " PLUGINS_DIR" << std::endl;
		return 1;
	}
	tagsInit();
	loadPlugins(argv[1]);
	if (gPlugins.size() == 0) {
		std::cout << "{\"log\": \"" << gLog << "\"}" << std::endl;
		return 1;
	}

	json_t *metaj = json_object(); // meta as json, plugins as json, etc
	json_t *psj = json_array();
	json_object_set_new(metaj, "applicationName", json_string(gApplicationName.c_str()));
	json_object_set_new(metaj, "applicationVersion", json_string(gApplicationVersion.c_str()));
	json_object_set_new(metaj, "apiHost", json_string(gApiHost.c_str()));
	json_object_set_new(metaj, "token", json_string(gToken.c_str()));
	json_object_set_new(metaj, "path", json_string(getcwd(NULL, 0)));
	json_object_set_new(metaj, "pluginCount", json_integer(gPlugins.size()));
	for (Plugin *plugin : gPlugins) {
		json_t *pj = json_object();
		json_object_set_new(pj, "slug", json_string(plugin->slug.c_str()));
		json_object_set_new(pj, "path", json_string(plugin->path.c_str()));
		json_object_set_new(pj, "version", json_string(plugin->version.c_str()));
		json_object_set_new(pj, "modelCount", json_integer(plugin->models.size()));
		json_t *msj = json_array();
		json_object_set_new(pj, "models", msj);
		for (Model *model : plugin->models) {
			json_t *mj = json_object();
			json_object_set_new(mj, "slug", json_string(model->slug.c_str()));
			json_object_set_new(mj, "name", json_string(model->name.c_str()));
			json_object_set_new(mj, "author", json_string(model->author.c_str()));
			json_t *tsj = json_array();
			for (ModelTag tag : model->tags) {
				json_array_append(tsj, json_string(gTagNames[tag].c_str()));
			}
			json_object_set_new(mj, "tags", tsj);

			// instantiate model for geometry -- someday!?!?
			// ModuleWidget *moduleWidget = model->createModuleWidget();
			// if (!moduleWidget) return 1;
			// json_object_set_new(mj, "width", json_integer(int(moduleWidget->box.size.x)));
			// json_object_set_new(mj, "height", json_integer(int(moduleWidget->box.size.y)));

			json_array_append(msj, mj);
		}
		json_array_append(psj, pj);
	}
	json_object_set_new(metaj, "plugins", psj);
	json_object_set_new(metaj, "log", json_string(gLog.c_str()));
	std::cout << json_dumps(metaj, JSON_INDENT(2) | JSON_REAL_PRECISION(9)) << std::endl;
	return 0;
}
