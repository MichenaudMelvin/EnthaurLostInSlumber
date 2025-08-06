// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTQuitMenu.generated.h"

class UENTCustomButton;

UCLASS()
class ENTUI_API UENTQuitMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> QuitGameButton;

	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleQuitGameInteraction();
};
