\page configuration-resource-system About ScheduleMaster's configuration resource system

ScheduleMaster uses a special system with configuration files inside Qt's resource system to provide an easy way to define and change the app's global configuration.
The configuration files are json files and are stored inside the `:/Config` resource folder.

ScheduleMaster supports two types of configuration files:

## Single-data files

These files are used to define non-scalable data, which means, this data has a constant amount of values and cannot be extendet.
For example, the current version number, the changelog or the update request URL.

These files are stored directly inside the `:/Config/` folder and have an individual json structure that is being parsed by the GlobalConfig class.

## Multi-data files

These files are used to define scalable data, which means, this data can be extended with new items by plugins or theoretically the app itself.
For example, the supported languages, themes or the folder locations.

These files are stored inside the `:/Config/<resource>/` folder, where `<resource>` is the name of the resource.

For every resource type, there can be as many files as you want, so for example, plugins can add new themes by adding a new file to the `:/Config/Themes/` folder.
Every file inside the resource folder needs to be a json-array on its root-level and should contain one or more objects with a eqivalent structure.
On startup, all files are being parsed and the data is being merged into a single list.

**Note:** The app istself usually only provides one file per resource type, defining all the natively supported languages, themes etc.
This file is usually named `base.json` and is being loaded first. Plugins should choose another individual name to avoid naming conflicts.

## The connection to the GlobalConfig class
The GlobalConfig class is the main interface inside the application to acces the configuration defined by the json files.

When calling GlobalConfig::initLanguages() and GlobalConfig::init() all json files are parsed and loaded.
Then the class provides several methods to retrive and manipulate that data.

For more information see GlobalConfig.
