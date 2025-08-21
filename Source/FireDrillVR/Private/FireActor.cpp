#include "FireActor.h"
#include "Components/BoxComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"


AFireActor::AFireActor()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	FireBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FireBox"));
	FireBox->SetupAttachment(Root);
	FireBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FireBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	FireBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	FireBox->SetGenerateOverlapEvents(true);

	FireFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFX"));
	FireFX->SetupAttachment(Root);
	FireFX->bAutoActivate = true;
}


void AFireActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFireActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireActor::ExtinguishInstant()
{
	if (FireSize <= 0.f) return;
	FireSize = 0.f;
	UpdateVisual();

	FireFX->Deactivate();
	SetActorEnableCollision(false);
	Destroy();
}

void AFireActor::UpdateVisual()
{
	FireFX->SetFloatParameter(TEXT("FireSize"), FireSize);
}

