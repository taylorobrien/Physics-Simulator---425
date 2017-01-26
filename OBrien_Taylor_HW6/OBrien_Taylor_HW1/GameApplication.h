#ifndef __GameApplication_h_
#define __GameApplication_h_

#include "BaseApplication.h"
#include "Agent.h"
#include "Grid.h"
#include "Barrel.h"

class Grid;
class Agent;
class Barrel;

class GameApplication : public BaseApplication
{
private:
	Agent* agent; // store a pointer to the character
	Barrel* barrel; 
	
public:
	void buttonHit(OgreBites::Button* b);
    GameApplication(void);
	std::list<Agent*> agentList; // Lecture 5: now a list of agents
	std::list<Barrel*> barrelList;
    virtual ~GameApplication(void);
	Grid* grid;
	void loadEnv(std::string level, std::string name1, std::string name2);			// Load the buildings or ground plane, etc.
	void setupEnv();		// Set up the lights, shadows, etc
	void loadObjects();		// Load other props or objects (e.g. furniture)
	void loadCharacters();	// Load actors, agents, characters
	//void createGUI();
	void addTime(Ogre::Real deltaTime);		// update the game state

	//////////////////////////////////////////////////////////////////////////
	// Lecture 4: keyboard interaction
	// moved from base application
	// OIS::KeyListener
    bool keyPressed( const OIS::KeyEvent &arg );
    bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    bool mouseMoved( const OIS::MouseEvent &arg );
    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	////////////////////////////////////////////////////////////////////////////


protected:
    virtual void createScene(void);
};

#endif // #ifndef __TutorialApplication_h_
