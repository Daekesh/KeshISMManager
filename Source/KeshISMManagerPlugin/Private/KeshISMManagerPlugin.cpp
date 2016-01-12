// Copyright 2015 Matthew Chapman, Inc. All Rights Reserved.

#include "KeshISMManagerPluginPrivatePCH.h"
#include "KeshISMManagerPlugin.h"

IMPLEMENT_MODULE( FKeshISMManagerPlugin, KeshISMManagerPlugin )
DEFINE_LOG_CATEGORY( LogKeshISMManager );

void FKeshISMManagerPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FKeshISMManagerPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
