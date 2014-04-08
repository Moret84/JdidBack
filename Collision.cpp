#include "Collision.hpp"

using namespace irr;
using namespace std;
using namespace scene;

CollisionHandler::CollisionHandler()
{

}

bool CollisionHandler::onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse &animator)
{
	cerr<<"jdidou"<<endl;
	//ISceneNode* miniSphere = animator.getTargetNode();
	//ISceneNode* sphereDestination = animator.getCollisionNode();

	return true;
}
