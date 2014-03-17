#include <irrlicht/irrlicht.h>

class EventHandler : public irr::IEventReceiver
{
	public:

		EventHandler();
		virtual bool OnEvent(const irr::Sevent &event);
		//getPositionSouris();
};
