/*
 * @Author: Raul Cisneros
 * @Date:   11/02/21
 */
#include "emitter.h"

BaseObject::BaseObject() {
	trans = ofVec3f(0,0,0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}
glm::mat4 BaseObject::getMatrix(ofVec3f pos) {
	return tMatrices;
}
void BaseObject::setMatrix(glm::mat4 tMatrixs) {
	 this->tMatrices = tMatrixs;
}
glm::vec3 BaseObject::heading() {
	glm::vec3 initialHeading = glm::vec3(0, -1, 0);	// Heading at start
	glm::mat4 Mrot = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
	glm::vec3 h = Mrot * glm::vec4(initialHeading, 1);
	return glm::normalize(h);
}
//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 60;
	height = 80;

}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}
// Changes a given sprites velocity
void Sprite::setVelocity(ofVec3f newVelocity)
{
	this->velocity = newVelocity;
}

//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0  + trans.x, -height / 2.0  + trans.y);
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
	}
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].trans += sprites[i].velocity/ ofGetFrameRate();
	}
}
bool SpriteSystem::checkCollision(ofVec3f point, float dist)
{
	
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			s = tmp;

			return true;
		}
		else {
			s++;
		}
	}
	return false;

}
//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
	//	ofPushMatrix();
	//	ofMultMatrix(sprites[i].tMatrices);
		sprites[i].draw();
	//	ofPopMatrix();
	}
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;
	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	mySound.load("sounds/lazerShoot.wav");
}
//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {

	ofSetColor(ofColor::white);

	// 2nd approach - tell OpenGL to transform all points using the "current transformation matrix" (CTM);
	ofPushMatrix();
	ofMultMatrix(tMatrices);

	if (drawable) {

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
		}
		else {
			ofSetColor(0, 0, 200);
			//ofDrawRectangle(-width/2 + trans.x, -height/2 + trans.y, width, height);
			ofDrawRectangle(-width / 2 + trans.x, -height / 2 + trans.y, 100, 100);
		}
	}
	ofPopMatrix();
	// draw sprite system
	sys->draw();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {

	if (started) {
		float time = ofGetElapsedTimeMillis();
		//use this code to shoot once, not using it for now
		if (shootOnce) {
			Sprite sprite;
			if (haveChildImage) sprite.setImage(childImage);
			sprite.velocity = velocity;
			sprite.lifespan = lifespan;
			sprite.setPosition(trans * tMatrices);
			sprite.birthtime = time;
			sprite.height = childHeight;
			mySound.play();
			sys->add(sprite);
			lastSpawned = time;
			stop();
		
		}
		
		
	}
	sys->update();
}
void Emitter::updateEnemy() {

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans * tMatrices);
		sprite.birthtime = time;
		sprite.height = childHeight;
		sys->add(sprite);
		lastSpawned = time;
	}
	sys->update();
}
/* old update version that just pauses bullet
void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0/rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans);
		sprite.birthtime = time;
		sys->add(sprite);
		lastSpawned = time;
		
	}
	sys->update();
}
*/
// Start/Stop the emitter.
//
void Emitter::start() {
	if (!started) {
		started = true;
		lastSpawned = ofGetElapsedTimeMillis();
	}
}

void Emitter::stop() {
	started = false;
}

void Emitter::removee() {
	
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	childHeight = img.getHeight();
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
}

void Emitter::setRate(float r) {
	rate = r;
}
// method to shoot once, not used for now
void Emitter::activateShootOnce(bool switch1) {

	shootOnce = switch1;
}
/*
//--------------------------------------------------------------
void Emitter::setup(){

	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/space-invaders-ship-scaled.png")) {
		imageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/space-invaders-ship-scaled.png");
		ofExit();
	}

	// set up spacing for row of numEmitters across top of window
	//
	numEmitters = 5;
	float margin = 50;
	float emitterWidth = 50;
	ofPoint winsize = ofGetWindowSize();
	float spacing = (winsize.x-emitterWidth - margin*2) / (numEmitters - 1);
	float x = emitterWidth / 2 + margin;

	// create an array of emitters and set their position;
	//
	for (int i = 0;  i < numEmitters; i++) {
		Emitter *emit = new Emitter(new SpriteSystem());
		emit->setPosition(ofVec3f(x, 0, 0));
		emit->drawable = false;                // make emitter itself invisible
		emit->setChildImage(defaultImage);
		x += spacing;
		emitters.push_back(emit);
		emit->start();
	}

	
	gui.setup();
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, 100, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	bHide = false;

}
*/
/*
//--------------------------------------------------------------
void Emitter::update() {

	for (int i = 0; i < emitters.size(); i++) {
		Emitter *emitter = emitters[i];
		emitter->setRate(rate);
		emitter->setLifespan(life * 1000);    // convert to milliseconds 
		emitter->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		emitter->update();
	}
}


//--------------------------------------------------------------
void Emitter::draw(){
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i]->draw();
	}

	if (!bHide) {
		gui.draw();
	}
}
*/

