/*
 * @Author: Raul Cisneros
 * @Date:   11/02/21
 */
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

// hello
typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec3f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
	glm::mat4 BaseObject::getMatrix(ofVec3f pos);
	void setMatrix(glm::mat4 tMatrixs);
	glm::mat4 tMatrices;
	glm::vec3 heading();
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	void setVelocity(ofVec3f newVelocity);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;  
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem  {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	bool checkCollision(ofVec3f point, float dist);
	vector<Sprite> sprites;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter: public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(ofVec3f);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void update();
	void updateEnemy();
	void removee();
	void activateShootOnce(bool switch1);
	

	SpriteSystem *sys;
	float rate;
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	float childHeight;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	ofSoundPlayer mySound;
	bool shootOnce;
	
	glm::mat4 Tmatrix;
	glm::vec3 position = glm::vec3(5.0, 5.0, 5.0);
	float rotations = 0.0;
	glm::vec3 scales = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 newheadingV;
};

/*
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		vector<Emitter *> emitters;
		int numEmitters;

		ofImage defaultImage;
		ofVec3f mouse_last;
		bool imageLoaded;

		bool bHide;

		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxVec3Slider velocity;
		ofxLabel screenSize;


		ofxPanel gui;
};
*/