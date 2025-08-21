#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireExtinguisherActor.generated.h"

UCLASS()
class FIREDRILLVR_API AFireExtinguisherActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireExtinguisherActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Body;
    UPROPERTY(VisibleAnywhere)
	class USceneComponent* Nozzle;
    UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* SprayFX;
    UPROPERTY(VisibleAnywhere)
	class USphereComponent* SprayTrigger;

	bool bSpraying = false;

	UFUNCTION(BlueprintCallable)
	void StartSpray();
    UFUNCTION(BlueprintCallable)
	void StopSpray();

	UFUNCTION()
    void OnSprayBegin(UPrimitiveComponent* Overlapped, AActor* Other,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                      bool bFromSweep, const FHitResult& SweepResult);

};
