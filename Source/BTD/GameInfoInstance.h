// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <activation.h>

#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameInfoInstance.generated.h"

/**
 * 
 */
UCLASS()
class BTD_API UGameInfoInstance : public UGameInstance
{
	GENERATED_BODY()

	/*Delegate called when session created*/
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	/*Delegate called when session started*/
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/*Handles for delegates that are creating/starting/Finding/Joining/Destroying a session*/
	FDelegateHandle onCreateSessionCompleteDelegateHandle;
	FDelegateHandle onStartSessionCompleteDelegateHandle;
	FDelegateHandle onFindSessionCompleteDelegateHandle;
	FDelegateHandle onJoinSessionCompleteDelegateHandle;
	FDelegateHandle onDestroySessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> sessionSettings;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	UGameInfoInstance(const FObjectInitializer& objectInitializer);

	/**
	 * Function to host game! 
	 * @param userID_  User that started the host game request
	 * @param sessionName_ Name of session
	 * @param bIslAN_ Is this a LAN game or an internet game?
	 * @param bIsPresence_ Are we creating a Presence session? 
	 * @param maxPlayers_ Maximum number of players allowed in session
	 * @return 
	 */
	bool HostSession(TSharedPtr<const FUniqueNetId> userID_, FName sessionName_, bool bIslAN_, bool bIsPresence_, int maxPlayers_);

	/**
	 * Find an online session
	 * @param userID_ User that started the search request
	 * @param bIsLAN_ Are we searching LAN matches?
	 * @param bIsPresence_ Are we searching presence sessions?
	 */
	void FindSessions(TSharedPtr<const FUniqueNetId> userID_,  bool bIsLAN_, bool bIsPresence_);

	/**
	 *  Joins a session based on a search result
	 * @param userID_ User who initiated the search
	 * @param sessionName_ Name of session
	 * @param searchResult_ Session to join
	 * @return bool true if successful, false otherwise
	 */
	bool JoinSessions(TSharedPtr<const FUniqueNetId> userID_, FName sessionName_, const FOnlineSessionSearchResult& searchResult_);

	/**
	 * Function fired when a session create request is completed 
	 * @param sessionName_ the name of the session this call is for
	 * @param bWasSuccessful_ true if the action completed without error, false otherwise
	 */
	virtual void OnCreateSessionComplete(FName sessionName_, bool bWasSuccessful_);

	/**
	 * Function fired when a session start request is completed 
	 * @param sessionName_ the name of the session this call is for
	 * @param bWasSuccessful_ true if the action completed without error, false otherwise
	 */
	virtual void OnStartOnlineGameComplete(FName sessionName_, bool bWasSuccessful_);

	/**
	 * Delegate fired when a session search is completed
	 * @param bWasSuccessful_ true if action completed without error, false otherwise
	 */
	void OnFindSessionsComplete(bool bWasSuccessful_);

	/**
	 * Delegate fired when a join session request has completec
	 * @param sessionName_ Name of session this call is for
	 * @param result_ true if the action completed without error, false otherwise
	 */
	void OnJoinSessionComplete(FName sessionName_, EOnJoinSessionCompleteResult::Type result_);

	/**
	 * Delegate fired when destroying a session is completed
	 * @param sessionName_ Name of session this call is for
	 * @param bWasSuccessful_ true if action completed without error, false otherwise
	 */
	virtual void OnDestroySessionComplete(FName sessionName_, bool bWasSuccessful_);

	// UFUNCTION(BlueprintCallable, Category = "Network")
	// void StartOnlineGame();
	//
	// UFUNCTION(BlueprintCallable, Category = "Network")
	// void FindOnlineGame();
	//
	// UFUNCTION(BlueprintCallable, Category = "Network")
	// void JoinOnlineGame();
	//
	// UFUNCTION(BlueprintCallable, Category = "Network")
	// void DestroySessionAndLeaveGame();
};
