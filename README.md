# JUCE Project Template for VS Code

This is a blank project template for creating and maintaining projects based on
the [JUCE Framework](https://juce.com/) in VS Code on Mac, with tentative and
untested support for Linux. It uses the XCode build targets that are
pre-configured by the _Projucer_ application and offers a couple of nifty
features:

- Auto-formatting with `clang-format`, based on the
  [Mozilla C++ style](https://firefox-source-docs.mozilla.org/code-quality/coding-style/coding_style_cpp.html);
- Pre-configured IntelliSense search paths with tooltips for JUCE modules;
- Build tasks for _Release_ and _Debug_ targets that parse the compiler output
  right back into VS Code in the case of errors/warnings. Just hit `Cmd-Shift-B`
  and select your build target.
- Task for opening the app after building (`Cmd-Shift-B` → `Run Task` →
  `Open Mac debug build`)

## Usage

### Install dependencies

You will need to have XCode installed. You won't need to open it ever, but VS
Code's build tasks use the generated XCode project to determine the build
configuration.

Next, download and install the [JUCE Framework](https://juce.com/). Open
`TODO.jucer` in _Projucer_ and open & save the project. You don't need to change
any settings, this just generates the `Build` and `JuceLibraryCode` directories
that are omitted by `.gitignore`. However this would be a good point to rename
your project, and to customize any settings, or add any modules you might want.

### Change JUCE folder location

This template assumes that the JUCE framework directory is located in your home
folder. If you have installed JUCE somewhere else, make sure to change your
browse path in `.vscode/c_cpp_properties.json` for the Mac or Linux
configuration.

### Merge branch and clean up repository

You will need to merge either the `app` or the `plugin` branch into your
`master` branch, depending on whether you want to make an audio application or a
plug-in respectively.

If you want to start with a fresh repository, you can also run something like
[this](https://stackoverflow.com/a/23486788/13135932) to reset the commit
history:

```bash
git reset $(git commit-tree HEAD^{tree} -m "Initial commit")
```

### Get started

That's it. If you have the
[command-line](https://code.visualstudio.com/docs/setup/mac#_launching-from-the-command-line)
tool installed, just run `code path/to/project` to get started!

You may be prompted to install VS Code's [C++
extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools),
which you will need for the features in this template to work properly.

## License

This template is distributed under the MIT license.
