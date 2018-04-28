
#include "app.hpp"
#include "plugin.hpp"
#include "util/common.hpp"
#include <jansson.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <list>

#if ARCH_WIN
	#include <windows.h>
	#include <direct.h>
	#define mkdir(_dir, _perms) _mkdir(_dir)
#else
	#include <dlfcn.h>
#endif
#include <dirent.h>

#if ARCH_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <pwd.h>
#endif

#if ARCH_WIN
#include <Windows.h>
#include <Shlobj.h>
#endif

#if ARCH_LIN
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

using namespace rack;

std::string getPlugsDir() {
	std::string dir;

#if ARCH_MAC
	// Get home directory
	struct passwd *pw = getpwuid(getuid());
	dir = pw->pw_dir;
	dir += "/Documents/Rack/plugins";
#endif

#if ARCH_WIN
	// Get "My Documents" folder
	char buf[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buf);
	dir = buf;
	dir += "/Rack/plugins";
#endif

#if ARCH_LIN
	const char *home = getenv("HOME");
	if (!home) {
		struct passwd *pw = getpwuid(getuid());
		home = pw->pw_dir;
	}
	dir = home;
	dir += "/.Rack/plugins";
#endif

	return dir;
}

static bool serializePluginsJSON() {
	json_t *metaj = json_object(); // meta as json, plugins as json, etc
	json_t *psj = json_array();
	json_object_set_new(metaj, "applicationName", json_string(gApplicationName.c_str()));
	json_object_set_new(metaj, "applicationVersion", json_string(gApplicationVersion.c_str()));
	json_object_set_new(metaj, "apiHost", json_string(gApiHost.c_str()));
	json_object_set_new(metaj, "token", json_string(gToken.c_str()));
	json_object_set_new(metaj, "path", json_string(getPlugsDir().c_str()));
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
			json_object_set_new(mj, "width", json_integer(69));
			json_t *tsj = json_array();
			for (ModelTag tag : model->tags) {
				json_array_append(tsj, json_string(gTagNames[tag].c_str()));
			}
			json_object_set_new(mj, "tags", tsj);
			json_array_append(msj, mj);
		}
		json_array_append(psj, pj);
	}
	json_object_set_new(metaj, "plugins", psj);

	printf("%s\n", json_dumps(metaj, 0));

	return true;
}

// Load one plugin
static bool loadPlug(std::string path) {
	std::string libraryFilename;
#if ARCH_LIN
	libraryFilename = path + "/" + "plugin.so";
#elif ARCH_WIN
	libraryFilename = path + "/" + "plugin.dll";
#elif ARCH_MAC
	libraryFilename = path + "/" + "plugin.dylib";
#endif

	// Check file existence
	if (!systemIsFile(libraryFilename)) {
		printf("Plugin file %s does not exist\n", libraryFilename.c_str());
		return false;
	}

	// Load dynamic/shared library
#if ARCH_WIN
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	HINSTANCE handle = LoadLibrary(libraryFilename.c_str());
	SetErrorMode(0);
	if (!handle) {
		int error = GetLastError();
		printf("Failed to load library %s: code %d\n", libraryFilename.c_str(), error);
		return false;
	}
#else
	void *handle = dlopen(libraryFilename.c_str(), RTLD_NOW);
	if (!handle) {
		printf("Failed to load library %s:\n%s\n", libraryFilename.c_str(), dlerror());
		return false;
	}
#endif

	// Call plugin's init() function
	typedef void (*InitCallback)(Plugin *);
	InitCallback initCallback;
#if ARCH_WIN
	initCallback = (InitCallback) GetProcAddress(handle, "init");
#else
	initCallback = (InitCallback) dlsym(handle, "init");
#endif
	if (!initCallback) {
		printf("Failed to read init() symbol in %s\n", libraryFilename.c_str());
		return false;
	}

	// Construct and initialize Plugin instance
	Plugin *plugin = new Plugin();
	plugin->path = path;
	plugin->handle = handle;
	initCallback(plugin);

	// Reject plugin if slug already exists
	Plugin *oldPlugin = pluginGetPlugin(plugin->slug);
	if (oldPlugin) {
		printf("Plugin \"%s\" is already loaded, not attempting to load it again\n", plugin->slug.c_str());
		// TODO
		// Fix memory leak with `plugin` here
		return false;
	}

	// Add plugin to list
	gPlugins.push_back(plugin);

	return true;
}

// Load all plugins
static bool loadAllPlugs() {
	// Get local plugins directory
	std::string localPlugins = getPlugsDir();

	// Load the built-in core plugin
	Plugin *corePlugin = new Plugin();
	init(corePlugin);
	gPlugins.push_back(corePlugin);

	// Load all plugins
	for (std::string pluginPath : systemListEntries(localPlugins)) {
		if (systemIsDirectory(pluginPath)) {
			loadPlug(pluginPath);
		}
	}

	return true;
}

int main(int argc, char* argv[]) {
	std::string libraryPath;
	tagsInit();

	if(argc == 1) {
		loadAllPlugs();
	}
	else if(argc == 2) {
		libraryPath = argv[1];
		loadPlug(libraryPath);
	}
	else {
		printf("Usage: %s [plugin directory]", argv[0]);
		return -1;
	}

	serializePluginsJSON(); // serialize JSON

	// Unload all plugins
	pluginDestroy();

	return 0;
}
