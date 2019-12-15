#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	laptime.Start();
	globaltime.Start();

	Map();
	restart = false;

	laps = 0;
	lap1 = 0;
	lap2 = 0;
	lap3 = 0;
	global_time = 0;

	passLine = false;
	lock = false;

	obstacle1(100, 4, -180, 25, 1, 1, Green, TypeObject::NONE, true, true);
	lapsound = App->audio->LoadFx("FX/lapsound.wav");

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	for (p2List_item<Cube*>* item = cubelist.getFirst(); item != nullptr; item = item->next)
	{
		cubelist.clear();
		bodylist.clear();
	}

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	global_time = (int)globaltime.Readsec();
	p2List_item<Cube*>* cuberend = cubelist.getFirst();
	while (cuberend != nullptr) {
		cuberend->data->Render();
		cuberend = cuberend->next;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
	{
		restart = true;
	}
	if (lock == true && blockt.Readsec() >= 30)
	{
		lock = false;
	}
	if (passLine && lock == false)
	{
		if (laps == 1)
		{
			lap1 = laptime.Readsec();
			lap1 = (int)lap1;
		}
		else if (laps == 2)
		{
			lap2 = laptime.Readsec();
			lap2 = (int)lap2;

		}
		else if (laps == 3)
		{
			lap3 = laptime.Readsec();
			lap3 = (int)lap3;
			restart = true;
			App->player->Menu = true;
		}
		lock = true;
		laps++;
		laptime.Start();
		passLine = false;
		blockt.Start();
	}
	if (restart) //Same as the start function
	{
		App->player->resetpos();
		App->player->resetdirection();
		App->player->vehicle->Stop();

		laps = 0;
		lap1 = 0;
		lap2 = 0;
		lap3 = 0;

		globaltime.Start();
		restart = false;
		
	}
	spin1.pcubei->GetTransform(&spin1.cubei->transform);
	spin1.cubei->Render();
	spin2.pcubei->GetTransform(&spin2.cubei->transform);
	spin2.cubei->Render();
	spin3.pcubei->GetTransform(&spin3.cubei->transform);
	spin3.cubei->Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1->type == TypeObject::BOOST) {
		App->player->Boost = true;
		App->player->boosttimer.Start();
	}
	else if (body1->type == TypeObject::OUT_OF_BOUNDS && body2->type != TypeObject::NONE)
	{
		restart = true;
	}
	else if (body1->type == TypeObject::START)
	{
		if (Laptimesound.Read() > 4000) {
			App->audio->PlayFx(lapsound);
			Laptimesound.Start();
		}
		passLine = true;
	}
}

Cube* ModuleSceneIntro::cube(vec3 position, vec3 size, Color rgb, float angle, vec3 pivot)
{
	Cube* cube = new Cube;
	cube->SetRotation(angle, pivot);
	cube->SetPos(position.x, position.y, position.z);
	cube->size = size;
	cube->wire;
	cube->color = rgb;
	return cube;
}

PhysBody3D* ModuleSceneIntro::CubeBody(Cube* cube, Module* Callback, TypeObject type, bool is_sensor, float mass)
{
	PhysBody3D* cubebody;
	cubebody = App->physics->AddBody(*cube, mass);
	cubebody->type = type;
	cubebody->collision_listeners.add(Callback);
	return cubebody;
}

void ModuleSceneIntro::path(int positionx, int positiony, int positionz, int angle, vec3 rotation)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(100, 1, 20), Blue, angle, rotation);
	bodylist.add(CubeBody(cubec, this));
	cubelist.add(cubec);
}

void ModuleSceneIntro::wall(int positionx, int positiony, int positionz, int angle, vec3 rotation, int type)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(100, 3, 1), Black, angle, rotation);
	bodylist.add(CubeBody(cubec, this));
	cubelist.add(cubec);
}

void ModuleSceneIntro::road(int positionx, int positiony, int positionz, int angle, vec3 rotation)
{
	if (angle = 0)
	{
		wall(positionx, positiony + 2, positionz + 10, angle, vec3(rotation.x, rotation.y, rotation.z), 1);
		wall(positionx, positiony, positionz - 10, angle, rotation, 1);
	}
	else
	{
		wall(positionx +10, positiony + 2, positionz, angle, vec3(rotation.x, rotation.y, rotation.z), 1);
		wall(positionx -10, positiony, positionz, angle, rotation, 1);
	}
}

void ModuleSceneIntro::cornerfloor(int positionx, int positiony, int positionz)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(20, 1, 20), White, 0, vec3(1, 0, 0));
	bodylist.add(CubeBody(cubec,this));
	cubelist.add(cubec);
}

void ModuleSceneIntro::corner(int positionx, int positiony, int positionz, int side)
{
	if (side == 1)
	{
		wall(positionx, positiony + 2, positionz - 10, 0, vec3(1, 0, 0), 2);
		wall(positionx + 10, positiony + 2, positionz, 90, vec3(0, 1, 0), 2);
	}

	cornerfloor(positionx, positiony, positionz);
}

void ModuleSceneIntro::ramp(int positionx, int positiony, int positionz, int updown)
{
	if (updown == 1)
	{
		Cube* cubec = cube(vec3(positionx, positiony + 8, positionz), vec3(100, 1, 20), White, 20, vec3(0, 0, 1));
		bodylist.add(CubeBody(cubec, this));
		cubelist.add(cubec);
	}
	else if (updown == 2)
	{
		Cube* cubec = cube(vec3(positionx, positiony + 8, positionz), vec3(100, 1, 20), White, -20, vec3(0, 0, 1));
		bodylist.add(CubeBody(cubec, this));
		cubelist.add(cubec);
	}
}

void ModuleSceneIntro::Map()
{
	corner(420, 2, -370, 1);
	road(420, 2, -310, 90, vec3(0, 1, 0));
	road(420, 2, -210, 90, vec3(0, 1, 0));
	road(420, 2, -110, 90, vec3(0, 1, 0));
	road(420, 2, -10, 90, vec3(0, 1, 0));
	corner(420, 2, 50, 2);

	road(360, 2, 50, 0, vec3(0, 1, 0));
	road(260, 2, 50, 0, vec3(0, 1, 0));
	road(160, 2, 50, 0, vec3(0, 1, 0));
	road(60, 2, 50, 0, vec3(0, 1, 0));
	road(-40, 2, 50, 0, vec3(0, 1, 0));
	road(-140, 2, 50, 0, vec3(0, 1, 0));
	corner(-200, 2, 50, 3);

	road(-200, 2, -10, 90, vec3(0, 1, 0));
	road(-200, 2, -110, 90, vec3(0, 1, 0));
	corner(-200, 2, -170, 4);

	ramp(-143, 11, -170, 1);
	ramp(-57, 42.6, -170, 1);

	ramp(89, 32, -170, 2);
	ramp(186, 6, -170, 2);
	road(260, 2, -170, 0, vec3(0, 1, 0));
	corner(320, 2, -170, 1);

	road(320, 2, -110, 90, vec3(0, 1, 0));
	corner(320, 2, -50, 2);

	road(260, 2, -50, 0, vec3(0, 1, 0));
	road(160, 2, -50, 0, vec3(0, 1, 0));
	corner(100, 2, -50, 3);

	road(100, 2, -110, 90, vec3(0, 1, 0));
	road(100, 2, -210, 90, vec3(0, 1, 0));
	road(100, 2, -310, 90, vec3(0, 1, 0));
    corner(100, 2, -370, 4);

	road(160, 2, -370, 0, vec3(0, 1, 0));
	road(260, 2, -370, 0, vec3(0, 1, 0));
	road(360, 2, -370, 0, vec3(0, 1, 0));

	//start position
	obstacle1(111, 7, -180, 3, 20, 5, Green);
	obstacle1(89, 7, -180, 3, 20, 5, Green);
    obstacle1(100, 17, -180, 20, 5, 5, Green);

	// buildings
	Building(-140, 25, -300, 200, 50, 140);
	Building(-100, 30, -60, 160, 60, 160);
	Building(260, 35, -280, 260, 70, 120);
	Building(200, 10, -110, 170, 20, 60);





	// obstacles
	setobs();
	turbine();

	//limits
	worldlimits();
}
void ModuleSceneIntro::obstacle1(int posx, int posy, int posz, int sizex, int sizey, int sizez, Color color, TypeObject type, bool is_sensor, bool wire)
{
	Cube* cubec = cube(vec3(posx, posy, posz), vec3(sizex, sizey, sizez), color, 0, vec3(0, 0, 1));
	cubec->wire = wire;
	bodylist.add(CubeBody(cubec, this, type, is_sensor));
	cubelist.add(cubec);

}

void ModuleSceneIntro::setobs()
{
    obstacle1(420, 4, -310, 2, 5, 60, Red);
	obstacle1(425, 4, -230, 2, 5, 60, Red);
	obstacle1(415, 4, -230, 2, 5, 60, Red);
	obstacle1(415, 4, -140, 10, 5, 10, Red);
	obstacle1(425, 4, -100, 10, 5, 10, Red);

	obstacle1(360, 4, 55, 5, 5, 5, Red);
	obstacle1(280, 4, 50, 5, 5, 5, Red);

	//boost
	speedboost(420, 4, -350, 15, 1, 1, Green);
	speedboost(390, 4, 50, 1, 1, 15, Green);
	speedboost(-190, 5, -170, 1, 2, 15, Green);
}

void ModuleSceneIntro::worldlimits()
{
	obstacle1(140, 0, 100, 1300, 1, 1200, Green, TypeObject::OUT_OF_BOUNDS, true);
	obstacle1(500, 90, 170, 2, 250, 1400, Blue, TypeObject::OUT_OF_BOUNDS, true);
	obstacle1(-500, 90, 170, 2, 250, 1400, Blue, TypeObject::OUT_OF_BOUNDS, true);
	obstacle1(100, 90, 200, 1400, 800, 200, Blue, TypeObject::OUT_OF_BOUNDS, true);
	obstacle1(100, 90, -550, 1400, 800, 200, Blue, TypeObject::OUT_OF_BOUNDS, true);
}

void ModuleSceneIntro::Building(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(sizex, sizey, sizez), Red, 0, vec3(0, 0, 1));
	bodylist.add(CubeBody(cubec, this));
	cubelist.add(cubec);
}

void ModuleSceneIntro::speedboost(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez, Color color)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(sizex, sizey, sizez), color, 0, vec3(0, 0, 1));
	bodylist.add(CubeBody(cubec, this, TypeObject::BOOST, true));
	cubelist.add(cubec);
}

void ModuleSceneIntro::turbine()
{
	spin1 = spin(420, 4, -10, 2, 2, 10, 90, 10);
	move1 = torque(420, 4, -10, 90, 10);
	App->physics->AddConstraintHinge(*move1.pcubei, *spin1.pcubei, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 0), true, true);
	spin2 = spin(160, 6, 55, 10, 1, 1, 90);
	move2 = torque(160, 6, 55, 90);
	App->physics->AddConstraintHinge(*move2.pcubei, *spin2.pcubei, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 0), true, true);
	spin3 = spin(60, 6, 45, 10, 1, 1, 90);
	move3 = torque(60, 6, 45, 90);
	App->physics->AddConstraintHinge(*move3.pcubei, *spin3.pcubei, vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 0), true, false);


}

obj_info ModuleSceneIntro::spin(int positionx, int positiony, int positionz, int sizex, int sizey, int sizez, float angle, float mass)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(sizex, sizey, sizez), Green, angle, vec3(0, 1, 0));
	cubelist.add(cubec);
	PhysBody3D* attach;
	attach = CubeBody(cubec, this, TypeObject::NONE, false, mass);
	attach->getPointer()->setLinearFactor(btVector3(0, 0, 0));
	obj_info c;
	c.cubei = cubec;
	c.pcubei = attach;
	return c;
}

obj_info ModuleSceneIntro::torque(int positionx, int positiony, int positionz, float angle, float mass)
{
	Cube* cubec = cube(vec3(positionx, positiony, positionz), vec3(0, 0, 0), Green, angle, vec3(0, 1, 0));
	cubelist.add(cubec);
	PhysBody3D* attached;
	attached = CubeBody(cubec, this, TypeObject::JOINT, false, mass);
	attached->getPointer()->setLinearFactor(btVector3(0, 0, 0));
	obj_info c;
	c.cubei = cubec;
	c.pcubei = attached;
	return c;
}
