// Copyright 2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma  once

class FKeshISMManagerPlugin : public IKeshISMManagerPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
