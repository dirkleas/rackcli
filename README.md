# rackcli
Commandline-interface (CLI) for [VCV Rack](https://vcvrack.com/) to help with managing your local rack installation, installed plugins, and patches. Additionally, it provides a complementary incubation venue for prototyping/implementing features/capabilities experimentally in consideration in the official VCV Rack environment under an alternative development model.

rackcli CLI "features" cater towards making your life easier for managing your rack installation as well as providing lots of "magic" to make your patching/wiggling/performace workflow smoother and faster than ever! It depends on one of two dependencies (both work-in-process): 1) a [fork of VCV Rack](https://github.com/dirkleas/Rack), or a stand-alone "shim" called rachshim included in this project's source code tree which is a non-GUI derivative of VCV Rack. The goal of both of these at a minimum is to make certain helpful information  (e.g. width of your plugin modules, etc.) available. The advantage of the "fork" is that it provides serveral other cool workflow enhacements -- check it out for details -- highlights include:

1. keyboard shortcut for File.Revert (renamed reOpen CMD-Shift-C)
2. new File.Catalog option for creating a complete list of all your installed plugin modules in JSON format -- great for querying, reporting, and feeding pigeons in the park
3. enhanced File.Save/SaveAs which saves plugin module width data in your patches which is 100% compatible with normal VCV Rack patches

How does rackcli fit into a typical rack workflow? YMMV, but I'll be using rackcli alongside rack so I can bounce down to my shell prompt, kick off a rackcli command or two, then back to rack for a quick *File+Revert* to revert to the updated patch file (of course I regularly hit *META-S* to save my wigglz) -- walah, instant magic!

Here's the highlight reel for features under active development or beta-ready (see [Issues](https://github.com/dirkleas/rackcli/issues) marked "enhancement" for many more under serious consideration -- or add submit your own ideas/requests):

**Rack Features**
1. version
1. get/set rack settings
1. panic (delete autosave.vcv, etc.)
1. ...

**Plugin/Module Features**
1. list plugins
1. list plugin modules
1. list/install non-PluginManager plugins
1. hack module resources
1. documentation generator
1. ...

**Patch Features**
1. layout manager with super-easy module selection syntax for referencing parts of your patch -- like multi-select on steroids -- and the ability to move, delete, shift right/left, squeeze out empty rack space, etc.
1. insert/delete rack row
1. import/export plugins/wiring/settings
1. documentation generator
1. common fragment generator (e.g. CF module mixers, AS re* composers, Turing Machine + Quatizer + Semis composer, or even patching automation for rich modules like Phrase-Seq-16, etc.)
1. social media share
1. ...

**Installation**
rackcli is implemented in [Python 3](https://www.python.org/downloads/). Once you download and install Python 3 for your OS, install the dependent library(s) as follows:
```
pip3 install fuzzywuzzy
```
Clone the supporting [VCV Rack fork](https://github.com/dirkleas/Rack) and follow the installation instructions for the best workflow experience. Don't forget to grab the v0.6 tag per the instructions (i.e. `git checkout v0.6`) as specified in the installation instructions.

Alternatively, your can follow the standard [VCV Rack](https://github.com/VCVRack/Rack) build instructions if you prefer to use the included rackshim (currently unstable), then complete the following additional steps:
```
export RACK_DIR=path-to-rack-source
make
```

If you get an error during make referencing a duplicate main symbol, temporarily delete /path-to-rack-source/src/build/main.cpp.

Optionally, [jq](https://stedolan.github.io/jq/) is an awesome open source tool for hacking JSON files. Here are a couple of interesting use cases with the generated meta.json and settings.json:
```
rachshim | jq
rackshim | jq 'del(.plugins)'
rackshim | jq '.plugins[].slug'
cat $RACK_DIR/settings.json | jq '.lastPath'
```

**Example Usage**
```
alias rackcli="/path-to-rachcli/rackcli"
alias rackshim="DYLD_FALLBACK_LIBRARY_PATH=path-to-vcv-rack /path-to-rackshim/rackshim --json"

rackcli fix ~/Downloads/twilight.vcv --backup
```

**Feature Braistorm**
```
rackcli --version
rackcli --settings
rackcli --settings --update "zoom=1.75 wireOpacity=75.0" --add-favorite 'plugin="Befaco" model="EvenVCO"'
rackcli --panic
rackcli --list
rackcli --documentation
rackcli --layout --row 1 --col 3 --shift-left ; remove empty HP beyond 3rd module
rackcli --layout --row 1:2 --shift-down 2
rackcli --layout --shift-left
rackcli --layout --row 3 --col 2 --add-hp 25 ; insert 25 HP of space before 3rd module
rackcli --share ~/Desktop/ep1cY0.vcv --to [youtube|patchstorage|twitter|instagram|etc] ; prompt for creds, no history
```

Again, check out the open issues marked "enhancement" for additional active conversations and please submit your own features/priorities/suggestions [there](https://github.com/dirkleas/rackcli/issues) as well. You can also reach out via the official [VCV Rack group](https://www.facebook.com/groups/vcvrack/) on Facebook or [message](https://www.facebook.com/dirkleas) me directly -- front there, we can bounce out to Skype or email as appropriate.

--

**Licenses**

All concepts and source code in this repository is licensed under [BSD-3-Clause](LICENSE) by Dirk Leas.
