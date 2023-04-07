#ifndef __ALIENSPACESHIP_H__
#define __ALIENSPACESHIP_H__

#include "GameObject.h"

class AlienSpaceShip : public GameObject
{
public:
	AlienSpaceShip(void);
	~AlienSpaceShip(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif

