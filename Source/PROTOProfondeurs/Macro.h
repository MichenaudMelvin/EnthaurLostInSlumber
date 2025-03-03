#pragma once
#define PRINT_TEXT(x) if(IsValid(GEngine)) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT(x)));