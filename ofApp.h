/*
 * @Author: Raul Cisneros
 * @Date:   11/02/21
 */
#pragma once

#include "ofMain.h"
#include "emitter.h"
#include "Particle.h"
#include "ParticleEmitter.h"


 //  Shape base class
 //
class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside() { return false; }

	glm::vec3 pos;
	float rotation = 0.0;
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

	vector<glm::vec3> verts;
	glm::mat4 T;
};

//  TriangleShape
//
class TriangleShape : public Shape {
public:
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}

	glm::vec3 newheading;
	glm::mat4 rot;
	glm::mat4 trans;

	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	
	void draw();
};

//  Example - we could define any type of shape we want
//
class LineDrawingShape : public Shape {
public:
	void draw() {};
};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void drawStartScreen();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void checkCollisions(Emitter* sampleEnemy);
	void playerHit(Emitter* sampleEnemy);
	glm::vec3 curveEval(float x, float scale, float cycles);
	glm::vec3 parabolaEval(float x, float cycles);
	glm::vec3 LinearEval(float x);
	glm::vec3 LinearEvalHigh(float x);
	void AsteroidSinPath(Emitter* enemy);
	void AsteroidParabolaPath(Emitter* enemy);
	void LinearPath(Emitter* enemy);
	void LinearPathLeft(Emitter* enemy);
	void followShip(Emitter* enemy);
	void gameOverDraw();
	void integrate();
	void teleportBounds();


	// initialize triangle - note: I changed vertices to match what I had in demo video
	TriangleShape tri = TriangleShape(glm::vec3(-20, -20, 0), glm::vec3(0, 40, 0), glm::vec3(20, -20, 0));
	glm::vec3 mouseCurrentPos;
	glm::vec3 mouseLastPos;
	glm::vec3 mousePosDifference;

	float outofBoundsWidth = 1200;
	float outofBoundsHeight = 2400;
	float outofBoundsBase = 0;

	float rotationValue = 2;


	ParticleEmitter partExplosion;
	ofVec2f explosionPos;

	int score;
	int highScore;
	int framerate = 60;

	bool inside;
	bool startscreen = true;
	bool headingVisible = false;
	ofImage image;

	Emitter* turret = NULL;
	Emitter* enemy = NULL;
	Emitter* enemy2 = NULL;
	Emitter* enemy3 = NULL;
	Emitter* enemyAsteroid = NULL;
	Emitter* enemyAsteroid2 = NULL;
	Emitter* enemyAsteroid3 = NULL;
	Emitter* star = NULL;
	
	int approachRange = 700;
	glm::vec3 approachVelocity = glm::vec3 (300, 300, 0);

	ofImage defaultImage;
	ofImage enemyImage;
	ofImage enemyLImage;
	ofImage enemyRImage;
	ofImage spriteImage;
	ofImage asteroidImage;
	ofImage starImage;
	ofSoundPlayer explosion;
	ofTrueTypeFont text;

	
	bool imageLoaded;
	bool fullscreen = false;
	bool isShooting = false;
	bool starScreen = true; //start screen
	bool shrink = true;
	bool guiMenu = false;
	bool useSpaceship = true;
	bool gameOver = false;
	
	ofxPanel gui;
	ofxFloatSlider rate;
	ofxFloatSlider life;
	ofxVec3Slider velocity;
	ofxVec3Slider shipVelocity;
	ofxLabel screenSize;
	ofxFloatSlider imageScale;
	ofxFloatSlider lazerScale;
	ofxFloatSlider scale;
	ofxFloatSlider cycles;
	ofxFloatSlider speed;
	ofxFloatSlider thrust;
	
	float speeds;
	float permaParabolaScale = 5;
	float permaCycle = 5.23;

	glm::vec3 p;
	glm::vec3 newPos;
	glm::vec3 homeOrient = glm::vec3(0, 1, 0);
	glm::vec3 home = glm::vec3(0, 0, 1);
	glm::vec3 headingSprite;

	

	ofVec3f position;
	ofVec3f velocity2;
	ofVec3f acceleration;
	float angularVelocity;
	float angularAcceleration;
	float thrust2;
	float	damping;
	float   mass;

};
