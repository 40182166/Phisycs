#include "game.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

using namespace std;
using namespace graphics_framework;
using namespace glm;

//#ifndef SPRING_H
//#define SPRING_H
//
//
//class SPRING
//{
//public:
//	//Indices of the balls at either end of the spring
//	unique_ptr<Entity> ball1;
//	unique_ptr<Entity> ball2;
//
//	//Tension in the spring
//	float tension;
//
//	float constant;
//	float length;
//
//	SPRING()
//	{}
//	~SPRING()
//	{}
//};
//
//#endif	//SPRING_H


#define physics_tick 1.0 / 60.0

static vector<unique_ptr<Entity>> SceneList;
static unique_ptr<Entity> floorEnt;

static vector<unique_ptr<Entity>> balls;

//float springConstant = 15.0f;
//float naturalLength = 1.0f;

//SPRING * spring;

unique_ptr<Entity> CreateParticle() {
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(vec3(0, 5.0 + (double)(rand() % 200) / 20.0, 0));
	//ent->SetPosition(vec3(10.0, 10.0f, 10.0f));
	unique_ptr<Component> physComponent(new cPhysics());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
	renderComponent->SetColour(phys::RandomColour());
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}


void CreateSpring(const vector<unique_ptr<Entity>> &particles, float originalLength, float k)
{
	vec3 spring = particles[1]->GetPosition() - particles[0]->GetPosition();
	auto length = glm::length(spring);
	float displacement = length - originalLength;

	vec3 springNormal = spring / length;
	vec3 restoreForce = springNormal * (displacement * k);
}


bool update(double delta_time) {
	static double t = 0.0;
	static double accumulator = 0.0;
	accumulator += delta_time;

	while (accumulator > physics_tick) {
		UpdatePhysics(t, physics_tick);
		accumulator -= physics_tick;
		t += physics_tick;
	}

	for (auto &e : SceneList) {
		e->Update(delta_time);
	}

	phys::Update(delta_time);
	return true;
}

bool load_content() {
	phys::Init();
	for (size_t i = 0; i < 2; i++) {
		SceneList.push_back(move(CreateParticle()));
		balls.push_back(move(CreateParticle()));
	}

	CreateSpring(balls, 0.001f, 0.0001f);

	/*spring = new SPRING;
	spring->ball1 == balls[0];
	spring->ball2 == balls[1];

	spring->constant = springConstant;
	spring->length = naturalLength;*/

	floorEnt = unique_ptr<Entity>(new Entity());
	floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));

	phys::SetCameraPos(vec3(10.0f, 40.0f, -50.0f));
	phys::SetCameraTarget(vec3(0.0f, 0.0f, 0));
	InitPhysics();
	return true;
}

bool render() {
	for (auto &e : SceneList) {
		e->Render();
	}
	phys::DrawScene();
	return true;
}

void main() {
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}

