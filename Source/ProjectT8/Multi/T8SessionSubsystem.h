

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "T8SessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreateCompletePin, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionFindCompletePin, bool, bWasSuccessful, const TArray<FBlueprintSessionResult>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoinCompletePin, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyCompletePin, bool, bWasSuccessful);



UCLASS()
class PROJECTT8_API UT8SessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UT8SessionSubsystem();

    void Initialize(FSubsystemCollectionBase& Collection);

    // Blueprint accessible delegates
    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionCreateCompletePin OnSessionCreateComplete_BP;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionFindCompletePin OnSessionFindComplete_BP;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionJoinCompletePin OnSessionJoinComplete_BP;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionDestroyCompletePin OnSessionDestroyComplete_BP;

    // Core session functions (BlueprintCallable for UI interaction)
    UFUNCTION(BlueprintCallable, Category = "Session")
    void CreateSession(int32 MaxPlayers, bool bIsLAN, FString MapName, FString TeamMode, FString SessionName = TEXT("MyGameSession"));

    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindSessions(int32 MaxResults, bool bIsLAN);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void JoinSession(const FBlueprintSessionResult& SearchResult);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void DestroySession();

protected:
    // Internal session interface pointer
    IOnlineSessionPtr SessionInterface;

    // Delegate handles
    FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
    FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
    FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
    FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

    FDelegateHandle OnCreateSessionCompleteDelegateHandle;
    FDelegateHandle OnFindSessionsCompleteDelegateHandle;
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;
    FDelegateHandle OnDestroySessionCompleteDelegateHandle;

    // Callbacks for delegates
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    // Session search object
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
};