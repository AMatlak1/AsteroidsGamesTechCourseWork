#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include "AlienSpaceShip.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<AlienSpaceShip> mAlienSpaceShip;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mHighScoreLabel;
	shared_ptr<GUILabel> mHighScoreLabel1;
	shared_ptr<GUILabel> mHighScoreLabel2;
	shared_ptr<GUILabel> mHighScoreLabel3;

	//High Score from the file 
	int mPlayerScore = 0;
	int mHighScoreTopFromFile = 0;
	int mHighScoreMidFromFile = 0;
	int mHighScoreBottomFromFile = 0;

	uint mLevel;
	uint mAsteroidCount;
	uint mAlienSpaceShipCount;

	//New headers have been added here also
	void ResetSpaceship();
	shared_ptr<GameObject>CreateSpaceship();
	void CreateGUI();
	void ReadHighScoreTableFromFile();
	void SaveHighScoreTableToFile();
	void RefreshLabel(shared_ptr<GUILabel> guiLabel, string value);
	void CreateAlienSpaceShip(const uint num_alienspaceships);
	void CreateAsteroids(const uint num_asteroids);
	void CreateSplitAsteroids(const uint num_asteroids, GLVector3f p);
	shared_ptr<GameObject> CreateExplosion();
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif