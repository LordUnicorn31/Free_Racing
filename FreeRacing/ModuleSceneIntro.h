#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "Timer.h"


#define MAX_SNAKE 2
struct obj_info
{
	Cube* cubei;
	PhysBody3D* pcubei;
};
struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void worldlimits();
	void ramp(int positionx, int positiony, int positionz, int updown);
	void cornerfloor(int positionx, int positiony, int positionz);
	void corner(int positionx, int positiony, int positionz, int side);
    void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void path(int positionx, int positiony, int positionz, int angle, vec3 rotation);
	void wall(int positionx, int positiony, int positionz, int angle, vec3 rotation, int type);
	void road(int positionx, int positiony, int positionz, int angle, vec3 rotation);
	void obstacle1(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez, Color color, TypeObject type=TypeObject::NONE, bool is_sensor = false, bool wire = false);
	void Building(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez);
	void speedboost(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez, Color color);
	void turbine();
	PhysBody3D* CubeBody(Cube* cube, Module* Callback, TypeObject type = TypeObject::NONE, bool is_sensor = false, float mass = 0.00f);
	Cube* cube(vec3 position, vec3 size, Color rgb, float angle = 0, vec3 pivot = 0);
	obj_info spin(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez, float angle, float mass = 1000.0f);
	obj_info torque(int positionx, int positiony, int positionz, float angle, float mass = 1000.0f);
	void Map();
	void setobs();


public:
	/*
	PhysBody3D* pb_snake[MAX_SNAKE];
	Sphere s_snake[MAX_SNAKE];

	PhysBody3D* pb_snake2[MAX_SNAKE];
	Sphere s_snake2[MAX_SNAKE];
	*/

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	p2List<PhysBody3D*> bodylist;
	p2List<Cube*> cubelist;
	vec3 roadborder = (100, 80, 1);
	vec3 roadsize = (200, 1, 1);
	uint stagemusic;
	uint lapsound;
	int laps = 0;
	Timer laptime;
	Timer globaltime;
	Timer blockt;
	Timer Laptimesound;
	Uint32 Lap1, Lap2, Lap3;
	int lap1 = 0, lap2 = 0, lap3 = 0;
	int global_time = 0;
	bool restart = false;
	bool passLine = false;
	bool lock = false;
	obj_info spin1;
	obj_info move1;
	obj_info spin2;
	obj_info move2;
	obj_info spin3;
	obj_info move3;
};
