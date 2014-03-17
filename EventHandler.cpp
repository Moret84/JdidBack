#include "EventHandler.hpp"

using namespace irr;
using namespace scene;

EventHandler::EventHandler()
{
}

bool EventHandler::OnEvent(const SEvent &event)
{
	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			return true;

		return false;
}




	

