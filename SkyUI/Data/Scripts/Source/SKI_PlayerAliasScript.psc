scriptname SKI_PlayerAliasScript extends ReferenceAlias  
 
SKI_ItemMenuMainScript property InitQuest auto
 
event OnPlayerLoadGame()
	InitQuest.Setup()
endEvent
