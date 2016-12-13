
set(CMAKE_AUTOMOC             ON)
set(CMAKE_CXX_STANDARD        14)

find_package(Qt5Core         REQUIRED)
find_package(Qt5Gui          REQUIRED)
find_package(KF5WindowSystem REQUIRED)
find_package(KF5Activities   REQUIRED)

set(KdeActivities_SRCS
	src/StdStream.cpp
	src/Connection.cpp
	src/FFWindow.cpp
	src/FFWindowsManager.cpp
	src/main.cpp
)

add_executable(kdeactivitiesffnc ${KdeActivities_SRCS})

target_link_libraries(kdeactivitiesffnc
	Qt5::Core
	Qt5::Gui
	KF5::WindowSystem
	KF5::Activities
)

install(TARGETS kdeactivitiesffnc RUNTIME DESTINATION /usr/bin/)
install(FILES src/kdeactivitiesffnc.json  DESTINATION /usr/lib/mozilla/native-messaging-hosts/)
