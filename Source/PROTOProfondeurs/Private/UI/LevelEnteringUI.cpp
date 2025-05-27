#include "UI/LevelEnteringUI.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LevelNameSubsystem.h"

void ULevelEnteringUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ULevelNameSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<ULevelNameSubsystem>();
	if (!IsValid(Subsystem))
	{
		return;
	}
	
	AreaNameCPP->SetText(Subsystem->GetLevelName().AreaZone);
	RegionNameCPP->SetText(Subsystem->GetLevelName().RegionZone);
}
