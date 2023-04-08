#ifndef __ALIENSPACESHIP_H__
#define __ALIENSPACESHIP_H__

#include "GameObject.h"
#include "Shape.h"

class AlienSpaceShip : public GameObject
{
public:
	AlienSpaceShip(void);
	~AlienSpaceShip(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
	virtual void Shoot(void);
	void SetBulletShape(shared_ptr<Shape> bullet_shape) { mBulletShape = bullet_shape; }
public:
	shared_ptr<Shape> mBulletShape;
};

#endif
