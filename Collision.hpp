#include <irrlicht/irrlicht.h>
#include <iostream>

class CollisionHandler: public irr::scene::ICollisionCallback
{
	public:
		CollisionHandler();
		virtual bool onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse &animator);
};
