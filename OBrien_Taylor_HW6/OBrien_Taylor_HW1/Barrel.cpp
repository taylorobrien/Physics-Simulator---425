#include "Barrel.h"

Barrel::Barrel(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, int id)
{
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this agent will be

	if (mSceneMgr == NULL)
	{
		std::cout << "ERROR: No valid scene manager in Agent constructor" << std::endl;
		return;
	}
	
	this->scale = scale;
	
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node
	
}

Barrel::~Barrel(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}



void Barrel::setPosition(float x, float y, float z){
	this->mBodyNode->setPosition(x, y, z);//+height, z);
}

Ogre::Vector3 Barrel::getPosition(){
	return this->mBodyNode->getPosition();
}


void Barrel::setGrid(Grid* grid){
	this->grid = grid;
}
