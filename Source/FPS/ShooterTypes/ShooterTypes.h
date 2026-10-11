#pragma once

#include "ShooterTypes.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left UMETA(DisplayName = "Turning Left"),
	Right UMETA(DisplayName = "Turning Right"),
	NotTurning UMETA(DisplayName = "Not Turning")
};

USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()
	
	//Shape Cut Factor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ShapeCutFactor_RoundFired = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ShapeCutFactor_Aiming = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ShapeCutFactor_NotAiming = 0.f;
	
	// Scale Factor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ScaleFactor_RoundFired = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ScaleFactor_Aiming = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float ScaleFactor_NotAiming = 0.f;
	
	// Interp Speeds
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float RoundFiredInterpSpeed = 20.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reticle")
	float AimingInterpSpeed = 15.f;
};

UENUM(meta = (Bitflags))
enum class ESpecialEliminationType :uint16
{
	None = 0,
	Headshot =  1 << 0,
	Sequential = 1 << 1,
	Streak = 1 << 2,
	Revenge = 1 << 3,
	Dethrone = 1 << 4,
	Showstopper = 1 << 5,
	FirstBlood = 1 << 6,
	GainedTheLead = 1 << 7,
	TiedTheLead = 1 << 8,
	LostTheLead = 1 << 9,
};

ENUM_CLASS_FLAGS(ESpecialEliminationType)