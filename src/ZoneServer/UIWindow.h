/*
---------------------------------------------------------------------------------------
This source file is part of swgANH (Star Wars Galaxies - A New Hope - Server Emulator)
For more information, see http://www.swganh.org


Copyright (c) 2006 - 2008 The swgANH Team

---------------------------------------------------------------------------------------
*/

#ifndef ANH_ZONESERVER_UIWINDOW_H
#define ANH_ZONESERVER_UIWINDOW_H

#include "Utils/typedefs.h"
#include "UIElement.h"

//================================================================================

class Message;
class PlayerObject;
class UICallback;

typedef std::vector<UIElement*>	Children;

//================================================================================

class UIWindow : public UIElement
{
	public:

		UIWindow(UICallback* callback,uint32 id,uint8 windowType,const string windowTypeStr,const int8* eventStr);
		virtual ~UIWindow();

		uint8			getWindowType(){ return mWindowType; }
		void			setWindowType(uint8 type){ mWindowType = type; }

		string			getWindowTypeStr(){ return mWindowTypeStr; }
		void			setWindowTypeStr(const string typeStr){ mWindowTypeStr = typeStr; }

		PlayerObject*	getOwner(){ return mOwner; }
		void			setOwner(PlayerObject* owner){ mOwner = owner; }

		uint64			getTimeOut(){ return mTimeOut; }
		void			setTimeOut(uint64 timeOut){ mTimeOut = timeOut; }

		string			getEventStr(){ return mEventStr; }
		void			setEventStr(string eventStr){ mEventStr = eventStr; }

		uint32			getChildrenPropertyCount();
		Children*		getChildren(){ return &mChildElements; }
		UIElement*		getChildById(uint32 id);
		void			addChild(UIElement* element){ mChildElements.push_back(element); }
		bool			removeChild(uint32 id);
		bool			removeChild(UIElement* element);

		virtual void	handleEvent(Message* message) = 0;
		virtual void	sendCreate() = 0;

		UICallback*		getCallback(){ return mUICallback; }
		void			setCallback(UICallback* callback){ mUICallback = callback; }

	protected:

		uint8			mWindowType;
		string			mWindowTypeStr;
		string			mEventStr;
		PlayerObject*	mOwner;
		uint64			mTimeOut;
		Children		mChildElements;
		UICallback*		mUICallback;
};

#endif


