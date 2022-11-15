/*
 * @Author: Raul Cisneros
 * @Date:   11/02/21
 */
#include "ofApp.h"
#include "ofxGui.h"

void TriangleShape::draw() {

	// gets translation,rotation and scale info
	trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), this->scale);

	//creates our transformation matrix
	T = trans * rot * scale;

	// sets up the heading 
	glm::vec3 initialHeading = glm::vec3(0, 1, 0);
	glm::vec3 heading = rot * glm::vec4(initialHeading, 1);
	newheading = glm::normalize(heading);

	ofSetColor(ofColor::white);

	//  1st approach transform each point
//	glm::vec3 p1 = T * glm::vec4(verts[0], 1);
//	glm::vec3 p2 = T * glm::vec4(verts[1], 1);
//	glm::vec3 p3 = T * glm::vec4(verts[2], 1);

	// 2nd approach - tell OpenGL to transform all points using the "current transformation matrix" (CTM);
	ofPushMatrix();
	ofMultMatrix(T);
	
	// now everything I draw will be transformed by the matrix "T"
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	
	ofPopMatrix();
}

void ofApp::integrate() {

	// (1) update position from velocity and time interval
	tri.pos += velocity2 * ((1.0) / framerate);
	// (2) update velocity (based on acceleration
	velocity2 += acceleration * ((1.0) / framerate);
	// (3) multiply final result by the damping factor to sim drag
	velocity2 = velocity2 * damping;

	// update rotation from angularVelocity and time
	tri.rotation += angularVelocity * ((1.0) / framerate);
	// update angularVelocity from angularAcceleration
	angularVelocity += angularAcceleration * ((1.0) / framerate);
	// multiply final result by damping factor
	angularVelocity = angularVelocity * damping;

}
//--------------------------------------------------------------
void ofApp::setup() {

	score = 0;
	image.load("images/backgroundF.jpg");

	spriteImage.load("images/Spaceship.png");
	spriteImage.mirror(true, false);

	enemyImage.load("images/enemy.png");
	enemyLImage.load("images/enemyL.png");
	enemyRImage.load("images/enemyR.png");

	
	asteroidImage.load("images/asteroid.png");

	starImage.load("images/star.png");

	explosion.load("sounds/explosion.mp3");

	text.loadFont("Oswald-Light.ttf", 20);

	tri.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 1);
	tri.rotation = 180.0;
	thrust2 = 500;
	// Sets Damping of turret
	damping = 0.99;
	// Sets initial acceleration of turret
	acceleration = glm::vec3(0,0,0);
	// Sets initial velocity of turret
	velocity2 = glm::vec3(0, 0, 0);



	if (defaultImage.load("images/lazer.png")) {
		imageLoaded = true;
		defaultImage.mirror(true, false);
	}
	else {
		ofLogFatalError("can't load image: images/space-invaders-ship-scaled.png");
		ofExit();
	}
	//create emiiters
	turret = new Emitter(new SpriteSystem());
	
	enemy = new Emitter(new SpriteSystem());
	enemy2 = new Emitter(new SpriteSystem());
	enemy3 = new Emitter(new SpriteSystem());

	enemyAsteroid = new Emitter(new SpriteSystem());
	enemyAsteroid2 = new Emitter(new SpriteSystem());
	enemyAsteroid3 = new Emitter(new SpriteSystem());
	star = new Emitter(new SpriteSystem());

	turret->activateShootOnce(true);
	//	turret = new Emitter();

	
	turret->setPosition(ofVec3f(tri.pos));
	turret->drawable = false;
	turret->setChildImage(defaultImage);
	turret->rot = 180;

	enemy->drawable = false;
	enemy->setChildImage(enemyImage);
	enemy->rot = 180;
	
	enemy2->drawable = false;
	enemy2->setChildImage(enemyLImage);
	enemy2->rot = 180;

	enemy3->drawable = false;
	enemy3->setChildImage(enemyRImage);
	enemy3->rot = 180;

	enemyAsteroid->drawable = false;
	enemyAsteroid->setChildImage(asteroidImage);
	enemyAsteroid->rot = 180;

	enemyAsteroid2->drawable = false;
	enemyAsteroid2->setChildImage(asteroidImage);
	enemyAsteroid2->rot = 180;

	enemyAsteroid3->drawable = false;
	enemyAsteroid3->setChildImage(asteroidImage);
	enemyAsteroid3->rot = 180;

	star->drawable = false;
	star->setChildImage(starImage);
	star->rot = 180;
	
	//sets up gui
	gui.setup();
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, 200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(shipVelocity.setup("Ship Velocity", ofVec3f(0, 200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(imageScale.setup("Image Scale", .21, .2, 3));
	gui.add(scale.setup("Scale", 200, 1, 400));
	gui.add(cycles.setup("Cycles", 4, 1, 10));
	gui.add(speed.setup("Speed", 400, 200, 800));
	gui.add(thrust.setup("Thrust", 1, 1, 10));


	partExplosion.setOneShot(true);
	partExplosion.setEmitterType(RadialEmitter);
	partExplosion.setGroupSize(50);
	partExplosion.setLifespan(0.0625);
	partExplosion.setVelocity(ofVec3f(1000, 0, 0));
	partExplosion.setParticleRadius(10);
}


//--------------------------------------------------------------
void ofApp::update() {
	
	integrate();

	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 
	turret->setVelocity(ofVec3f(shipVelocity->x, shipVelocity->y, shipVelocity->z) * tri.rot);
	turret->setPosition(ofVec3f(0, 160.0, 0));
	turret->trans = turret->trans + 0 * tri.newheading;
	turret->setMatrix(tri.T);

	turret->update();		

//	if (score <= 0 && startscreen == false) {
	enemy->start();
		enemy->setRate(rate);
		enemy->setLifespan(life * 1000);    // convert to milliseconds 
		enemy->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemy->setPosition(ofVec3f(ofGetWindowWidth() / 1.8, 0, 0));

	//	enemy->update();
		enemy->updateEnemy();
		checkCollisions(enemy);
		playerHit(enemy);
//	}
	
	if (score >= 5 ) {
		enemyAsteroid->start();

		enemyAsteroid->setRate(rate);
		enemyAsteroid->setLifespan(life * 1000);    // convert to milliseconds 
		enemyAsteroid->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemyAsteroid->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, 0, 0));

		AsteroidParabolaPath(enemyAsteroid);
		enemyAsteroid->updateEnemy();
		checkCollisions(enemyAsteroid);
		playerHit(enemyAsteroid);
	}
	
	// Increase approachRange and approachVelocity as score increases
	if (score >= 10) {
		enemy2->start();
		
		enemy2->setRate(rate);
		enemy2->setLifespan(life * 1000);    // convert to milliseconds 
		enemy2->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemy2->setPosition(ofVec3f(ofGetWindowWidth(),ofGetWindowHeight()/2, 0));
		LinearPathLeft(enemy2);
		followShip(enemy2);

		enemy2->updateEnemy();
		checkCollisions(enemy2);
		playerHit(enemy2);
		
		enemyAsteroid2->start();

		enemyAsteroid2->setRate(rate);
		enemyAsteroid2->setLifespan(life * 1000);    // convert to milliseconds 
		enemyAsteroid2->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemyAsteroid2->setPosition(ofVec3f(-(ofGetWindowWidth() / 4), 0, 0));

		AsteroidSinPath(enemyAsteroid2);
		enemyAsteroid2->updateEnemy();
		checkCollisions(enemyAsteroid2);
		playerHit(enemyAsteroid2);
	}
	if (score >= 15) {
		enemy3->start();

		enemy3->setRate(rate);
		enemy3->setLifespan(life * 1000);    // convert to milliseconds 
		enemy3->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemy3->setPosition(ofVec3f(0, ofGetWindowHeight() / 3, 0));

		LinearPath(enemy3);
		followShip(enemy3);

		enemy3->updateEnemy();

		checkCollisions(enemy3);
		playerHit(enemy3);

		enemyAsteroid3->start();

		enemyAsteroid3->setRate(rate);
		enemyAsteroid3->setLifespan(life * 1000);    // convert to milliseconds
		enemyAsteroid3->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		enemyAsteroid3->setPosition(ofVec3f(ofGetWindowWidth() / 1.05, 0, 0));
		enemyAsteroid3->updateEnemy();
		checkCollisions(enemyAsteroid3);
		playerHit(enemyAsteroid3);

	}
	if (score >= 20) {
		star->start();

		star->setRate(rate);
		star->setLifespan(life * 1000);    // convert to milliseconds
		star->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		star->setPosition(ofVec3f(ofGetWindowWidth() / 6, 0, 0));
		followShip(star);
		star->updateEnemy();
		checkCollisions(star);
		playerHit(star);
	}
	partExplosion.update();
}
glm::vec3 ofApp::curveEval(float x, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWidth();
	return (glm::vec3(x, -scale * sin(u) + (ofGetHeight() / 2), 0));
}
glm::vec3 ofApp::parabolaEval(float x, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (permaCycle * x * PI) / (ofGetWidth());
	return (glm::vec3(x, -permaParabolaScale * pow(u,2)  + (ofGetHeight() + 300), 0));
}

glm::vec3 ofApp::LinearEval(float x)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (permaCycle * x * PI) / (ofGetWidth());
	return (glm::vec3(x, -permaParabolaScale * pow(u, 1) + (ofGetHeight() / 2), 0));
}
glm::vec3 ofApp::LinearEvalHigh(float x)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (permaCycle * x * PI) / (ofGetWidth());
	return (glm::vec3(x, -permaParabolaScale * pow(u, 1) + (ofGetHeight() / 3), 0));
}
//--------------------------------------------------------------
void ofApp::draw() {

	if (startscreen) {
		drawStartScreen();
	}
	else {

		//ofSetColor(ofColor::red);
		if (fullscreen == true) {
			image.draw(0, 0, 2000, 1200);
		}
		else {
			image.draw(0, 0, 1200, 1000);
		}

		//draws the heading for visualization purposes
		if (headingVisible == true) {
			ofDrawLine(tri.pos, tri.pos + 100 * tri.newheading);
		}
		
		tri.draw();

		if (guiMenu == true) {
			gui.draw();
		}
		if (useSpaceship == true) {

			float scale = imageScale;   // we need to do this extra assign to pull the value from the slider
			tri.scale = glm::vec3(scale, scale, scale);
			
			enemy->draw(); // outside everything but scale,translation,rotation matches the ship
			
			enemy2->draw();
			enemy3->draw();
			enemyAsteroid->draw();
			enemyAsteroid2->draw();
			enemyAsteroid3->draw();
			star->draw();

			ofPushMatrix();
			ofMultMatrix(tri.T);
			spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
			ofPopMatrix();
			turret->draw();
			
			string scoreText;
			scoreText += "Score: " + std::to_string(score);
			text.drawString(scoreText, 7, 35);
		}
		
		// Draws Particle Explosion
		ofPushMatrix();
		ofTranslate(explosionPos);
		partExplosion.draw();
		ofPopMatrix();
		
		if (gameOver) {
			gameOverDraw();
		}
	}
}
void ofApp::AsteroidSinPath(Emitter* enemy) {
	
	speeds = this->speed / ofGetFrameRate();

	for (int i = 0; i < enemy->sys->sprites.size(); i++) {
		headingSprite = glm::normalize(curveEval(enemy->sys->sprites[i].trans.x + speed, scale, cycles) - enemy->sys->sprites[i].trans);
		p = enemy->sys->sprites[i].trans + (headingSprite * speeds);
		newPos = curveEval(p.x, scale, cycles);
		headingSprite = glm::normalize(newPos - enemy->sys->sprites[i].trans);   // save heading because we will draw it later
		enemy->sys->sprites[i].trans = newPos;
		enemy->sys->sprites[i].rot = -glm::degrees(glm::orientedAngle(headingSprite, homeOrient, home));
	}
}

void ofApp::AsteroidParabolaPath(Emitter* enemy) {

	speeds = this->speed / ofGetFrameRate();

	for (int i = 0; i < enemy->sys->sprites.size(); i++) {

		headingSprite = glm::normalize(parabolaEval(enemy->sys->sprites[i].trans.x + speeds,cycles) - enemy->sys->sprites[i].trans);
		p = enemy->sys->sprites[i].trans + (headingSprite * speeds);
		newPos = parabolaEval(p.x,cycles);
		headingSprite = glm::normalize(newPos - enemy->sys->sprites[i].trans);   // save heading because we will draw it later
		enemy->sys->sprites[i].trans = newPos;
		enemy->sys->sprites[i].rot = -glm::degrees(glm::orientedAngle(headingSprite, homeOrient, home));

	}
}
void ofApp::LinearPath(Emitter* enemy) {

	speeds = this->speed / ofGetFrameRate();

	for (int i = 0; i < enemy->sys->sprites.size(); i++) {

		headingSprite = glm::normalize(LinearEval(enemy->sys->sprites[i].trans.x + speeds) - enemy->sys->sprites[i].trans);
		p = enemy->sys->sprites[i].trans + (headingSprite * speeds);
		newPos = LinearEval(p.x);
		headingSprite = glm::normalize(newPos - enemy->sys->sprites[i].trans);   // save heading because we will draw it later
		enemy->sys->sprites[i].trans = newPos;
		enemy->sys->sprites[i].rot = -glm::degrees(glm::orientedAngle(headingSprite, homeOrient, home));

	}
}
void ofApp::LinearPathLeft(Emitter* enemy) {

	speeds = this->speed / ofGetFrameRate();

	for (int i = 0; i < enemy->sys->sprites.size(); i++) {

		headingSprite = glm::normalize(LinearEvalHigh(enemy->sys->sprites[i].trans.x + speeds) - enemy->sys->sprites[i].trans);
		p = enemy->sys->sprites[i].trans + (-headingSprite * speeds);
		newPos = LinearEvalHigh(p.x);
		headingSprite = glm::normalize(newPos - enemy->sys->sprites[i].trans);   // save heading because we will draw it later
		enemy->sys->sprites[i].trans = newPos;
		enemy->sys->sprites[i].rot = -glm::degrees(glm::orientedAngle(-headingSprite, homeOrient, home));

	}
}
void ofApp::followShip(Emitter* enemy) {
	for (int i = 0; i < enemy->sys->sprites.size(); i++) {
		float distance = sqrtf(pow(tri.pos.x - enemy->sys->sprites[i].trans.x, 2) + pow(tri.pos.y - enemy->sys->sprites[i].trans.y, 2));
		// Trigger rotation once distance between turret and enemy2 sprite is within threshold
		if (distance <= approachRange) {
			//		cout << turret->trans.x - enemy->sys->sprites[i].trans.x  << endl;
			if (tri.pos.x - enemy->sys->sprites[i].trans.x < 0) {
				enemy->sys->sprites[i].rot = ofRadToDeg(acos((tri.pos.y - enemy->sys->sprites[i].trans.y) / distance));
			}
			else {
				enemy->sys->sprites[i].rot = -ofRadToDeg(acos((tri.pos.y - enemy->sys->sprites[i].trans.y) / distance));
				enemy->sys->sprites[i].rot += 180;
				enemy->sys->sprites[i].setVelocity(approachVelocity * enemy->sys->sprites[i].heading());
			}
		}
	}
}
void ofApp::drawStartScreen() {
	//ofSetBackgroundColor(ofColor::black);
	//image.draw(1024, 768);
	//ofDrawBitmapString("Press the spacebar to start!", ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0);
	string scoreText;

	if (fullscreen == true) {
		image.draw(0, 0, 2000, 1200);
		scoreText = "Press the spacebar to start!";
		text.drawString(scoreText, ofGetWidth() / 2.3, ofGetHeight() / 2);
		scoreText = " Use Arrow keys to move and spacebar to shoot!";
		text.drawString(scoreText, ofGetWidth() / 2.6, ofGetHeight() / 1.9);
	}
	else {
		image.draw(0, 0, 1200, 1000);

		scoreText = "Press the spacebar to start!";
		text.drawString(scoreText, ofGetWidth() / 2.2, ofGetHeight() / 2);
		scoreText = " Use Arrow keys to move and spacebar to shoot!";
		text.drawString(scoreText, ofGetWidth() / 2.6, ofGetHeight() / 1.9);
	}

}

// inside() test method - check to see if point p is inside triangle.
// how could you improve the design of this method ?
bool TriangleShape::inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 v1 = glm::normalize(p1 - p);
	glm::vec3 v2 = glm::normalize(p2 - p);
	glm::vec3 v3 = glm::normalize(p3 - p);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}
//--------------------------------------------------------------
//
void ofApp::keyPressed(int key) {
	// all the ifs check for out of bounds
	switch (key) {
	case OF_KEY_LEFT:
		if (gameOver == false) {
			// updates our triangles rotation 
			angularAcceleration = angularAcceleration + -this->thrust;
			break;
		}
	case OF_KEY_RIGHT:
		if (gameOver == false) {
			// updates our triangles rotation 
			angularAcceleration = angularAcceleration + this->thrust;
			break;
		}
	case OF_KEY_UP:
		if (gameOver == false) {
			if (tri.pos.x > outofBoundsHeight || tri.pos.x < outofBoundsBase || tri.pos.y < outofBoundsBase || tri.pos.y >  outofBoundsHeight) {
				tri.pos = tri.pos - (glm::vec3(1, 1, 1) * tri.newheading);
				break;
			}
			else {
				acceleration = (acceleration + tri.newheading) * this->thrust;
				break;
			}
		}
	case OF_KEY_DOWN:
		if (gameOver == false) {
			if (tri.pos.x > outofBoundsHeight || tri.pos.x < outofBoundsBase || tri.pos.y < outofBoundsBase || tri.pos.y >  outofBoundsHeight) {
				tri.pos = tri.pos + (glm::vec3(1, 1, 1) * tri.newheading); //test this plus later
				break;
			}
			else {
				acceleration = (acceleration - tri.newheading) * this->thrust;
				break;
			}
		}
	case ' ':
		if (starScreen == true) {
			startscreen = false;
			starScreen = false;
		}
		else if (starScreen == false) {
			if (gameOver == false) {
				turret->start();
			}
		}

		break;
	case 'h':
		//helps turn on header
		headingVisible = !headingVisible;
		shrink = true;
		break;
	case 's':
		useSpaceship = !useSpaceship;
		break;
	case 'g':
		guiMenu = !guiMenu;
		break;
	case 'f':
		ofToggleFullscreen();
		fullscreen = true;
		break;
	case 'r':
		if (gameOver) {
			gameOver = false;
			score = 0;
			enemy->start();
		}
		break;
	default:
		break;
	}
}

void ofApp::checkCollisions(Emitter* sampleEnemy) {
	// Find distance when two sprites will collide
	float collisionDist = turret->childHeight / 2 + sampleEnemy->childHeight / 2;
	bool test = true;
	// Iterate through each turret sprite and remove any enemy and turret lazer sprite
	// that came within collision distance
	for (int i = 0; i < turret->sys->sprites.size(); i++) {
		if (sampleEnemy->sys->checkCollision(turret->sys->sprites[i].trans, collisionDist)) {
			
			explosionPos = turret->sys->sprites[i].trans;
			
			partExplosion.start();

			explosion.play();
			// Increases the score
			score++;
			// Removes specific lazer sprite from game
			turret->sys->remove(i);
		}
	}
}
void ofApp::playerHit(Emitter* sampleEnemy)
{
	// Find distance when enemy sprite and turret emitter collide
	float collisionDist = turret->height / 2 + sampleEnemy->childHeight / 2;

	if (sampleEnemy->sys->checkCollision(tri.pos, collisionDist)) {
		
		explosionPos = tri.pos;
		
		partExplosion.start();
		
		explosion.play();
		// Sets Game Over to true
		gameOver = true;
	}
}
void ofApp::gameOverDraw(){
		// Removes all turret sprites
		for (int i = 0; i < turret->sys->sprites.size(); i++) {
			turret->sys->remove(i);
		}
		// Removes all enemy sprites
		for (int i = 0; i < enemy->sys->sprites.size(); i++) {
			enemy->sys->remove(i);
		}
		// Removes all enemy sprites
		for (int i = 0; i < enemy2->sys->sprites.size(); i++) {
			enemy2->sys->remove(i);
		}
		// Removes all enemy sprites
		for (int i = 0; i < enemy3->sys->sprites.size(); i++) {
			enemy3->sys->remove(i);
		}
		// Removes all enemy2 sprites
		for (int i = 0; i < enemyAsteroid->sys->sprites.size(); i++) {
			enemyAsteroid->sys->remove(i);
		}
		// Removes all enemy2 sprites
		for (int i = 0; i < enemyAsteroid2->sys->sprites.size(); i++) {
			enemyAsteroid2->sys->remove(i);
		}
		// Removes all enemy2 sprites
		for (int i = 0; i < enemyAsteroid3->sys->sprites.size(); i++) {
			enemyAsteroid3->sys->remove(i);
		}
		for (int i = 0; i < star->sys->sprites.size(); i++) {
			star->sys->remove(i);
		}

		// Replaces highScore with score if score exceeds highScore
		if (score > highScore)
			highScore = score;
		
		// Stops all emitters
		turret->stop();
		enemy->stop();
		enemy2->stop();
		enemy3->stop();
		enemyAsteroid->stop();
		enemyAsteroid2->stop();
		enemyAsteroid3->stop();
		star->stop();
		
		// Draws "Game Over" screen, displays high score, and shows directions for restrating
		string Text = "Game Over! High Score: " + std::to_string(highScore);
		text.drawString(Text, ofGetWindowWidth() / 2 - text.stringWidth(Text) / 2, ofGetWindowHeight() / 2);
		Text = "\nPress R to restart game";
		text.drawString(Text, ofGetWindowWidth() / 2 - text.stringWidth(Text) / 2, ofGetWindowHeight() / 2);
	
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		if (starScreen != true) {
			isShooting = false;
			turret->stop();
			break;
		}
	case OF_KEY_LEFT:   // turn left
		angularAcceleration = 0;
		break;
	case OF_KEY_RIGHT:  // turn right
		angularAcceleration = 0;
		break;
	case OF_KEY_UP:     // go forward
		acceleration = glm::vec3(0, 0, 0);
		break;
	case OF_KEY_DOWN:   // go backward
		acceleration = glm::vec3(0, 0, 0);
		break;
	default:
		break;
	}
}
void ofApp::teleportBounds() {

	// When moves off edge of screen, appears on other side
	if (tri.pos.x > ofGetWindowWidth())
		tri.pos = (ofVec3f(0, tri.pos.y, 0));
	if (tri.pos.x < 0)
		tri.pos = (ofVec3f(ofGetWindowWidth(), tri.pos.y, 0));
	if (tri.pos.y > ofGetWindowHeight())
		tri.pos = (ofVec3f(tri.pos.x, 0, 0));
	if (tri.pos.y < 0)
		tri.pos = (ofVec3f(tri.pos.x, ofGetWindowHeight(), 0));
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	//cout << glm::vec2(x, y) << endl;

	// if the mouse is inside it will move the triangle

	if (inside != false) {
		cout << "Inside" << endl;
		if (tri.pos.x > outofBoundsHeight || tri.pos.x < outofBoundsBase || tri.pos.y < outofBoundsBase || tri.pos.y >  outofBoundsHeight) {

			// keeps triangle position in bounds
			tri.pos -= (glm::vec3(1, 1, 1) * tri.newheading);

		}
		else {
			mouseCurrentPos = glm::vec3(x, y, 1);

			// calculates the difference between mouse position 
			mousePosDifference = mouseCurrentPos - mouseLastPos;

			// adds the mouses difference to our triangles position  
			tri.pos += mousePosDifference;

			// store last mouse position for later use
			mouseLastPos = mouseCurrentPos;
		}

	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	//stores current mouse coordinates
	mouseCurrentPos = glm::vec3(x, y, 1);

	// gets the new position of the triangle after moving the original triangle 
	glm::vec3 movedPos = glm::inverse(tri.T) * glm::vec4(mouseCurrentPos, 1);

	// checks to see if the mouse is inside the triangle 
	inside = tri.inside(movedPos, tri.verts[0], tri.verts[1], tri.verts[2]);

	//if the mouse is inside then store current position in the last position
	if (inside) {
		mouseLastPos = mouseCurrentPos;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
