#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f
#define BPOWER 1700.0f
#define SPOWER 1700.00f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void resetpos();
	void addvehicle();
	void MConditions();
	void resetdirection();


public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	float skid;
	float zoom = 10.00;
	float accelerationcam = 0.02;
	float currentvelocity;
	bool constantVelocity;
	bool Boost = false;
	bool Menu = true;
	bool PlayMusic = true;
	uint esound;
	uint startesound;
	uint eloop;
	uint constspeedengine;
	uint accengine;
	uint brakesound;
	uint turning;
	uint tireskid;
	uint boostsound;
	Timer elooptimer;
	Timer econstTimer;
	Timer eaccTimer;
	Timer boosttimer;
	mat4x4 matrix;
};