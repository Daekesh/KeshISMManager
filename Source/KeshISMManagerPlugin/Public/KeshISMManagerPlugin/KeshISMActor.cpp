// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshISMManagerPluginPrivatePCH.h"
#include "KeshISMManagerPlugin/KeshISMManager.h"
#include "KeshISMManagerPlugin/KeshISMActor.h"


AKeshISMActor::AKeshISMActor( const class FObjectInitializer& ObjectInitializer )
{
	Channels.SetNum( 0 );

	RootComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "Scene Root" ) );
}


void AKeshISMActor::PostLoad()
{
	Super::PostLoad();

	if ( GetWorld() == NULL )
		return;

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( !Manager->RegisterWorld( this ) )
		Destroy();
}


void AKeshISMActor::Destroyed()
{
	Super::Destroyed();

	if ( GetWorld() == NULL )
		return;

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();
	Manager->RemoveWorld( GetWorld() );
}