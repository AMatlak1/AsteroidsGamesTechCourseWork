#include "AlienSpaceShip.h"
#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <iostream>
#include <iomanip>
#include <fstream>

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mAlienSpaceShipCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	Animation *enemy_anim = AnimationManager::GetInstance().CreateAnimationFromFile("enemy", 128, 8192, 128, 128, "enemy_fs.png");

	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);

	CreateAlienSpaceShip(1);

	//Reads High Score Numbers From File
	ReadHighScoreTableFromFile();

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);


	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

//Reads High Score From the HigherScoreTable.txt File
void Asteroids::ReadHighScoreTableFromFile()
{
	int scoreFromFile;
	ifstream ifs;
	ifs.open("HighScoreTable.txt");
	if (!ifs) {
		//If File fails to open the text below is displayed in terminal
		cout << " Failed to open" << endl;
	} else {
		//If File opens the text below is displayed in terminal
		cout << "Opened OK" << endl;

		//Reads the score in order
		ifs >> scoreFromFile;
		mHighScoreTopFromFile = scoreFromFile;
		ifs >> scoreFromFile;
		mHighScoreMidFromFile = scoreFromFile;
		ifs >> scoreFromFile;
		mHighScoreBottomFromFile = scoreFromFile;
	}

	ifs.close();
}

//Saves High Score Table information to the File
void Asteroids::SaveHighScoreTableToFile()
{
	ofstream fout;
	fout.open("HighScoreTable.txt");
	fout << mHighScoreTopFromFile << endl;
	fout << mHighScoreMidFromFile << endl;
	fout << mHighScoreBottomFromFile << endl;	
	fout.close();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		mAlienSpaceShip->Shoot();
		mSpaceship->Shoot();
		mAlienSpaceShip->Shoot();
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		//Creates 2 smaller Asteroids when one big asteroid is hit by bullet
		CreateSplitAsteroids(2, object->GetPosition());
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		//Displays HighScoreTable when User is out of Lives and its Game over
		mGameOverLabel->SetVisible(true);
		mHighScoreLabel->SetVisible(true);
		mHighScoreLabel1->SetVisible(true);
		mHighScoreLabel2->SetVisible(true);
		mHighScoreLabel3->SetVisible(true);

		if (mHighScoreTopFromFile < mPlayerScore) {
			std::ostringstream h_msg_stream;
			h_msg_stream << "1: Your Score: " << mPlayerScore;
			mHighScoreBottomFromFile = mHighScoreMidFromFile;
			mHighScoreMidFromFile = mHighScoreTopFromFile;			
			mHighScoreTopFromFile = mPlayerScore;
			std::string h_score_msg = h_msg_stream.str();
			mHighScoreLabel1->SetText(h_score_msg);
			RefreshLabel(mHighScoreLabel2, "2: Score: " + std::to_string(mHighScoreMidFromFile));
			RefreshLabel(mHighScoreLabel3, "3: Score: " + std::to_string(mHighScoreBottomFromFile));			
		}
		else if (mHighScoreMidFromFile < mPlayerScore) {
			std::ostringstream h_msg_stream;
			h_msg_stream << "2: Your Score: " << mPlayerScore;
			mHighScoreBottomFromFile = mHighScoreMidFromFile;
			mHighScoreMidFromFile = mPlayerScore;
			std::string h_score_msg = h_msg_stream.str();
			mHighScoreLabel2->SetText(h_score_msg);
			RefreshLabel(mHighScoreLabel3, "3: Score: " + std::to_string(mHighScoreBottomFromFile));
		}
		else if (mHighScoreBottomFromFile < mPlayerScore) {
			std::ostringstream h_msg_stream;
			h_msg_stream << "3: Your Score: " << mPlayerScore;
			mHighScoreBottomFromFile = mPlayerScore;
			std::string h_score_msg = h_msg_stream.str();
			mHighScoreLabel3->SetText(h_score_msg);
		}

		SaveHighScoreTableToFile();
	}
}

//Updates the Label Order depending on if the User has beaten a score
void Asteroids::RefreshLabel(shared_ptr<GUILabel> guiLabel, string value) {
	std::ostringstream h_msg_stream;
	h_msg_stream << value;
	std::string h_score_msg = h_msg_stream.str();
	guiLabel->SetText(h_score_msg);
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

//Creates the orignal asteroids which populate the game world 
void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

//Creates the Alien space ship and also sets the bullet
void Asteroids::CreateAlienSpaceShip(const uint num_alienspaceships)
{
	mAlienSpaceShipCount = num_alienspaceships;
	for (uint i = 0; i < num_alienspaceships; i++)
	{
		mAlienSpaceShip = make_shared<AlienSpaceShip>();
		mAlienSpaceShip->SetBoundingShape(make_shared<BoundingSphere>(mAlienSpaceShip->GetThisPtr(), 4.0f));
		shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
		mAlienSpaceShip->SetBulletShape(bullet_shape);
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("enemy");
		shared_ptr<Sprite> enemy_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);		
		enemy_sprite->SetLoopAnimation(true);		
		mAlienSpaceShip->SetBoundingShape(make_shared<BoundingSphere>(mAlienSpaceShip->GetThisPtr(), 10.0f));
		mAlienSpaceShip->SetSprite(enemy_sprite);
		mAlienSpaceShip->SetScale(0.2f);
		mGameWorld->AddObject(mAlienSpaceShip);		
	}
	
}


//Creates Smaller Asteroids by half the size
void Asteroids::CreateSplitAsteroids(const uint num_asteroids, GLVector3f p)
{
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid2 = make_shared<Asteroid>();
		asteroid2->SetBoundingShape(make_shared<BoundingSphere>(asteroid2->GetThisPtr(), 5.0f));
		asteroid2->SetSprite(asteroid_sprite);
		asteroid2->SetScale(0.1f);
		asteroid2->SetPosition(p);
		mGameWorld->AddObject(asteroid2);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_LEFT);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);

	
	mHighScoreLabel = shared_ptr<GUILabel>(new GUILabel("High Score"));
	mHighScoreLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_LEFT);
	mHighScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoreLabel->SetVisible(false);

	mHighScoreLabel1 = shared_ptr<GUILabel>(new GUILabel("1: Score: " + std::to_string(mHighScoreTopFromFile)));
	mHighScoreLabel1->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_LEFT);
	mHighScoreLabel1->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoreLabel1->SetVisible(false);

	mHighScoreLabel2 = shared_ptr<GUILabel>(new GUILabel("2: Score: " + std::to_string(mHighScoreMidFromFile)));
	mHighScoreLabel2->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_LEFT);
	mHighScoreLabel2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoreLabel2->SetVisible(false);

	mHighScoreLabel3 = shared_ptr<GUILabel>(new GUILabel("3: Score: " + std::to_string(mHighScoreBottomFromFile)));
	mHighScoreLabel3->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_LEFT);
	mHighScoreLabel3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoreLabel3->SetVisible(false);


	// Add the GUILabel to the GUIContainer  	
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.4f, 0.5f));

	//Adds the GUILabels for each of the HighScoreLabels
	shared_ptr<GUIComponent> high_score_component
		= static_pointer_cast<GUIComponent>(mHighScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(high_score_component, GLVector2f(0.4f, 0.8f));
	shared_ptr<GUIComponent> high_score_component1
		= static_pointer_cast<GUIComponent>(mHighScoreLabel1);
	mGameDisplay->GetContainer()->AddComponent(high_score_component1, GLVector2f(0.4f, 0.75f));
	shared_ptr<GUIComponent> high_score_component2
		= static_pointer_cast<GUIComponent>(mHighScoreLabel2);
	mGameDisplay->GetContainer()->AddComponent(high_score_component2, GLVector2f(0.4f, 0.7f));
	shared_ptr<GUIComponent> high_score_component3
		= static_pointer_cast<GUIComponent>(mHighScoreLabel3);
	mGameDisplay->GetContainer()->AddComponent(high_score_component3, GLVector2f(0.4f, 0.65f));

}

void Asteroids::OnScoreChanged(int score)
{
	//to keep track
	mPlayerScore = score;
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




