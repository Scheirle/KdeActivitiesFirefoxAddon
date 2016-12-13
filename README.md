# KdeActivitiesFirefoxAddon

KdeActivitiesFirefoxAddon is a Firefox Add-on to add rudimentary support for KDE Activities.

Opening a link (from an external application) will now always open this link in a Firefox window on the current Activity.
This is done by moving the tab to either a new window or an already existing window on the current Activity.

This Add-on consists of two parts, the Firefox Add-on (WebExtension) and a native component.
The native component is required to know what window is on which Activity.


If you open multiple Firefox windows simultaneously (e.g. on session restore) it is hard for the native component to distinguish them.
To prevent confusion in this case the Add-on opens a special tab in every window (with a different id per window) and closes right away again.


## How to build

	git clone …
	cd KdeActivitiesFirefoxAddon
	mkdir build


### How to build the WebExtensions add-on

	cd ./addon/
	web-ext build

[Getting started with web-ext](https://developer.mozilla.org/en-US/Add-ons/WebExtensions/Getting_started_with_web-ext)


### How to build the native component

	cd ./build/
	cmake .. -DCMAKE_BUILD_TYPE=Release
	make
	sudo make install

#### Dependencies

	* [Qt 5](https://www.qt.io/)
		* Qt5Core
		* Qt5Gui
	* [KF5WindowSystem](https://api.kde.org/frameworks/kwindowsystem/html/index.html)
	* [KF5Activities] (https://api.kde.org/frameworks/kactivities/html/index.html)

## Develop / Test
Start firefox and browse to `about:debugging`.
Add this add on temporary by selecting a file from the `kdeactivities/addon/` subdirectory.

Alternatively you can run `web-ext run` in `kdeactivities/addon/`.

