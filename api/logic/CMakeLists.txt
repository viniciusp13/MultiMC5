project(MultiMC_logic)

include (UnitTest)

set(CORE_SOURCES
	# LOGIC - Base classes and infrastructure
	BaseVersionList.h
	BaseVersionList.cpp
	InstanceList.h
	InstanceList.cpp
	FolderInstanceProvider.h
	FolderInstanceProvider.cpp
	BaseVersion.h
	BaseInstance.h
	BaseInstance.cpp
	BaseInstanceProvider.h
	NullInstance.h
	MMCZip.h
	MMCZip.cpp
	MMCStrings.h
	MMCStrings.cpp

	# Prefix tree where node names are strings between separators
	SeparatorPrefixTree.h

	# WARNING: globals live here
	Env.h
	Env.cpp

	# JSON parsing helpers
	Json.h
	Json.cpp

	FileSystem.h
	FileSystem.cpp

	Exception.h

	# RW lock protected map
	RWStorage.h

	# A variable that has an implicit default value and keeps track of changes
	DefaultVariable.h

	# a smart pointer wrapper intended for safer use with Qt signal/slot mechanisms
	QObjectPtr.h

	# Compression support
	GZip.h
	GZip.cpp

	# Command line parameter parsing
	Commandline.h
	Commandline.cpp

	# Version number string support
	Version.h
	Version.cpp

	# A Recursive file system watcher
	RecursiveFileSystemWatcher.h
	RecursiveFileSystemWatcher.cpp
)

add_unit_test(FileSystem
	SOURCES FileSystem_test.cpp
	LIBS MultiMC_logic
	DATA testdata
	)

add_unit_test(GZip
	SOURCES GZip_test.cpp
	LIBS MultiMC_logic
	)

set(PATHMATCHER_SOURCES
	# Path matchers
	pathmatcher/FSTreeMatcher.h
	pathmatcher/IPathMatcher.h
	pathmatcher/MultiMatcher.h
	pathmatcher/RegexpMatcher.h
)

set(NET_SOURCES
	# network stuffs
	net/ByteArraySink.h
	net/ChecksumValidator.h
	net/Download.cpp
	net/Download.h
	net/FileSink.cpp
	net/FileSink.h
	net/HttpMetaCache.cpp
	net/HttpMetaCache.h
	net/MetaCacheSink.cpp
	net/MetaCacheSink.h
	net/NetAction.h
	net/NetJob.cpp
	net/NetJob.h
	net/PasteUpload.cpp
	net/PasteUpload.h
	net/Sink.h
	net/URLConstants.cpp
	net/URLConstants.h
	net/Validator.h
)

# Game launch logic
set(LAUNCH_SOURCES
	launch/steps/PostLaunchCommand.cpp
	launch/steps/PostLaunchCommand.h
	launch/steps/PreLaunchCommand.cpp
	launch/steps/PreLaunchCommand.h
	launch/steps/TextPrint.cpp
	launch/steps/TextPrint.h
	launch/steps/Update.cpp
	launch/steps/Update.h
	launch/LaunchStep.cpp
	launch/LaunchStep.h
	launch/LaunchTask.cpp
	launch/LaunchTask.h
	launch/LoggedProcess.cpp
	launch/LoggedProcess.h
	launch/LogModel.cpp
	launch/LogModel.h
	launch/MessageLevel.cpp
	launch/MessageLevel.h
)

# Old update system
set(UPDATE_SOURCES
	updater/GoUpdate.h
	updater/GoUpdate.cpp
	updater/UpdateChecker.h
	updater/UpdateChecker.cpp
	updater/DownloadTask.h
	updater/DownloadTask.cpp
)

add_unit_test(UpdateChecker
	SOURCES updater/UpdateChecker_test.cpp
	LIBS MultiMC_logic
	DATA updater/testdata
	)

add_unit_test(DownloadTask
	SOURCES updater/DownloadTask_test.cpp
	LIBS MultiMC_logic
	DATA updater/testdata
	)

# Rarely used notifications
set(NOTIFICATIONS_SOURCES
	# Notifications - short warning messages
	notifications/NotificationChecker.h
	notifications/NotificationChecker.cpp
)

# Backend for the news bar... there's usually no news.
set(NEWS_SOURCES
	# News System
	news/NewsChecker.h
	news/NewsChecker.cpp
	news/NewsEntry.h
	news/NewsEntry.cpp
)

# Icon interface
set(ICONS_SOURCES
	# News System
	icons/IIconList.h
	icons/IIconList.cpp
)

# Minecraft services status checker
set(STATUS_SOURCES
	# Status system
	status/StatusChecker.h
	status/StatusChecker.cpp
)

# the screenshots feature
set(SCREENSHOTS_SOURCES
	screenshots/Screenshot.h
	screenshots/ImgurUpload.h
	screenshots/ImgurUpload.cpp
	screenshots/ImgurAlbumCreation.h
	screenshots/ImgurAlbumCreation.cpp
)

set(TASKS_SOURCES
	# Tasks
	tasks/Task.h
	tasks/Task.cpp
	tasks/ThreadTask.h
	tasks/ThreadTask.cpp
	tasks/SequentialTask.h
	tasks/SequentialTask.cpp
)

set(SETTINGS_SOURCES
	# Settings
	settings/INIFile.cpp
	settings/INIFile.h
	settings/INISettingsObject.cpp
	settings/INISettingsObject.h
	settings/OverrideSetting.cpp
	settings/OverrideSetting.h
	settings/PassthroughSetting.cpp
	settings/PassthroughSetting.h
	settings/Setting.cpp
	settings/Setting.h
	settings/SettingsObject.cpp
	settings/SettingsObject.h
)

add_unit_test(INIFile
	SOURCES settings/INIFile_test.cpp
	LIBS MultiMC_logic
	)

set(JAVA_SOURCES
	# Java related code
	java/launch/CheckJava.cpp
	java/launch/CheckJava.h
	java/JavaChecker.h
	java/JavaChecker.cpp
	java/JavaCheckerJob.h
	java/JavaCheckerJob.cpp
	java/JavaInstall.h
	java/JavaInstall.cpp
	java/JavaInstallList.h
	java/JavaInstallList.cpp
	java/JavaUtils.h
	java/JavaUtils.cpp
	java/JavaVersion.h
	java/JavaVersion.cpp
)

add_unit_test(JavaVersion
	SOURCES java/JavaVersion_test.cpp
	LIBS MultiMC_logic
	)

set(TRANSLATIONS_SOURCES
	# Translations
	trans/TranslationDownloader.h
	trans/TranslationDownloader.cpp
)

set(TOOLS_SOURCES
	# Tools
	tools/BaseExternalTool.cpp
	tools/BaseExternalTool.h
	tools/BaseProfiler.cpp
	tools/BaseProfiler.h
	tools/JProfiler.cpp
	tools/JProfiler.h
	tools/JVisualVM.cpp
	tools/JVisualVM.h
)

################################ COMPILE ################################

# we need zlib
find_package(ZLIB REQUIRED)

set(LOGIC_SOURCES
	${CORE_SOURCES}
	${PATHMATCHER_SOURCES}
	${NET_SOURCES}
	${LAUNCH_SOURCES}
	${UPDATE_SOURCES}
	${NOTIFICATIONS_SOURCES}
	${NEWS_SOURCES}
	${STATUS_SOURCES}
	${SCREENSHOTS_SOURCES}
	${TASKS_SOURCES}
	${SETTINGS_SOURCES}
	${JAVA_SOURCES}
	${TRANSLATIONS_SOURCES}
	${TOOLS_SOURCES}
	${WONKO_SOURCES}
	${ICONS_SOURCES}
)

add_library(MultiMC_logic SHARED ${LOGIC_SOURCES})
set_target_properties(MultiMC_logic PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN 1)

generate_export_header(MultiMC_logic)

# Link
target_link_libraries(MultiMC_logic xz-embedded unpack200 ${QUAZIP_LIBRARIES} nbt++ ${ZLIB_LIBRARIES})
qt5_use_modules(MultiMC_logic Core Xml Network Concurrent)
add_dependencies(MultiMC_logic QuaZIP)

# Mark and export headers
target_include_directories(MultiMC_logic PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" PRIVATE "${ZLIB_INCLUDE_DIRS}")
