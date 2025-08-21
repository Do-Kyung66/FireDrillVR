#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireActor.generated.h"

UCLASS()
class FIREDRILLVR_API AFireActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* FireBox;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* FireFX;

	UPROPERTY(EditAnywhere, Category="Fire")
    float FireSize = 1.0f;

	UFUNCTION(BlueprintCallable)
	void ExtinguishInstant();
	
	UFUNCTION(BlueprintCallable)
    void UpdateVisual();




};
