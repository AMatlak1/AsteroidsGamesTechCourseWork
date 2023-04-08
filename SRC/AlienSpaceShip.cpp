#include <stdlib.h>
#include "GameUtil.h"
#include "AlienSpaceShip.h"
#include "BoundingShape.h"
#include "Bullet.h"
#include "BoundingSphere.h"
#include "Spaceship.h"

AlienSpaceShip::AlienSpaceShip(void) : GameObject("Asteroid")
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

AlienSpaceShip::~AlienSpaceShip(void)
{
}

bool AlienSpaceShip::CollisionTest(shared_ptr<GameObject> o)
{
	/*
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
	
	if (o->GetType() != GameObjectType("AlienSpaceShip")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
	*/
	return false; //no collision, always visible
}

void AlienSpaceShip::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

void AlienSpaceShip::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f enemy_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	enemy_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (enemy_heading * 15);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + enemy_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
	(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}