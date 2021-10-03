// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfoInstance.h"


#include "OnlineSubsystem.h"

UGameInfoInstance::UGameInfoInstance(const FObjectInitializer& objectInitializer) {
	/*Bind function for finding a session*/
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGameInfoInstance::OnFindSessionsComplete);

	/*Bind function for destroying a session*/
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGameInfoInstance::OnDestroySessionComplete);
}


bool UGameInfoInstance::HostSession(TSharedPtr<const FUniqueNetId> userID, FName sessionName_, bool bIslAN_, bool bIsPresence_, int maxPlayers_) {
	//Get the online subsystem to work with
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();

	if (onlineSub) {
		//Get the session interface, so we can call CreateSession function.
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && userID.IsValid()) {
			/*Fill in all the settings that we want to use*/
			{
				sessionSettings = MakeShareable(new FOnlineSessionSettings());
			
				sessionSettings->bIsLANMatch = bIslAN_;
				sessionSettings->bUsesPresence = bIsPresence_;
				sessionSettings->NumPublicConnections = maxPlayers_;
				sessionSettings->NumPrivateConnections = 0;
				sessionSettings->bAllowInvites = true;
				sessionSettings->bAllowJoinInProgress = true;
				sessionSettings->bShouldAdvertise = true;
				sessionSettings->bAllowJoinViaPresence = true;
				sessionSettings->bAllowJoinViaPresenceFriendsOnly = true;

				sessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);
			}
			
			//Set the delegate to the handle of the sessions interface
			onCreateSessionCompleteDelegateHandle = sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			

			return sessions->CreateSession(*userID, sessionName_, *sessionSettings);
		}
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsystem found!"));

	return false;

}

void UGameInfoInstance::FindSessions(TSharedPtr<const FUniqueNetId> userID_, bool bIsLAN_, bool bIsPresence_) {
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr sessions = OnlineSub->GetSessionInterface();

		if (sessions.IsValid() && userID_.IsValid())
		{
			/*
			Fill in all the SearchSettings
			*/
			{
			sessionSearch = MakeShareable(new FOnlineSessionSearch());

			sessionSearch->bIsLanQuery = bIsLAN_;
			sessionSearch->MaxSearchResults = 20;
			sessionSearch->PingBucketSize = 50;
			}
			
			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence_) sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence_, EOnlineComparisonOp::Equals);

			//Make a reference to our search settings
			TSharedRef<FOnlineSessionSearch> searchSettingsRef = sessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			onFindSessionCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			
			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			sessions->FindSessions(*userID_, searchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

bool UGameInfoInstance::JoinSessions(TSharedPtr<const FUniqueNetId> userID_, FName sessionName_, const FOnlineSessionSearchResult& searchResult_) {
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid() && userID_.IsValid())
		{
			// Set the Handle again
			onJoinSessionCompleteDelegateHandle = sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			
			/*Call the "JoinSession" Function with the passed in "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			"FOnlineSessionSearchResult" and pass it*/
			bSuccessful = sessions->JoinSession(*userID_, sessionName_, searchResult_);
		}
	}
		
	return bSuccessful;
}

void UGameInfoInstance::OnCreateSessionComplete(FName sessionName_, bool bWasSuccessful_) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *sessionName_.ToString(), bWasSuccessful_));

	//Get the OnlineSubsystem so we can get the session interface
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub) {
		//Get the session interface to call the StartSession function
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid()) {
			//clear the SessoinComplete delegate handle, since we finished the call
			sessions->ClearOnCreateSessionCompleteDelegate_Handle(onCreateSessionCompleteDelegateHandle);

			if (bWasSuccessful_) {
				//Set the StartSession delegate handle
				onStartSessionCompleteDelegateHandle = sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				//Call StartSessionComplete delegate
				sessions->StartSession(sessionName_);
			}
		}
	}
}

void UGameInfoInstance::OnStartOnlineGameComplete(FName sessionName_, bool bWasSuccessful_) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *sessionName_.ToString(), bWasSuccessful_));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since this call is complete
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(onStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, open LobbyMap
	if (bWasSuccessful_)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, "listen");
	}
}

void UGameInfoInstance::OnFindSessionsComplete(bool bWasSuccessful_) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful_));

	//Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		//Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid()) {
			//Clear the Delegate handle, since we finished this call
			sessions->ClearOnFindSessionsCompleteDelegate_Handle(onFindSessionCompleteDelegateHandle);

			//Debug the Number of Search results
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), sessionSearch->SearchResults.Num()));
		
			//If we have found at least 1 session, we just going to debug them.
			if (sessionSearch->SearchResults.Num() > 0)
			{
				//"SessionSearch->SearchResults" is an Array that contains all the information.
				for (int32 searchIndex = 0; searchIndex < sessionSearch->SearchResults.Num(); searchIndex++)
				{
					//OwningUserName is just SessionName
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), searchIndex + 1, *(sessionSearch->SearchResults[searchIndex].Session.OwningUserName)));
				}
			}
		}
	}
}

void UGameInfoInstance::OnJoinSessionComplete(FName sessionName_, EOnJoinSessionCompleteResult::Type result_) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *sessionName_.ToString(), static_cast<int32>(result_)));

	//Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		//Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			//Clear the Delegate again
			sessions->ClearOnJoinSessionCompleteDelegate_Handle(onJoinSessionCompleteDelegateHandle);

			/*Get the first local PlayerController (AKA Player index 0), so we can call "ClientTravel" to get to the Server Map
			NOTE: With online play - everyone is player index 0 on their own client so easy peasy here. -Noah*/
			APlayerController * const playerController = GetFirstLocalPlayerController();

			/*Make string for ClientTravel
			Let the SessionInterface construct the String for us by giving him the SessionName and an empty String.
			We want to do this, because every OnlineSubsystem uses different TravelURLs*/
			FString travelURL;

			if (playerController && sessions->GetResolvedConnectString(sessionName_, travelURL))
			{
				//Call the ClientTravel
				playerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UGameInfoInstance::OnDestroySessionComplete(FName sessionName_, bool bWasSuccessful_) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *sessionName_.ToString(), bWasSuccessful_));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
	if (onlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();

		if (sessions.IsValid())
		{
			// Clear the Delegate
			sessions->ClearOnDestroySessionCompleteDelegate_Handle(onDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (Will be MainMenu! -Noah)
			if (bWasSuccessful_)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
			}
		}
	}
}

// void UGameInfoInstance::StartOnlineGame() {
// 	//Create a local player to get userID
// 	ULocalPlayer* const player = GetFirstGamePlayer();
//
// 	//Call HostSession function
// 	HostSession(player->GetPreferredUniqueNetId(), GameSessionName, true, true, 4);
// }
//
// void UGameInfoInstance::FindOnlineGame() {
// 	//Create a local player to get userID
// 	ULocalPlayer* const player = GetFirstGamePlayer();
//
// 	//Call FindSession function
// 	FindSessions(player->GetPreferredUniqueNetId(), true, true);
// }
//
// void UGameInfoInstance::JoinOnlineGame() {
// 	ULocalPlayer* const player = GetFirstGamePlayer();
//
// 	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
// 	FOnlineSessionSearchResult searchResult;
//
// 	// If the Array is not empty, we can go through it
// 	if (sessionSearch->SearchResults.Num() > 0)
// 	{
// 		for (int32 i = 0; i < sessionSearch->SearchResults.Num(); i++)
// 		{
// 			// To avoid something crazy, we filter sessions from ourself
// 			if (sessionSearch->SearchResults[i].Session.OwningUserId != player->GetPreferredUniqueNetId())
// 			{
// 				searchResult = sessionSearch->SearchResults[i];
//
// 				/*Once we found sounce a Session that is not ours, just join it.*/
// 				JoinSession(player->GetPreferredUniqueNetId(), GameSessionName, searchResult);
// 				break;
// 			}
// 		}
// 	}	
// }
//
// void UGameInfoInstance::DestroySessionAndLeaveGame() {
// 	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();
// 	if (onlineSub)
// 	{
// 		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
//
// 		if (sessions.IsValid())
// 		{
// 			sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
//
// 			sessions->DestroySession(GameSessionName);
// 		}
// 	}
// }
