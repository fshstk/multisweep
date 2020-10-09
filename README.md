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
  and select your build target;
- Task for opening the app after building (`Cmd-Shift-B` → `Run Task` →
  `Open Mac debug build`).

## Usage

It's important that you **follow these instructions in the right order**. If you
don't, builds will probably fail.

You will need the [JUCE Framework](https://juce.com/) if you don't have it
already.

You will also need to have XCode installed. You won't need to open it ever, but
VS Code's build tasks use the generated XCode project to determine the build
configuration.

### Merge branch

You will need to merge either the `app`, `audioapp` or the `plugin` branch into
your `master` branch, depending on what sort of project you want to create.

### Run init.sh

Before you get started with your project, you will need to:

- Change `<JUCERPROJECT id="......">` to a new and unique six-character
  alphanumeric ID in `NewProject.jucer`;
- Update the location of your JUCE library directory in
  `.vscode/c_cpp_properties.json` (this defaults to `~/JUCE`, so if this is where
  you have installed JUCE, there's no need to change anything);
- Find/replace all occurrences of `NewProject` with your new project name.
  (Careful: this is used as part of the default class names, so make sure you
  format your name in PascalCase);
- Rename `NewProject.jucer`.

I've included a shell script that takes care of all this:

```bash
chmod +x init.sh
./init.sh
```

### Generate JUCE files

Open `NewProject.jucer` in _Projucer_ and open & save the project. You don't
need to change any settings, this just generates the `Build` and
`JuceLibraryCode` directories that are omitted by `.gitignore`. However this
would be a good point to rename your project, and to customize any settings, or
add any modules you might want.

### Get started

That's it. If you have the
[command-line](https://code.visualstudio.com/docs/setup/mac#_launching-from-the-command-line)
tool installed, just run `code path/to/project` to get started!

You may be prompted to install VS Code's [C++
extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools),
which you will need for the features in this template to work properly.

If you want to start with a fresh repository, you can also run something like
[this](https://stackoverflow.com/a/23486788/13135932) to reset the commit
history:

```bash
git reset $(git commit-tree HEAD^{tree} -m "Initial commit")
```

## License

This template is distributed under the MIT license.
