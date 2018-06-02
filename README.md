# rackcli
Commandline-interface (CLI) for [VCV Rack](https://vcvrack.com/) to help with managing your local rack installation, installed plugins, and patches. Additionally, it provides a complementary incubation venue for prototyping/implementing features/capabilities experimentally in consideration in the official VCV Rack environment under an alternative development model.

rackcli CLI "features" cater towards making your life easier for managing your rack installation as well as providing lots of "magic" to make your patching/wiggling/performance workflow smoother and faster than ever! Core to these features is the cocept of centralized metadata for all the
installed plugins and their modules which I refer to as the "catalog" -- it's actually a simple
JSON file named `catalog.json` residing in the Rack directory. The care and feeding of the catalog depends on one of two mechanisms 1) my [fork of VCV Rack](https://github.com/dirkleas/Rack), or 2) my plugin module [DLwigglz r4xH4x](https://github.com/dirkleas/DLwigglz). Reference/example versions of the catalog files
are included in the fork directory. The advantage of the
 "fork" is that it provides several other cool workflow enhancements -- check it out for details -- highlights include:

1. keyboard shortcut for File.Revert (renamed reOpen CMD-Shift-C)
2. new File.Catalog option for creating a complete list of all your installed plugin modules in JSON format -- great for querying, reporting, and feeding pigeons in the park
3. enhanced File.Save/SaveAs which saves plugin module width data in your patches which is 100% compatible with normal VCV Rack patches

If you go with my Rack fork, and use the new `File.Catalog` menu option, a full `catalog.json`
file will be generated. It's possible given the range of "official" and experimental plugin
 modules available out there, that or more modules may "misbehave" and crash Rack. If this happens, don't
panic. Just look at the `log.txt` log file or your OS-specific crash dump facilities to identify the offending module and add it to a fresh patch along with `DLwigglz r4xH4x` and click the `patch` button. This will generate a `patch.json` file with the important metadata for the offending patch(s). You can then run `p2f` from the fork directory to capture this in the
 `faults.json` file which will keep `File.Catalog` from tripping over the faulty modules.
 It's good practice to report a bug on the plugin module's developer Github site (look that
up using the official VCV Rack [Plugin Manager page](https://vcvrack.com/plugins.html)).

How does rackcli fit into a typical rack workflow? YMMV, but I'll be using rackcli alongside rack so I can bounce down to my shell prompt, kick off a rackcli command or two, then back to rack for a quick *File+Revert* to revert to the updated patch file (of course I regularly hit *META-S* to save my wigglz) -- walah, instant magic!

To get a sense of how the fork and plugin modules work, check out the rackcli/r4xH4x [lab videos](https://www.youtube.com/channel/UCv-mq6lyycCbvbQiZclik7Q).

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
rackcli is implemented in [Python 3](https://www.python.org/downloads/). Once you download and install the latest Python 3 version for your OS, install the dependent libraries as follows:
```
pip3 install requests click
```
Clone the supporting [VCV Rack fork](https://github.com/dirkleas/Rack) and follow the installation instructions for the best workflow experience as it generates a full `catalog.json`, complete with plugin module widths. Don't forget to grab the v0.6 tag per the instructions (i.e. `git checkout v0.6`) as specified in the build installation instructions.

Alternatively, your can install the [DLwigglz r4xH4x](https://github.com/dirkleas/DLwigglz) plugin
module to generate a partial catalog `catalog.partial.json` or `patch.json` for the current patch.

Optionally, [jq](https://stedolan.github.io/jq/) is an awesome open source tool for hacking JSON files. Here are a couple of interesting use cases with the generated `catalog.json` and `settings.json`:
```
export RACK_DIR=wherever-rack-is-installed

cat $RACK_DIR/catalog.json | jq
cat $RACK_DIR/catalog.partial.json | jq
cat ANY_PATCH.vcv | jq
cat $RACK_DIR/settings.json | jq '.lastPath'
```

**Example Usage -- Live Features (beta)**
```
alias rackcli="/path-to-rachcli/rackcli"

rackcli
rackcli --help

rackcli settings --help
rackcli settings
rackcli settings --rackdir ~/Downloads/Rack

rackcli catalog --help
rackcli catalog --share $RACK_DIR/catalog.json | jq
rackcli catalog --share $RACK_DIR/dox.vcv > proof.json
rackcli catalog --sync $RACK_DIR/catalog.partial.json
```

**Feature Brainstorm**
```
rackcli --version
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
