#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)


enum groupIndex {
	BALL = -2,
	RIGID_PINBALL = -1,
	FLIPPERS,
	PLUNGE_TOP,
	PLUNGE_BOTTOM,

	NO_DEF
};

enum PhysBody_Type {
	BALL_,
	TRIANGLE,
	
	NO_DEF_
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	b2Body* bodyB;
	b2Joint* joint;
	Module* listener;
	PhysBody_Type physType = NO_DEF_;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();


	PhysBody* CreateRightFlipper();
	PhysBody* CreateLeftFlipper();
	PhysBody* CreateBall(int x, int y, int radius);
	PhysBody* CreatePlunge();

	PhysBody* CreateCircle(int x, int y, int radius);
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, Module* listener);
	PhysBody* CreateChain(int x, int y, int* points, int size, groupIndex index, float restitution, PhysBody_Type type);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);


	b2World* world;
private:

	bool debug;
	b2Body* ground;

	// Mouse joint
	b2MouseJoint* mouse_joint;
	b2Body* body_clicked;
};