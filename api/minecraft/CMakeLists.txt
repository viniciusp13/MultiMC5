project(MultiMC_minecraft)

include (UnitTest)

# Support for Minecraft instances and launch
set(MINECRAFT_SOURCES
	# Minecraft support
	auth/AuthSession.h
	auth/AuthSession.cpp
	auth/MojangAccountList.h
	auth/MojangAccountList.cpp
	auth/MojangAccount.h
	auth/MojangAccount.cpp
	auth/YggdrasilTask.h
	auth/YggdrasilTask.cpp
	auth/flows/AuthenticateTask.h
	auth/flows/AuthenticateTask.cpp
	auth/flows/RefreshTask.cpp
	auth/flows/RefreshTask.cpp
	auth/flows/ValidateTask.h
	auth/flows/ValidateTask.cpp
	onesix/BaseInstaller.h
	onesix/BaseInstaller.cpp
	onesix/OneSixUpdate.h
	onesix/OneSixUpdate.cpp
	onesix/OneSixInstance.h
	onesix/OneSixInstance.cpp
	onesix/OneSixProfileStrategy.cpp
	onesix/OneSixProfileStrategy.h
	onesix/OneSixVersionFormat.cpp
	onesix/OneSixVersionFormat.h
	onesix/update/AssetUpdateTask.h
	onesix/update/AssetUpdateTask.cpp
	onesix/update/FMLLibrariesTask.cpp
	onesix/update/FMLLibrariesTask.h
	onesix/update/FoldersTask.cpp
	onesix/update/FoldersTask.h
	onesix/update/LibrariesTask.cpp
	onesix/update/LibrariesTask.h
	launch/CreateServerResourcePacksFolder.cpp
	launch/CreateServerResourcePacksFolder.h
	launch/ModMinecraftJar.cpp
	launch/ModMinecraftJar.h
	launch/DirectJavaLaunch.cpp
	launch/DirectJavaLaunch.h
	launch/ExtractNatives.cpp
	launch/ExtractNatives.h
	launch/LauncherPartLaunch.cpp
	launch/LauncherPartLaunch.h
	launch/PrintInstanceInfo.cpp
	launch/PrintInstanceInfo.h
	legacy/LegacyModList.h
	legacy/LegacyModList.cpp
	legacy/LegacyUpdate.h
	legacy/LegacyUpdate.cpp
	legacy/LegacyInstance.h
	legacy/LegacyInstance.cpp
	legacy/LwjglVersionList.h
	legacy/LwjglVersionList.cpp
	tools/MCEditTool.cpp
	tools/MCEditTool.h
	GradleSpecifier.h
	MinecraftProfile.cpp
	MinecraftProfile.h
	MojangVersionFormat.cpp
	MojangVersionFormat.h
	JarMod.h
	MinecraftInstance.cpp
	MinecraftInstance.h
	MinecraftVersion.cpp
	MinecraftVersion.h
	MinecraftVersionList.cpp
	MinecraftVersionList.h
	Rule.cpp
	Rule.h
	OpSys.cpp
	OpSys.h
	ParseUtils.cpp
	ParseUtils.h
	ProfileUtils.cpp
	ProfileUtils.h
	ProfileStrategy.h
	Library.cpp
	Library.h
	MojangDownloadInfo.h
	VersionBuildError.h
	VersionFile.cpp
	VersionFile.h
	ProfilePatch.h
	VersionFilterData.h
	VersionFilterData.cpp
	Mod.h
	Mod.cpp
	ModList.h
	ModList.cpp
	World.h
	World.cpp
	WorldList.h
	WorldList.cpp

	# FTB
	ftb/OneSixFTBInstance.h
	ftb/OneSixFTBInstance.cpp
	ftb/LegacyFTBInstance.h
	ftb/LegacyFTBInstance.cpp
	ftb/FTBProfileStrategy.h
	ftb/FTBProfileStrategy.cpp
	ftb/FTBInstanceProvider.h
	ftb/FTBInstanceProvider.cpp

	# Assets
	AssetsUtils.h
	AssetsUtils.cpp

	# Forge and all things forge related
	forge/ForgeVersion.h
	forge/ForgeVersion.cpp
	forge/ForgeVersionList.h
	forge/ForgeVersionList.cpp
	forge/ForgeXzDownload.h
	forge/ForgeXzDownload.cpp
	forge/LegacyForge.h
	forge/LegacyForge.cpp
	forge/ForgeInstaller.h
	forge/ForgeInstaller.cpp

	# Liteloader and related things
	liteloader/LiteLoaderInstaller.h
	liteloader/LiteLoaderInstaller.cpp
	liteloader/LiteLoaderVersionList.h
	liteloader/LiteLoaderVersionList.cpp
	SkinUpload.cpp
	SkinUpload.h
	)

add_unit_test(GradleSpecifier
	SOURCES GradleSpecifier_test.cpp
	LIBS MultiMC_minecraft
	)

add_unit_test(MojangVersionFormat
	SOURCES MojangVersionFormat_test.cpp
	LIBS MultiMC_minecraft
	DATA testdata
	)

add_unit_test(Library
	SOURCES Library_test.cpp
	LIBS MultiMC_minecraft
	)

# FIXME: shares data with FileSystem test
add_unit_test(ModList
	SOURCES ModList_test.cpp
	DATA testdata
	LIBS MultiMC_minecraft
	)

add_unit_test(ParseUtils
	SOURCES ParseUtils_test.cpp
	LIBS MultiMC_minecraft
	)

# FIXME: this shouldn't be here. it should be generic.
set(WONKO_SOURCES
	# Wonko
	wonko/tasks/BaseWonkoEntityRemoteLoadTask.cpp
	wonko/tasks/BaseWonkoEntityRemoteLoadTask.h
	wonko/tasks/BaseWonkoEntityLocalLoadTask.cpp
	wonko/tasks/BaseWonkoEntityLocalLoadTask.h
	wonko/format/WonkoFormatV1.cpp
	wonko/format/WonkoFormatV1.h
	wonko/format/WonkoFormat.cpp
	wonko/format/WonkoFormat.h
	wonko/BaseWonkoEntity.cpp
	wonko/BaseWonkoEntity.h
	wonko/WonkoVersionList.cpp
	wonko/WonkoVersionList.h
	wonko/WonkoVersion.cpp
	wonko/WonkoVersion.h
	wonko/WonkoIndex.cpp
	wonko/WonkoIndex.h
	wonko/Wonko.cpp
	wonko/Wonko.h
	wonko/WonkoReference.cpp
	wonko/WonkoReference.h
)

add_unit_test(WonkoIndex
	SOURCES wonko/WonkoIndex_test.cpp
	LIBS MultiMC_minecraft
	)

################################ COMPILE ################################

add_library(MultiMC_minecraft SHARED ${MINECRAFT_SOURCES} ${WONKO_SOURCES})
set_target_properties(MultiMC_minecraft PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN 1)

generate_export_header(MultiMC_minecraft)

# Link
target_link_libraries(MultiMC_minecraft MultiMC_logic)
qt5_use_modules(MultiMC_minecraft Core Xml Network Concurrent)

# Mark and export headers
target_include_directories(MultiMC_minecraft PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
