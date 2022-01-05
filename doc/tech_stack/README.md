# Technology Stack

## Plugin extension points

	A plug-in extension point is an interface provided by a plug-in for others to implement. The plug-in will then retrieve all implementations of the interface and use those implementations. In other words, they extend the functionality of the plugin. Typically, the implementations of an interface are placed in a global pool of objects during plugin initialization, and the plugin can retrieve them from the pool at the end of plugin initialization.

	For example, the Find plugin provides the FindFilter interface to other plugins. In the FindFilter interface, additional search scopes can be added and will be displayed in the Advanced Search dialog box. The Find plugin retrieves all FindFilter implementations from the global pool of objects and displays them in the dialog box. The plug-in forwards the actual search request to the correct FindFilter implementation and then performs the search.

## Using the global object pool

	With ExtensionSystem::PluginManager::addObject(), you can add objects to the global object pool and get objects of a specific type again with ExtensionSystem::PluginManager::getObjects(). This should be used mainly for plug-in extension point implementations.

	Note: Do not put a singleton into the object pool and do not retrieve it from it. Please use the singleton pattern instead.