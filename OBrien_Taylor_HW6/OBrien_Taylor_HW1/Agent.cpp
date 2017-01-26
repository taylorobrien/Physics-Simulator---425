#include "Agent.h"

Agent::Agent(Ogre::SceneManager* SceneManager, std::string name, std::string filename, float height, float scale, int id)
{
	using namespace Ogre;

	mSceneMgr = SceneManager; // keep a pointer to where this agent will be

	if (mSceneMgr == NULL)
	{
		std::cout << "ERROR: No valid scene manager in Agent constructor" << std::endl;
		return;
	}
	this->id = id;
	this->height = height;
	this->scale = scale;
	this->rotateUp = 1.2;
	this->numberOfFish = 0;
	this->score = 0;
	this->mass = 8;
	this->canScore = false;
	this->currentScale = 1;
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); // create a new scene node
	mBodyEntity = mSceneMgr->createEntity(name, filename); // load the model
	mBodyNode->attachObject(mBodyEntity);	// attach the model to the scene node
	
	this->ent = mSceneMgr->createEntity("barrel.mesh");
	this->node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-10, 8, -60.0));
	this->entCube = mSceneMgr->createEntity("cube.mesh");
	this->nodeCube = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-10, 14, -60.0));
	nodeCube->attachObject(entCube);

	node->attachObject(ent);
	Ogre::AxisAlignedBox cubeBox = this->entCube->getWorldBoundingBox();
	nodeCube->setVisible(false);
	//cubeBox.intersects(cubeBox);
	//node->roll(Ogre::Degree(90));
	//node->pitch(Ogre::Degree(90));
	nodeCube->setScale(0.1, 0.005, 0.1);
	node->setScale(2.0, 2.0, 2.0);

	ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout
	ps = mSceneMgr->createParticleSystem("Fountain1", "Examples/PurpleFountain");
	Ogre::SceneNode* mnode = node->createChildSceneNode();
	mnode->yaw(Degree(90));
	mnode->attachObject(ps);
	ps->setVisible(false);
	//ps->removeAllEmitters();
	//ps->getEmitter(0)->mWidth = 10;
	//ps->setDefaultWidth(10);
	//mBodyNode->translate(0,0, 25); //4.5,0); // make the Ogre stand on the plane (almost)
	mBodyNode->yaw(Ogre::Degree(-90));
	//setupAnimations();  // load the animation for this character

	// configure walking parameters
	mWalkSpeed = 70.0f;	
	mDirection = Ogre::Vector3::ZERO;
	mDistance = 0;
	//mDestination = Ogre::Vector3::ZERO;
	//mBodyEntity = 0;
	//mBodyNode = 0;
	projectile = false;
	mDestination = Ogre::Vector3::ZERO;
	// create random points to walk to 
//	float x, y;
	
 
	
 /*
	ent = mSceneMgr->createEntity("knot.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		 Ogre::Vector3(550.0, -10.0, 50.0));
	node->attachObject(ent);
	node->setScale(0.1, 0.1, 0.1);
 
	ent = mSceneMgr->createEntity("knot.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		Ogre::Vector3(-100.0, -10.0,-200.0));
	node->attachObject(ent);
	node->setScale(0.1, 0.1, 0.1);
	*/
	mBodyNode->roll(Ogre::Degree(-20));
	this->fishAngle = 20;
}

Agent::~Agent(){
	// mSceneMgr->destroySceneNode(mBodyNode); // Note that OGRE does not recommend doing this. It prefers to use clear scene
	// mSceneMgr->destroyEntity(mBodyEntity);
}

// update is called at every frame from GameApplication::addTime
void Agent::update(Ogre::Real deltaTime, std::list<Agent*> agentList) 
{
	//this->updateAnimations(deltaTime);	// Update animation playback
	//this->updateLocomote(deltaTime, agentList);	// Update Locomotion
	if (projectile){ // Lecture 10
		shoot(deltaTime);
	}
	else{
		this->updateLocomote(deltaTime, agentList);
	}

}


void Agent::setupAnimations()
{
	this->mTimer = 0;	// Start from the beginning
	this->mVerticalVelocity = 0;	// Not jumping

	// this is very important due to the nature of the exported animations
	mBodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	// Name of the animations for this character
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	/*for (int i = 0; i < 13; i++)
	{
		mAnims[i] = mBodyEntity->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}*/

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);
	//setBaseAnimation(ANIM_RUN_BASE);
	//setTopAnimation(ANIM_RUN_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
}

void Agent::setPosition(float x, float y, float z){
	this->mBodyNode->setPosition(x, y, z);//+height, z);
}

Ogre::Vector3 Agent::getPosition(){
	return this->mBodyNode->getPosition();
}

void Agent::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < 13)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id; 

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}
	
void Agent::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < 13)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

int Agent::getBaseAnimation(){
	return mBaseAnimID;
}

void Agent::updateAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime; // how much time has passed since the last update 
	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void Agent::setGrid(Grid* grid){
	this->grid = grid;
}

void Agent::AStarWalk(GridNode* startPos, GridNode* currentPos, GridNode* destination){
	bool isfound;
	if(!destination->isClear()){
		std::cout << "Space not empty, return"  << std::endl;
		return;
	}
	if(startPos == destination){
		return; //Character does not need to move
	}
	if(closed.size() >= 600){
		closed.clear();
		open.clear();
		/*int ranrow = rand() % grid->getRows();
		int rancol = rand() % grid->getCols();		
		Ogre::Vector3 currentPlace = mBodyNode->getPosition();
		int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
		int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
		if(grid->getNode(r, c) == NULL){
			std::cout << "ERROR" << std::endl;
		}
		AStarWalk(grid->getNode(r, c), grid->getNode(r, c), grid->getNode(ranrow, rancol));
		*/
		return;
	}
	//for the EastNode
	if(startPos == currentPos){
		currentPos->GValue = 0;
		currentPos->HValue = grid->getDistance(currentPos, destination);
		currentPos->FValue = currentPos->GValue + currentPos->HValue;

	}
	else{
		//if((std::find(open.begin(), open.end(), currentPos) != open.end())){
			open.remove(currentPos);
		//}
	}
	if(currentPos == NULL){
		std::cout <<"ERR"<<std::endl;
	}
	temp = grid->getEastNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getNENode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getNorthNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }

	temp = grid->getNWNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }

	temp = grid->getSENode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getSouthNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getSWNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(14 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 14 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 14 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	temp = grid->getWestNode(currentPos);
	if(temp != NULL && temp->isClear()){ //if the null exists and its not blocked
		isfound = (std::find(closed.begin(), closed.end(), temp) != closed.end());  
		if (!isfound){   //make sure its not in the closed list
			//need to check if its in the open list
			isfound = (std::find(open.begin(), open.end(), temp) != open.end()); 
			//if its on the open list, we need to compute a new G value, 
			//from parent and then see if that G value is lower then current.
			//if the G value is lower, reset the parent of the node & G value
			if(isfound){
				if(10 + temp->parent->GValue < temp->GValue){
					temp->parent = currentPos;
					temp->GValue = 10 + temp->parent->GValue;//FValue = 15 + grid->getDistance(temp, destination);
					//temp->HValue = grid->getDistance(temp, destination); 
					temp->FValue = temp->GValue + temp->HValue;	
				}
			}
			//if its not on the open list, set the parent to the current, compute G value
			//for this node using current node, add it to the open list
			else{
				temp->parent = currentPos;
				temp->GValue = 10 + temp->parent->GValue;
				temp->HValue = grid->getDistance(temp, destination); 
				temp->FValue = temp->GValue + temp->HValue;
				if(temp == NULL || currentPos == NULL){
					std::cout << "BREAK" << std::endl;
				}
				open.push_back(temp);
			}  }  }
	if(currentPos){ if(!(std::find(closed.begin(), closed.end(), currentPos) != closed.end())){
		closed.push_front(currentPos); }}
	else{
		std::cout << "CurrentPos = ERROR" << std::endl;
		//currentPos = mBodyNode->getPosition();

		Ogre::Vector3 currentPlace = mBodyNode->getPosition();
		int r = (currentPlace[2] - (NODESIZE/2) + ((grid->getRows() * NODESIZE)/2))/NODESIZE;
		int c = (currentPlace[0] - (NODESIZE/2) + ((grid->getCols() * NODESIZE)/2))/NODESIZE;
				//GridNode* start = grid->getNode(r, c);
				//if(temp){
		currentPos = grid->getNode(r, c);
		temp = currentPos;
		//closed.push_front(currentPos);
	}
	//temp = NULL;
	lowestF = 1000;
	std::list<GridNode*>::iterator iter;
		for (iter = open.begin(); iter != open.end(); iter++){
			
			if((*iter) == destination){
				walkBackwards(startPos, destination);
				return;
			}
			if((*iter)->FValue < lowestF){
				temp = (*iter);
				lowestF = (*iter)->FValue;
			}
		}
		if(temp == NULL){ 
			temp = startPos;
			closed.clear();
			return;
			
		}
		AStarWalk(startPos, temp, destination);

}

void Agent::walkBackwards(GridNode* start, GridNode* Destination){
	temp = Destination;
	start->setContains('S');
	
	int counter = 0;
	while(temp->parent != start){
		if(temp == NULL){
			std::cout << "ERROR" << std::endl;
			break;
		}
			std::cout << "walkBackwards1" << std::endl;
			std::cout << "Row: " << temp->getRow() << " Col: " << temp->getColumn()<< std::endl;
			path.push_front(temp);
			temp = temp->parent;
			
	}
	path.push_front(temp);
	std::cout << "walkBackwards2" << std::endl;
	std::cout << "Row: " << temp->getRow() << " Col: " << temp->getColumn()<< std::endl;
	std::list<GridNode*>::iterator iter;
	for (iter = path.begin(); iter != path.end(); iter++){
		walkToNode((*iter));
		(*iter)->setContains('0' + counter);
		counter++;
		(*iter) = NULL;
	}
	Destination->setContains('E');
	grid->printToFile();
	std::list<GridNode*>::iterator iterOPEN;
	for (iterOPEN = open.begin(); iterOPEN != open.end(); iterOPEN++){
		//(*iterOPEN)->parent = NULL;
		(*iterOPEN) = NULL;//->parent = NULL;
	}
	open.clear();		
	std::list<GridNode*>::iterator iterCLOSED;
	for (iterCLOSED = closed.begin(); iterCLOSED != closed.end(); iterCLOSED++){
		//(*iterCLOSED)->parent = NULL;
		(*iterCLOSED) = NULL;//->parent = NULL;
	}
	closed.clear();
	path.clear();

}

void Agent::walkToNode(GridNode* node){
	if(node){
		//if the node exists, get the row and column of it
		int row = node->getRow();
		std::cout << "WalktoNode" << std::endl;

		int col = node->getColumn();
		//if the row and column arent NULL
		if(row != NULL && col != NULL){
			//get the X, Y, and Z position of it (so we can tell the agent to go there)
			float z = (row * NODESIZE) - (grid->getRows() * NODESIZE)/2.0 + NODESIZE/2.0; 
			float y = 5; 
			float x = (col * NODESIZE) - (grid->getCols() * NODESIZE)/2.0 + NODESIZE/2.0; 
			//add the location to the walkList of the agent
			mWalkList.push_back(Ogre::Vector3(x, y, z));
			//This is for error checking
			if(mBodyNode->getPosition().y < 0){
				std::cout << "break" << std::endl;
			}
		}
	}
	else{
		//also used for error checking

		std::cout << "error" << std::endl;
	}
}

void Agent::fadeAnimations(Ogre::Real deltaTime)
{
	using namespace Ogre;

	for (int i = 0; i < 13; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * 7.5f; //ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

bool Agent::nextLocation()
{
	if(mWalkList.empty()){
		return false;
	}
	mDestination = mWalkList.front();
	mWalkList.pop_front();
	this->mDirection = mDestination - mBodyNode->getPosition();
	mDistance = mDirection.normalise();
	//mDirection = mDestination - mBodyNode->getPosition();
	return true;
}

Ogre::Vector3 Agent::getFlockVector(std::list<Agent*> agentList){
	std::list<Agent*>::iterator iter;
	Ogre::Vector3 tempDistance = Ogre::Vector3::ZERO;
	Ogre::Vector3 totalSeparate = Ogre::Vector3::ZERO;
	//Ogre::Vector3 tempVelocity;
	Ogre::Vector3 totalAlign = Ogre::Vector3::ZERO;
	Ogre::Vector3 totalCohesion = Ogre::Vector3::ZERO;

	float KSeparate = 0.2;
	float KAlign = 0.3;
	float KCohesion = 0.2;
	int numberInNeighborhood = 0;


	for (iter = agentList.begin(); iter != agentList.end(); iter++){
		//if the agent we're checking isnt the same as the current one
		if(getPosition() != (*iter)->getPosition() && id != (*iter)->id){
			//get the distance between the two
			tempDistance = (*iter)->getPosition() - getPosition();
			//if the other agent is in the neighborhood
			if(abs(tempDistance[0]) <= 50 && abs(tempDistance[1]) <= 50 && abs(tempDistance[2]) <= 50){
				//
				if(abs(tempDistance[0]) < 5 && abs(tempDistance[2]) < 5){
					//This doesnt seem to work. I tried to translate the ogres, 
					//change the values, but nothing seems to be the perfect "fix"
					totalSeparate -= (2*tempDistance);
					//mBodyNode->translate((id*-1)*(1/id), 0, (id*-1)*(1/id));
					//KSeparate = 0.8;
					//KAlign = .01;
					//KCohesion = .01;
					
				}
				//if they're close, push them away
				else if(abs(tempDistance[0]) < 15 && abs(tempDistance[2]) < 15){
					totalSeparate -= tempDistance;
					//if(!someoneAtGoal(agentList)){
					//	totalSeparate -= tempDistance; //.normalise();
					//}
					//else{
						//totalSeparate -= tempDistance;
					//}
				}
				if(someoneAtGoal(agentList)){
					totalSeparate -= tempDistance;
				}
				Ogre::Vector3 temp =  (*iter)->mDirection;
				Ogre::Vector3 temptwo = (*iter)-> mDirection * (*iter)->mWalkSpeed;
				totalAlign += temptwo;//(*iter)->mWalkSpeed;
				totalCohesion += (*iter)->getPosition();
				//using this numberInNeighborhood to prevent div by 0
				numberInNeighborhood++;
			}
		}	
	}
		
		Ogre::Vector3 newVector;
		if(numberInNeighborhood != 0){
			totalSeparate = KSeparate * totalSeparate; 
			totalAlign = KAlign * (totalAlign /numberInNeighborhood);
			totalCohesion = KCohesion * (totalCohesion/numberInNeighborhood - getPosition());
		}
		else{
			newVector = mDirection;
		}
		newVector =  totalCohesion + totalAlign + totalSeparate; //totalSeparate + totalAlign;
		//newVector[1] = 5;
		return newVector;

}
//Testing to see if someone is at the goal and whether the agents need to stop or continue until closer.
bool Agent::someoneAtGoal(std::list<Agent*> agentList){
	std::list<Agent*>::iterator iter;
	bool stopAgents = false;
	if(mDistance < 1.0){
		stopAgents = true;
	}
	if(stopAgents){
		for (iter = agentList.begin(); iter != agentList.end(); iter++){
			if(abs((*iter)->mDistance) < 15.0 && (*iter)->isNeighborStopped(agentList, (*iter))){
			//if((*iter)->isNeighborStopped(agentList)){
				(*iter)->mDistance = 0;	
			}
			else{
				//(*iter)->mDistance -= 15;
			}
			//}
		}
		return true;
	}
	

	return false;
}
//this method is used to see if the current agent needs to stop (if their neighbor is stopped)
//setting the mDistance to 0 causes the ogre to stop.
bool Agent::isNeighborStopped(std::list<Agent*> agentList, Agent* tempAgent){
	std::list<Agent*>::iterator iter;
	Ogre::Vector3 tempVec;
	for (iter = agentList.begin(); iter != agentList.end(); iter++){

		if(tempAgent->getPosition() !=(*iter)->getPosition() && tempAgent->id != (*iter)->id){
			if(abs(tempAgent->getPosition()[0] - (*iter)->getPosition()[0]) < 8 && abs(tempAgent->getPosition()[2] - (*iter)->getPosition()[2]) < 8 && (*iter)->mDistance < 1){
				tempVec = (*iter)->getPosition();
				tempAgent->mDistance = 0;
				return true;
			}
		}
	}
	return false;
}

void Agent::updateLocomote(Ogre::Real deltaTime, std::list<Agent*> agentList)
{
	//std::cout << "Location: " << mBodyNode->getPosition() << std::endl;
	//std::cout << "Direction: " << mDirection << std::endl;
	//std::cout << "Destination: " << mDestination << std::endl;
	Ogre::String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
		"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};
	if (mDirection == Ogre::Vector3::ZERO) 
	{
		if(nextLocation()){
			/*			
			mAnims[ANIM_RUN_TOP]->setEnabled(true);
			mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
			mAnims[ANIM_RUN_BASE]->setEnabled(true);
			mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
			mAnims[ANIM_IDLE_BASE]->setEnabled(false);
			mAnims[ANIM_IDLE_TOP]->setEnabled(false);
			*/
			Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
			if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
			{
			  mBodyNode->yaw(Ogre::Degree(180));
			}
			else
			{
			// mBodyNode->setOrientation(mBodyNode->getOrientation()[0], mBodyNode->getOrientation()[1], 15, mBodyNode->getOrientation()[3]);

			 Ogre::Quaternion quat = src.getRotationTo(mDirection);
			//quat[2] = 5;
			 
			 mBodyNode->rotate(quat);
			 //mBodyNode->setOrientation(mBodyNode->getOrientation()[0], mBodyNode->getOrientation()[1], 15, mBodyNode->getOrientation()[3]);
				 //mBodyNode->yaw(Ogre::Degree(90));
			}
			
		}
	}
	else{
		//mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
		//mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
		Ogre::Real move = mWalkSpeed * deltaTime;
		mDistance -= move;
		
		if(mDistance <= 0){
			//mBodyNode->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;
			if(nextLocation()){
				
				//rotation code
				Ogre::Vector3 src = mBodyNode->getOrientation() * Ogre::Vector3::UNIT_Z;
				if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
				{
				  mBodyNode->yaw(Ogre::Degree(180));
				}
				else
				{
				 Ogre::Quaternion quat = src.getRotationTo(mDirection);
				 mBodyNode->rotate(quat);
				 //mBodyNode->lookAt(Ogre::Vector3(mDestination.x, mDestination.y, mDestination.z + 100), Node::TS
				 //mBodyNode->yaw(Ogre::Degree(90));
				}
			}
			else{
				//mAnims[ANIM_IDLE_BASE]->setEnabled(true);
				//mAnims[ANIM_IDLE_TOP]->setEnabled(true);
				//mAnims[ANIM_RUN_TOP]->setEnabled(false);
				////mAnims[ANIM_RUN_TOP]->setTimePosition(mTimer);
				//mAnims[ANIM_RUN_BASE]->setEnabled(false);
				////mAnims[ANIM_RUN_BASE]->setTimePosition(mTimer);
				
			}
		}
		else{
			//Ogre::Vector3 newDirection = getFlockVector(agentList);
			//newDirection[1] = 0;
			//newDirection.normalise();
			
			mBodyNode->translate(move * mDirection);//(newDirection));
	
		}

	}
		

}

int Agent::throwFish(){

	numberOfFish++;
	return numberOfFish;
}
void Agent::angleDown(){
	mBodyNode->roll(Ogre::Degree(10));
	this->fishAngle -= 10;
	rotateUp -= 0.1;

}
void Agent::angleUp(){
	mBodyNode->roll(Ogre::Degree(-10));
	this->fishAngle += 10;
	rotateUp += 0.1;

}
void Agent::increaseVelocity(){
	mWalkSpeed++;
	std::cout << "WalkSpeed: " << mWalkSpeed << std::endl;
}
void Agent::decreaseVelocity(){
	mWalkSpeed--;
	std::cout << "WalkSpeed: " << mWalkSpeed << std::endl;
}

void Agent::fire() // lecture 10
{
	if(!projectile){

	projectile = true; // turns on the movement
//	this->setBaseAnimation(ANIM_NOD);
	// set up the initial state
	initPos = this->mBodyNode->getPosition();
	vel.x = 0;
	this->numberOfFish++;
	if(rotateUp <= .9){
		vel.y = mWalkSpeed * rotateUp;
		vel.z = (-mWalkSpeed / rotateUp) * 0.5;
	}
	else if(rotateUp != 1){
		vel.y = mWalkSpeed * rotateUp;
		vel.z = -mWalkSpeed / rotateUp;
	}
	else{
		vel.y = mWalkSpeed;
		vel.z = -mWalkSpeed;
	}
	
	gravity.x = 0;
	gravity.y = -9.81;
	gravity.z = 0;
	
	//this->mBodyNode->yaw(Ogre::Degree(180));
	//this->mBodyNode->pitch(Ogre::Degree(45));
	//this->mBodyNode->showBoundingBox(true); 
	this->canScore = true;
	}
}

void Agent::restartGame(){
	this->numberOfFish = 0;
	this->score = 0;

	std::cout << "In Restart Game" << std::endl;
}

void Agent::shoot(Ogre::Real deltaTime) // lecture 10 call for every frame of the animation
{
	using namespace Ogre;

	Vector3 pos = this->mBodyNode->getPosition();

	// Calculate projectile motion here. What is vel? and What is new pos?
	//mass = 5;
	vel += deltaTime * gravity *mass; // 5 is the mass
	pos += 0.5*vel*deltaTime;

	this->mBodyNode->setPosition(pos);


	Ogre::AxisAlignedBox objBox = this->mBodyEntity->getWorldBoundingBox();
	//objBox.intersects(objBox); 
	Ogre::AxisAlignedBox CubeBox = this->entCube->getWorldBoundingBox();
	if(objBox.intersects(CubeBox) && canScore){
		this->score++;
		this->canScore = false;
		ps->setVisible(true);
		//PlaySound("DingSound.wave", NULL, SND_FILENAME);
	}
	else if(objBox.intersects(CubeBox)){
		//ps->setVisible(true);
	}
	//objBox.intersects(


	if (this->mBodyNode->getPosition().y <= 2) // if it get close to the ground, stop
	{
		this->mBodyNode->scale(4.0/mass, 4.0/mass, 4.0/mass);
		// when finished reset
		projectile = false;
		//setBaseAnimation(ANIM_WAVE);
		ps->setVisible(false);
		//this->mBodyNode->pitch(Ogre::Degree(-45));
		//this->mBodyNode->yaw(Ogre::Degree(180));
		this->mBodyNode->setPosition(initPos);
		mass = rand() % 10 + 5;
		currentScale = (mass/4.0);
		this->mBodyNode->scale(currentScale, currentScale, currentScale);
		

	}
}


