/*
---------------------------------------------------------------------------------------
This source file is part of swgANH (Star Wars Galaxies - A New Hope - Server Emulator)
For more information, see http://www.swganh.org


Copyright (c) 2006 - 2008 The swgANH Team

---------------------------------------------------------------------------------------
*/

#include "UIOpcodes.h"
#include "UIManager.h"
#include "HarvesterObject.h"
#include "UIMessageBox.h"
#include "UIListBox.h"
#include "UIInputBox.h"
#include "UITransferBox.h"
#include "UISkillSelectBox.h"
#include "UIOfferTeachBox.h"
#include "UIPlayerSelectBox.h"
#include "UITicketSelectListBox.h"
#include "UIResourceSelectListBox.h"
#include "UICloneSelectListBox.h"
#include "PlayerObject.h"
#include "Common/MessageDispatch.h"
#include "Common/MessageFactory.h"
#include "Common/Message.h"
#include "Common/DispatchClient.h"
#include "DatabaseManager/Database.h"
#include "DatabaseManager/DataBinding.h"
#include "DatabaseManager/DatabaseResult.h"
#include "LogManager/LogManager.h"

//======================================================================================================================

bool		UIManager::mInsFlag		= false;
UIManager*	UIManager::mSingleton	= NULL;

//======================================================================================================================

UIManager::UIManager(Database* database,MessageDispatch* dispatch) :
mDatabase(database),
mMessageDispatch(dispatch)
{
	_registerCallbacks();
}

//======================================================================================================================

UIManager* UIManager::Init(Database* database,MessageDispatch* dispatch)
{
	if(mInsFlag == false)
	{
		mSingleton = new UIManager(database,dispatch);
		mInsFlag = true;
		return mSingleton;
	}
	else
		return mSingleton;
}

//======================================================================================================================

UIManager::~UIManager()
{
	mUIWindows.clear();

	_unregisterCallbacks();

	mInsFlag = false;
	delete(mSingleton);
}

//======================================================================================================================

void UIManager::_registerCallbacks()
{
	mMessageDispatch->RegisterMessageCallback(opSuiEventNotification,this);
}

//======================================================================================================================

void UIManager::_unregisterCallbacks()
{
	mMessageDispatch->UnregisterMessageCallback(opSuiEventNotification);
}

//======================================================================================================================

UIWindow* UIManager::getUIWindow(uint32 id)
{
	UIWindowMap::iterator it = mUIWindows.find(id);

	if(it != mUIWindows.end())
		return((*it).second);

	return(NULL);
}

//======================================================================================================================

void UIManager::handleDispatchMessage(uint32 opcode,Message* message,DispatchClient* client)
{
	switch(opcode)
	{
		case opSuiEventNotification:
			_processEventNotification(message,client);
		break;

		default: break;
	} 
}

//======================================================================================================================

void UIManager::_processEventNotification(Message* message,DispatchClient* client)
{
	uint32		windowId	= message->getUint32();
	UIWindow*	window		= getUIWindow(windowId);

	gLogger->logMsgF("UI Event %u",MSG_LOW,windowId);

	if(window == NULL)
	{
		gLogger->logMsgF("UIManager::_processEventNotification: could not find window %u",MSG_NORMAL,windowId);
		return;
	}

	window->handleEvent(message);
}

//======================================================================================================================
//
// create a message box
//

void UIManager::createNewMessageBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* text,PlayerObject* playerObject,ui_window_types windowType,uint8 mbType)
{
	uint32 mbId = _getFreeId();

	UIMessageBox* messageBox =  new UIMessageBox(callback,mbId,windowType,eventStr,caption,text,playerObject,mbType);

	mUIWindows.insert(mbId,messageBox);
	playerObject->addUIWindow(mbId);

	messageBox->sendCreate();
}

//======================================================================================================================
//
// create a listbox
//

void UIManager::createNewListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems,PlayerObject* playerObject,ui_window_types windowType,uint8 lbType)
{
	uint32 lbId = _getFreeId();

	UIListBox* listBox =  new UIListBox(callback,lbId,windowType,eventStr,caption,prompt,dataItems,playerObject,lbType);

	mUIWindows.insert(lbId,listBox);
	playerObject->addUIWindow(lbId);

	listBox->sendCreate();
}

//======================================================================================================================
//
// create an input box
//

void UIManager::createNewInputBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* text,const BStringVector dropdownElements,PlayerObject* playerObject,uint8 ibType,ui_window_types windowType,uint16 maxInputLength)
{
	uint32 ibId = _getFreeId();

	UIInputBox* inputBox =  new UIInputBox(callback,ibId,windowType,eventStr,caption,text,dropdownElements,playerObject,ibType,maxInputLength);

	mUIWindows.insert(ibId,inputBox);
	playerObject->addUIWindow(ibId);

	inputBox->sendCreate();
}

//======================================================================================================================
//
// create a skill select box(teaching)
//

void UIManager::createNewSkillSelectListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems,PlayerObject* playerObject,uint8 lbType,PlayerObject* pupil)
{
	uint32 lbId = _getFreeId();

	UISkillSelectBox* listBox =  new UISkillSelectBox(callback,lbId,eventStr,caption,prompt,dataItems,playerObject,lbType,pupil);

	mUIWindows.insert(lbId,listBox);
	playerObject->addUIWindow(lbId);

	listBox->sendCreate();
}
//======================================================================================================================
//
// create a ticket select list box(travel - by command)
//

void UIManager::createNewTicketSelectListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems,PlayerObject* playerObject,string port,Shuttle* shuttle,uint8 lbType)
{
	uint32 lbId = _getFreeId();

	UITicketSelectListBox* ticketSelectBox =  new UITicketSelectListBox(callback,lbId,eventStr,caption,prompt,dataItems,playerObject,port,shuttle,lbType);

	mUIWindows.insert(lbId,ticketSelectBox);
	playerObject->addUIWindow(lbId);

	ticketSelectBox->sendCreate();
}

//======================================================================================================================
//
// create a transfer box(trade)
//

void UIManager::createNewTransferBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* text,const int8* leftTitle,const int8* rightTitle,uint32 leftValue, uint32 rightValue,PlayerObject* playerObject)
{
	uint32 ibId = _getFreeId();

	UITransferBox* transferBox =  new UITransferBox(callback,ibId,eventStr,caption,text,leftTitle,rightTitle,leftValue,rightValue,playerObject);

	mUIWindows.insert(ibId,transferBox);
	playerObject->addUIWindow(ibId);

	transferBox->sendCreate();
}

//======================================================================================================================
//
// create a skill teach request message box(teaching)
//

void UIManager::createNewSkillTeachMessageBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* text,PlayerObject* playerObject,uint8 mbType,PlayerObject* pupil,Skill* skill)
{
	uint32 mbId = _getFreeId();

	UIOfferTeachBox* messageBox =  new UIOfferTeachBox(callback,mbId,eventStr,caption,text,playerObject,mbType,pupil,skill);

	mUIWindows.insert(mbId,messageBox);
	playerObject->addUIWindow(mbId);

	messageBox->sendCreate();
}

//======================================================================================================================
//
// create a player select list box(group loot master)
//

void UIManager::createNewPlayerSelectListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems, std::vector<PlayerObject*> playerList,  PlayerObject* playerObject,uint8 lbType)
{
	uint32 lbId = _getFreeId();

	UIPlayerSelectBox* listBox =  new UIPlayerSelectBox(callback,lbId,eventStr,caption,prompt,dataItems,playerList,playerObject,lbType);

	mUIWindows.insert(lbId,listBox);
	playerObject->addUIWindow(lbId);

	listBox->sendCreate();
}

//======================================================================================================================
//
// create a resource picker list box, used for all types(category,resourceType,resource)
//

void UIManager::createNewResourceSelectListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems,ResourceIdList resourceIdList,PlayerObject* playerObject,uint8 windowType,uint8 lbType)
{
	uint32 lbId = _getFreeId();

	UIResourceSelectListBox* listBox =  new UIResourceSelectListBox(callback,lbId,eventStr,caption,prompt,dataItems,resourceIdList,playerObject,windowType,lbType);

	mUIWindows.insert(lbId,listBox);
	playerObject->addUIWindow(lbId);

	listBox->sendCreate();
}

//======================================================================================================================
//
// create a clone location select list box
//

void UIManager::createNewCloneSelectListBox(UICallback* callback,const int8* eventStr,const int8* caption,const int8* prompt,const BStringVector dataItems,std::vector<BuildingObject*> buildingList,PlayerObject* playerObject,uint8 lbType)
{
	uint32 lbId = _getFreeId();

	UICloneSelectListBox* listBox =  new UICloneSelectListBox(callback,lbId,eventStr,caption,prompt,dataItems,buildingList,playerObject,lbType);

	mUIWindows.insert(lbId,listBox);
	playerObject->addUIWindow(lbId);

	listBox->sendCreate();
}

//======================================================================================================================
//
// get a random window id
//

uint32 UIManager::_getFreeId()
{
	uint32 id;

	do
	{
		id = gRandom->getRand()%4294967294 + 1;
	}
	while(getUIWindow(id) != NULL);

	return(id);
}

//======================================================================================================================

void UIManager::destroyUIWindow(uint32 id,bool sendForceClose)
{
	UIWindowMap::iterator it = mUIWindows.find(id);

	if(it != mUIWindows.end())
	{
		if(sendForceClose)
		{
			sendForceCloseWindow((*it).second);
		}

		mUIWindows.erase(it);	
	}
	else
		gLogger->logMsgF("UIManager::destroyWindow: couldn't find window %u",MSG_NORMAL,id);
}

//======================================================================================================================

void UIManager::sendForceCloseWindow(UIWindow* window)
{
	PlayerObject*	player = window->getOwner();
	if(!player ||( player->getConnectionState() != PlayerConnState_Connected))
		return;

	Message*		newMessage;
	

	gMessageFactory->StartMessage();             
	gMessageFactory->addUint32(opSuiForceClosePage);  
	gMessageFactory->addUint32(window->getId());

	newMessage = gMessageFactory->EndMessage();

	
	(player->getClient())->SendChannelA(newMessage,player->getAccountId(),CR_Client,2);
}

//======================================================================================================================
void UIManager::createNewDiagnoseListBox(UICallback* callback,PlayerObject* Medic, PlayerObject* Patient)
{
	BStringVector attributesMenu;

	BString FirstName = Patient->getFirstName(); FirstName.toUpper();
	BString LastName = Patient->getLastName(); LastName.toUpper();

	int8 title[64];
	sprintf(title,"PATIENT %s %s'S WOUNDS",FirstName.getAnsi(), LastName.getAnsi());

	int8 desc[512];
	sprintf(desc, "Below is a listing of the Wound and Battle Fatigue levels of %s %s. Wounds are healed through /tendWound or use of Wound Medpacks. High levels of Battle Fatigue can inhibit the healing process, and Battle Fatigue can only be healed by the patient choosing to watch performing entertainers",Patient->getFirstName().getAnsi(), Patient->getLastName().getAnsi());

	int8 Health[32];
	sprintf(Health,"Health -- %i",Patient->getHam()->mHealth.getWounds());
	attributesMenu.push_back(Health);

	int8 Strength[32];
	sprintf(Strength,"Strength -- %i",Patient->getHam()->mStrength.getWounds());
	attributesMenu.push_back(Strength);
	
	int8 Constitution[32];
	sprintf(Constitution,"Constitution -- %i",Patient->getHam()->mConstitution.getWounds());
	attributesMenu.push_back(Constitution);
	
	int8 Action[32];
	sprintf(Action,"Action -- %i",Patient->getHam()->mAction.getWounds());
	attributesMenu.push_back(Action);

	int8 Quickness[32];
	sprintf(Quickness,"Quickness -- %i",Patient->getHam()->mQuickness.getWounds());
	attributesMenu.push_back(Quickness);

	int8 Stamina[32];
	sprintf(Stamina,"Stamina -- %i",Patient->getHam()->mStamina.getWounds());
	attributesMenu.push_back(Stamina);

	int8 Mind[32];
	sprintf(Mind,"Mind -- %i",Patient->getHam()->mMind.getWounds());
	attributesMenu.push_back(Mind);

	int8 Focus[32];
	sprintf(Focus,"Focus -- %i",Patient->getHam()->mFocus.getWounds());
	attributesMenu.push_back(Focus);

	int8 Willpower[32];
	sprintf(Willpower,"Willpower -- %i",Patient->getHam()->mWillpower.getWounds());
	attributesMenu.push_back(Willpower);

	int8 BattleFatigue[32];
	sprintf(BattleFatigue,"Battle Fatigue -- %i",Patient->getHam()->getBattleFatigue());
	attributesMenu.push_back(BattleFatigue);

	createNewListBox(callback,"handleDiagnoseMenu",title, desc, attributesMenu, Medic, SUI_Window_ListBox);
}

void UIManager::createNewStructureDestroyBox(UICallback* callback,PlayerObject* player, PlayerStructure* structure, bool redeed)
{
	BStringVector attributesMenu;

	string text = "You have elected to destroy a structure. Petinent structure data can be found in the list below. Please complete the following steps to confirm structure deletion.\xa\xa";
			text <<"If you wish to redeed your structure, all structure data must be GREEN To continue with structure deletion, click YES. Otherwise, please click NO.\xa";
			
	if(structure->canRedeed())
	{
		text <<"WILL REDEED: \\#006400 YES \\#FFFFFF";			

		int8 redeedText[32];
		sprintf(redeedText,"CAN REDEED: \\#006400 YES\\#FFFFFF");
		attributesMenu.push_back(redeedText);
	}
	else
	{
		text <<"WILL REDEED: \\#FF0000 NO \\#FFFFFF";			

		int8 redeedText[32];
		sprintf(redeedText,"CAN REDEED: \\#FF0000 NO\\#FFFFFF");
		attributesMenu.push_back(redeedText);
	}
			
		

	int8 condition[64];
	sprintf(condition,"-CONDITION:%u/%u",1000,1000);
	attributesMenu.push_back(condition);

	int8 maintenance[128];
	sprintf(maintenance,"-MAINTENANCE:%u/%u",-1,-1);
	attributesMenu.push_back(maintenance);

	string name = structure->getCustomName();			
	name.convert(BSTRType_ANSI);
	
	createNewListBox(callback,"handleStructure Destroy",name.getAnsi(), text.getAnsi(), attributesMenu, player, SUI_Window_Structure_Delete,SUI_LB_OKCANCEL);
}


