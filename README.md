# rackcli
Platform commandline-interface (CLI) for [VCV Rack](https://vcvrack.com/) for managing your local rack installation, installed plugins, and patches. Additionally, this provides a complementary incubation venue for prototyping/implementing features/capabilities experimentally in consideration in the official VCV Rack environment under an alternative development model.

rackcli CLI features cater towards making your life easier for both your rack install and the plugins/modules you've installed. Features are composable with JSON output. Cross-platform, single binary without external dependencies or compiler/interpreter installations required.

How does rackcli fit into a typical rack workflow? YMMV, but I'll be using rackcli alongside rack so I can bounce down to my shell prompt, kick off a rackcli command or two, then back to rack for a quick *File+Revert* to revert to the updated patch file (of course I regularly hit *META-S* to save my wigglz) -- walah, instant magic!

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
1. layout manager
1. insert/delete rack row
1. import/export plugins/wiring/settings
1. documentation generator
1. social media share
1. ...

**Example Usage**
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

Check out the open issues for additional active conversations and please submit your own features/priorties/suggestions [there](https://github.com/dirkleas/rackcli/issues) as well. 

--

**Licenses**

All concepts and source code in this repository is licensed under [BSD-3-Clause](LICENSE) by Dirk Leas.
