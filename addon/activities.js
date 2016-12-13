
var port = null;
var tmpTabs = [];
var tabsToCreate = 0;

console.log("File load");
setTimeout(init, 2000);



function onError(error)
{
	console.log("Error: " + error);
}


function init()
{
	console.log("Init");
	port = browser.runtime.connectNative("kdeactivitiesffnc");

	port.onMessage.addListener(onMessage)
	browser.tabs.onCreated.addListener(onTabCreated)

	// Debug: Get all Windows and their title
	var getting = browser.tabs.query({active: true});
	getting.then(printWindowList, onError);
}

function postMessage(kwsid, type, data)
{
	var message = {
		kwsid : kwsid,
		type  : type,
		data  : data
	};
	port.postMessage(message);
}


function onMessage(m)
{
	var type  = m['type']
	var kwsid = m['kwsid']
	var data  = m['data']

	if (type == "title_settoid")
	{
		console.log("Message: title_settoid")

		var excludeArray = data["exclude"];
		console.log("Exclude: " + excludeArray)

		tmpTabs = [];
		var getting = browser.windows.getAll();
		getting.then(function (windowInfoArray)
		{
			tabsToCreate = windowInfoArray.length;
			console.log("browser.windows.getAll - tabsToCreate: " + tabsToCreate )
			for (windowInfo of windowInfoArray)
			{
				if (excludeArray.indexOf(windowInfo.id) != -1)
				{
					tabsToCreate = tabsToCreate - 1;
					continue;
				}
				var creating = browser.tabs.create({active: true, windowId: windowInfo.id, url:"http://example.org"});
				creating.then(function (tab)
				{
					tmpTabs.push(tab.id);
					var executing = browser.tabs.executeScript(tab.id, {code:"document.title = '" + tab.windowId + "'"})
					executing.then(function ()
					{
						if (tmpTabs.length == tabsToCreate)
						{
							console.log("Created all tabs: " + tabsToCreate)
							setTimeout(function ()
							{
								postMessage("ffwindowsmanager", "title_tabscreated", "")
							}, 500);

						}
					}, onError);
				}, onError);
			}
		}, onError);
	}
	else if (type == "title_restore")
	{
		console.log("Message: title_restore")
		for (tabid of tmpTabs)
		{
			browser.tabs.remove(tabid);
		}
		tmpTabs = [];
	}
	else if (type == "singleupdate")
	{
		var excludeArray = data["exclude"];
		var getting = browser.windows.getAll();
		getting.then(function (windowInfoArray)
		{
			var unknownWindows = []
			for (windowInfo of windowInfoArray)
			{
				if (excludeArray.indexOf(windowInfo.id) != -1)
				{
					continue;
				}
				unknownWindows.push(windowInfo.id);
			}
			postMessage("ffwindowsmanager", "singleupdate_unknownwindows", unknownWindows)
		}, onError);
	}
	else if (type == "movetabtonewwindow")
	{
		var tid = data["tid"];
		var wid = data["wid"]; // old window
		var creating = browser.windows.create({tabId: tid});
		creating.then(function(windowInfo)
		{
			browser.windows.update(wid, {drawAttention: false});
		}, onError);
	}
	else if (type == "movetabtowindow")
	{
		var tid = data["tid"];
		var wid = data["wid"];
		var moving = browser.tabs.move(tid, {windowId:wid, index:-1});
		moving.then(function(tab)
		{
			browser.tabs.update(tid, {active:true})
		}, onError);
	}
	else
	{
		console.log("Message: Unkown: " + m)
	}
}

function printWindowList(tabs)
{
	console.log("printWindowList:");
	var list = []
	for (tab of tabs)
	{
		console.log("Window(" + tab.windowId + ", " + tab.title + ")")
	}
}


function logWindow(windowInfo)
{
	console.log("Window: " + windowInfo.id)
}


function onTabCreated(tab)
{
	console.log("onTabCreated: ")
	console.log(" ID: " + tab.id)
	console.log(" session ID: " + window.sessionId)
	console.log(" window ID: " + tab.windowId)
	var data = {
		"wid" : tab.windowId,
		"tid" : tab.id
	}
	postMessage("ffwindowsmanager", "newtab", data);
}

