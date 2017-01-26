#ifndef __Barrel_h_
#define __Barrel_h_
#include "BaseApplication.h"
#include <deque>
#include "Grid.h"
#include <list>
#include <vector>

class GameApplication;
class Grid;
class GridNode;

class Barrel{
private:
	Grid* grid;
	
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			
	Ogre::Entity* mBodyEntity;
	float height;						// height the character should be moved up
	float scale;

public:
	Barrel(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, int id);
	~Barrel();
	void setPosition(float x, float y, float z);
	Ogre::Vector3 getPosition();
	void setGrid(Grid* grid);
};

#endif //#ifndef __Agent_h_