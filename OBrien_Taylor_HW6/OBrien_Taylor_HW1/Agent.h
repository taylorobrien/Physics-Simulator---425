#ifndef __Agent_h_
#define __Agent_h_
#include "BaseApplication.h"
#include <deque>
#include "Grid.h"
#include <list>
#include <vector>
//#include <windows.h>
//#include <mmsystem.h>


class GameApplication;
class Grid;
class GridNode;

class Agent{
private:
	Grid* grid;
	
	Ogre::SceneManager* mSceneMgr;		// pointer to scene graph
	Ogre::SceneNode* mBodyNode;			
	Ogre::Entity* mBodyEntity;
	float height;						// height the character should be moved up
	float scale;

	// all of the animations our character has, and a null ID
	// some of these affect separate body parts and will be blended together
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

	Ogre::AnimationState* mAnims[13];		// master animation list
	AnimID mBaseAnimID;						// current base (full- or lower-body) animation
	AnimID mTopAnimID;						// current top (upper-body) animation
	bool mFadingIn[13];						// which animations are fading in
	bool mFadingOut[13];					// which animations are fading out
	Ogre::Real mTimer;						// general timer to see how long animations have been playing
	Ogre::Real mVerticalVelocity;			// for jumping
	Ogre::Vector3 getFlockVector(std::list<Agent*> agentList);
	void fadeAnimations(Ogre::Real deltaTime);				// blend from one animation to another
	void updateAnimations(Ogre::Real deltaTime);			// update the animation frame

	// for locomotion
	
	Ogre::Vector3 mDirection;				// The direction the object is moving
	Ogre::Vector3 mDestination;				// The destination the object is moving towards
	std::deque<Ogre::Vector3> mWalkList;	// The list of points we are walking to
	
	bool nextLocation();					// Is there another destination?
	void updateLocomote(Ogre::Real deltaTime, std::list<Agent*> agentList);			// update the character's walking
	
	std::list<GridNode*> path;
	bool procedural;
	GridNode* temp;
	int lowestF;
	
	bool projectile; // is this agent going to be launched?
	Ogre::Vector3 initPos; // initial position
	Ogre::Vector3 vel; // velocity of agent
	Ogre::Vector3 gravity; 
	void shoot(Ogre::Real deltaTime); // shoots the agent through the air
	Ogre::ParticleSystem* ps;
	Ogre::Entity* ent;
	Ogre::SceneNode* node;
	Ogre::Entity* entCube;
	Ogre::SceneNode* nodeCube;
	bool canScore; //this makes sure it doesnt add 1 to the score every frame when touching
	float currentScale;

public:

	Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, int id);
	~Agent();
	void setupAnimations();					// load this character's animations
	bool isNeighborStopped(std::list<Agent*> agentList, Agent* tempAgent);
	void setPosition(float x, float y, float z);
	void update(Ogre::Real deltaTime, std::list<Agent*> agentList);		// update the agent
	int getBaseAnimation();
	bool someoneAtGoal(std::list<Agent*> agentList);
	void AStarWalk(GridNode* startPos, GridNode* currentPos, GridNode* destination);
	void walkBackwards(GridNode* start, GridNode* Destination);
	std::list<GridNode*> open;
	Ogre::Real mDistance;					// The distance the agent has left to travel
	std::list<GridNode*> closed;
	Ogre::Vector3 getPosition();
	void setGrid(Grid* grid);
	int id;
	void walkToNode(GridNode* node);
	void setBaseAnimation(AnimID id, bool reset = false);	// choose animation to display
	void setTopAnimation(AnimID id, bool reset = false);
	int throwFish();
	void restartGame();
	int fishAngle;
	void angleUp();
	void angleDown();
	void increaseVelocity();
	void decreaseVelocity();
	float rotateUp;
	int mass;
	int numberOfFish;
	int score;
	//void shoot(Ogre::Real deltaTime);
	void fire();
	Ogre::Real mWalkSpeed;					// The speed at which the object is moving
};

#endif //#ifndef __Agent_h_