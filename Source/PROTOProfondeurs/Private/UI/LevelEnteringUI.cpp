#include "UI/LevelEnteringUI.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LevelNameSubsystem.h"

void ULevelEnteringUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<ULevelNameSubsystem>();
	AreaName->SetText(Subsystem->GetLevelName().AreaZone);
	RegionName->SetText(Subsystem->GetLevelName().RegionZone);
}
