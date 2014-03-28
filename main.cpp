#include "Plateau.hpp"
#include "Rendu.hpp"

using namespace irr;
using namespace std;

int main()
{

	Plateau P;
	Rendu R(&P);

	video::SColor blanc(255,255,255,255);
	while (R.getDevice()->run())
	{
		R.getDriver()->beginScene(true,true, blanc);
		R.majSphere();
		R.getSceneManager()->drawAll();
		R.getDriver()->endScene();
		P.afficher();
	}

	R.getDevice()->drop();

	return 0;
}
