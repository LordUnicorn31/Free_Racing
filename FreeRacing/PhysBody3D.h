#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"

class btRigidBody;
class Module;

enum class TypeObject {

	WALL,
	BOOST,
	CAR,
	TURBINE,
	OUT_OF_BOUNDS,
	START,
	JOINT,
	NONE

};

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void Sensor(bool sensor)const;
	btRigidBody* getPointer();

private:
	btRigidBody* body = nullptr;

public:
	TypeObject type = TypeObject::NONE;
	p2List<Module*> collision_listeners;
	bool mutable sensor = false;
};

#endif // __PhysBody3D_H__