#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "Timer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	addvehicle();
	esound = App->audio->LoadFx("Fx/esound.wav");
	startesound = App->audio->LoadFx("Fx/startesound.wav");
	constspeedengine = App->audio->LoadFx("Fx/constspeedengine.wav");
	accengine = App->audio->LoadFx("Fx/accengine.wav");
	brakesound = App->audio->LoadFx("Fx/brakesound.wav");
	turning = App->audio->LoadFx("Fx/turning.wav");
	tireskid = App->audio->LoadFx("Fx/tireskid.wav");
	boostsound = App->audio->LoadFx("Fx/boostsound.wav");
	eloop = App->audio->LoadFx("Fx/eloop.wav");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 2, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 12, 10);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (Menu == true) 
	{
		if (PlayMusic == true) 
		{
			App->audio->PlayMusic("Music/Menu.wav");
			PlayMusic = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			Menu = false;
			PlayMusic = true;
			App->audio->PlayFx(startesound);
			App->audio->PlayMusic("Music/CircuitMusic.wav");
		}
	}

	if (Menu == false) {

		if (Boost == true) {
			vehicle->ApplyEngineForce(BPOWER);
			App->audio->PlayFx(boostsound);

			if (boosttimer.Read() > 2000)
			{
				Boost = false;
			}
		}
		if(App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE)
		{
			App->audio->PlayFx(esound);
		}
		if (Boost == false)
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE)
			{
				if (vehicle->GetKmh() > 0) 
				{
					acceleration = -MAX_ACCELERATION * 0.25;

				}
			}
		}

	

	 if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	 {
		acceleration = MAX_ACCELERATION;
	  }

	 if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	 {
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	  }

	  if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	  {
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	  }
	  
	  if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	  {
		  acceleration = -MAX_ACCELERATION;
	  }

	  if(App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT)
	  {
		brake = BRAKE_POWER;
	  }
	  MConditions();



	     vehicle->ApplyEngineForce(acceleration);
	     vehicle->Turn(turn);
	     vehicle->Brake(brake);

	     vehicle->Render();

	 char title[80];
	 sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	 App->window->SetTitle(title);
	 App->camera->LookAt(vehicle->GetPos());

	 if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_IDLE)
	 {
		 if (currentvelocity > vehicle->GetKmh())
		 {
			 if (zoom > 15)
			 {
				 zoom -= accelerationcam;
				 App->camera->Position = (vehicle->GetPos() - vehicle->GetVec()* zoom + vec3(0, 6, 0));
			 }
			 else if (currentvelocity < vehicle->GetKmh())
			 {
				 if (vehicle->GetKmh() > 50 && zoom < 20)
				 {
					 zoom += accelerationcam;
					 App->camera->Position = (vehicle->GetPos() - vehicle->GetVec() * zoom + vec3(0, 6, 0));

				 }
			 }
			 else if (currentvelocity == vehicle->GetKmh())
			 {
				 if (currentvelocity < 2.00 && zoom > 10)
				 {
					 zoom -= 0.5;
				 }
				 App->camera->Position = (vehicle->GetPos() - vehicle->GetVec() * zoom + vec3(0, 6, 0));
			 }
		 }
		 else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		 {
			 App->camera->Position = (vehicle->GetPos() + vehicle->GetVec() * 12 + vec3(0, 6, 0));
		 }
	 }
	 currentvelocity = vehicle->GetKmh();
    }
	
	else
	{
		brake = BRAKE_POWER;
		vehicle->Brake(brake);

		App->camera->LookAt(vehicle->GetPos());
		App->camera->Position = (vehicle->GetPos() - vehicle->GetVec() *zoom + vec3(0, 500, -20));
		char title[100];
		sprintf_s(title, "PRESS SPACE BAR TO PLAY");
		App->window->SetTitle(title);
	}
	 return UPDATE_CONTINUE;
}

void ModulePlayer::MConditions()
{
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_UP)
	{
		App->audio->StopMusic(1, 300);
	}
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN && vehicle->GetKmh() > 50)
	{
		App->audio->PlayFx(brakesound);
	}
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN))
	{
		App->audio->PlayFx(turning);
	}
	if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) & vehicle->GetKmh() > 170)
	{
		App->audio->PlayFx(tireskid);
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		App->audio->StopMusic(4, 500);
		eaccTimer.Start();
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE && vehicle->GetKmh() < 10)
	{
		if (elooptimer.Read() >= 1000)
		{
			App->audio->PlayFx(eloop);
			elooptimer.Start();
		}
	}
	if (vehicle->GetKmh() > 10) 
	{
		if (econstTimer.Read() >= 4000)
		{
			App->audio->PlayFx(constspeedengine);
			econstTimer.Start();
		}
	}
	else if (vehicle->GetKmh() <= 10)
	{
		App->audio->StopMusic(3, 500);
	}

}

void ModulePlayer::resetpos()
{
	vehicle->SetPos(100, 5, -165);
	vehicle->GetTransform(&matrix);
	matrix.rotate(180, vec3(0, 1, 0));
	vehicle->SetTransform(&matrix);
}

void ModulePlayer::resetdirection()
{
	vec3 c;
	vehicle->GetTransform(&matrix);
	matrix.rotate(360, vec3(1, 0, 0));
	matrix.rotate(180, vec3(0, 1, 0));
	vehicle->SetTransform(&matrix);
	c = vehicle->GetPos();
	vehicle->SetPos(c.x, c.y + 3, c.z);
}

void ModulePlayer::addvehicle()
{
	VehicleInfo car;
	car.chassis_size.Set(2, 1, 5);
	car.chassis_offset.Set(0, 0.70, 0);

	car.chassis2_size.Set(2, 1, 1.50);
	car.chassis2_offset.Set(0, 1, 1.75);

	car.chassis3_size.Set(1.25, 1, 1);
	car.chassis3_offset.Set(0, 0.70, 2.50);

	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 1000.0f;
	car.maxSuspensionForce = 6000.0f;
	// Wheel properties ---------------------------------------

	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.0f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = false;
	car.wheels[0].brake = true;
	car.wheels[0].steering = true;
	car.wheels[0].forwardwheel = true;
	car.wheels[0].skidwheel = false;
	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = false;
	car.wheels[1].brake = true;
	car.wheels[1].steering = true;
	car.wheels[1].forwardwheel = true;
	car.wheels[1].skidwheel = false;
	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = true;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;
	car.wheels[2].forwardwheel = false;
	car.wheels[2].skidwheel = true;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = true;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;
	car.wheels[3].forwardwheel = false;
	car.wheels[3].skidwheel = true;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(100, 2, -165); //start point
								   //vehicle->SetPos(126, 5, 50);
	vehicle->GetTransform(&matrix);
	matrix.rotate(180, vec3(0, 1, 0));
	vehicle->SetTransform(&matrix);
}



