// Fill out your copyright notice in the Description page of Project Settings.


#include "FireExtinguisherActor.h"

// Sets default values
AFireExtinguisherActor::AFireExtinguisherActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFireExtinguisherActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireExtinguisherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

