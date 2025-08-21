#include "FireExtinguisherActor.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "FireActor.h"


AFireExtinguisherActor::AFireExtinguisherActor()
{

	PrimaryActorTick.bCanEverTick = true;

    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    SetRootComponent(Body);

    Nozzle = CreateDefaultSubobject<USceneComponent>(TEXT("Nozzle"));
    Nozzle->SetupAttachment(Body);

    SprayFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SprayFX"));
    SprayFX->SetupAttachment(Nozzle);
    SprayFX->bAutoActivate = false;

    SprayTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SprayTrigger"));
    SprayTrigger->SetupAttachment(Nozzle);
    SprayTrigger->InitSphereRadius(60.f);

    SprayTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SprayTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);

    SprayTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    SprayTrigger->SetGenerateOverlapEvents(true);

    SprayTrigger->OnComponentBeginOverlap.AddDynamic(this, &AFireExtinguisherActor::OnSprayBegin);

}

void AFireExtinguisherActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFireExtinguisherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireExtinguisherActor::StartSpray()
{
    if (bSpraying) return;
    bSpraying = true;
    SprayFX->Activate(true);
    SprayTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AFireExtinguisherActor::StopSpray()
{
    if (!bSpraying) return;
    bSpraying = false;
    SprayFX->Deactivate();
    SprayTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFireExtinguisherActor::OnSprayBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bSpraying) return;

    if (AFireActor* Fire = Cast<AFireActor>(Other))
    {
        Fire->ExtinguishInstant();
        SprayTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        StopSpray();
    }
}

