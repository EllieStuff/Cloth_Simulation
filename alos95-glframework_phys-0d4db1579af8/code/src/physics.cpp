#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\glm.hpp>
#include "Mesh.h"
#include "Utils.h"

//Exemple
extern void Exemple_GUI();
extern void Exemple_PhysicsInit();
extern void Exemple_PhysicsUpdate(float dt);
extern void Exemple_PhysicsCleanup();


ParticleSystem ps;
float angle = 0, initialAngle = 0;
int nextParticleIdx = 0;
extern bool renderParticles;
extern bool renderSphere;
extern bool renderCapsule;
extern bool renderCloth;
float maxAge = 30.f;
const int INIT_PARTICLES = 1000;
float currTime = 1.f / ps.emissionRate;
bool emissionType = true;

//class Solver {
//private:
//
//public:
//	Solver() {};
//
//	virtual void updateParticles(Mesh _mesh, glm::vec3* forces) {};
//
//};
//
//class Euler : public Solver {
//private:
//
//public:
//	Euler() {};
//
//	void updateParticles(Mesh _mesh, glm::vec3* forces) override {
//
//	}
//
//};
//
//class Verlet : public Solver {
//private:
//
//public:
//	Verlet() {};
//
//	void updateParticles(Mesh _mesh, glm::vec3* forces) override {
//
//	}
//
//};


//Solver solver;
Mesh mesh;

//void PhysicsInit() {
//	// TODO: Mirar d'on ha tret aixo del solver
//	solver = Verlet();
//	//solver = Euler();
//
//	mesh = Mesh(ClothMesh::numCols, ClothMesh::numRows);
//	renderCloth = true;
//
//	// Per a provar en una mesh mes petita
//	//mesh = Mesh(1, 2);
//
//	// Per a rendiritzar particules per a debugar
//	//renderParticles = true;
//	//LilSpheres::particleCount = mesh.width * mesh.height;
//}
//
//void PhysicsUpdate(float dt) {
//	// TODO: Posar les dades dins la funció i descomentar
//	//glm::vec3* forces = mesh.getSpringForces(/*Passar-hi dades de la funció*/);
//
//	// sumar gravetat
//
//	//solver.updateParticles(mesh, forces);
//
//	//ClothMesh::updateClothMesh(&(mesh.positions[0].x));
//	LilSpheres::updateParticles(0, mesh.width * mesh.height, &(mesh.particles[0].pos.x));
//}
//
//void PhysicsCleanup() {
//
//}


bool show_test_window = false;
void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::SliderFloat("Emission Rate", &ps.emissionRate, 0.f, 10.f);
		ImGui::SliderFloat("Particles emmited per pulse", &ps.particlesForEachEmission, 1.f, 20.f);
		ImGui::Checkbox("Fountain <--On / Off--> Cascade", &emissionType);
		//Exemple_GUI();
	}

	ImGui::End();
}

void PhysicsInit() {
	// TODO: Mirar d'on ha tret aixo del solver
	//solver = Verlet();
	//solver = Euler();

	ClothMesh::setupClothMesh();
	/*ClothMesh::numCols = 14;
	ClothMesh::numRows = 18;*/

	mesh = Mesh(ClothMesh::numCols, ClothMesh::numRows);
	renderCloth = true;

	//Exemple_PhysicsInit();

	renderParticles = true;
	ps = ParticleSystem(INIT_PARTICLES);
	renderSphere = false;
	if(renderSphere)
		Sphere::setupSphere(glm::vec3(-2, 5, 0), 2.f);
	//renderCapsule = true;
	//Capsule::setupCapsule(glm::vec3(3, 3, 0), glm::vec3(2, 8, 0), 1.5f);
}

void spawn(glm::vec3 initPos = glm::vec3(0, 0, 0), glm::vec3 initVelocity = glm::vec3(0, 0, 0)) {

	ps.UpdateParticle(nextParticleIdx, initPos, initVelocity);

	ps.spawnParticle(initPos, initVelocity);

	nextParticleIdx++;
}

void UpdateFountain(float dt) {
	if (currTime >= 1.f / ps.emissionRate) {
		currTime = 0;
		if (nextParticleIdx < ps.GetMaxParticles()) {
			for (int i = 0; i < ps.particlesForEachEmission; i++)
			{
				spawn(glm::vec3(0, 0, 0), glm::vec3(Utils::Randomize(-5, 5), Utils::Randomize(-10, -15), Utils::Randomize(-5, 5)));
			}
		}
	}
}

void UpdateCascade(float dt) {
	if (currTime >= 1.f / ps.emissionRate) {
		currTime = 0;
		if (nextParticleIdx < ps.GetMaxParticles()) {
			for (int i = 0; i < ps.particlesForEachEmission; i++)
			{
				spawn(glm::vec3(Utils::Randomize(-5, 5), 10.f, Utils::Randomize(-5, 5)), glm::vec3(Utils::Randomize(-1, 1), Utils::Randomize(0, 1), Utils::Randomize(-1, 1)));
			}
		}
	}
}

void PhysicsUpdate(float dt) {
	currTime += dt;
	//ps.destroyOldParticles(maxAge);

	/*if (emissionType)
		UpdateFountain(dt);
	else if (!emissionType)
		UpdateCascade(dt);*/

	ps.updateLilSpheres();
	/*ps.updateAge(dt);
	ps.UpdateSpeed(dt);*/

	if (renderSphere) {
		Sphere::updateSphere(glm::vec3(-2, 5, 0), 2.f);
		Sphere::drawSphere();
	}
	/*Capsule::updateCapsule(glm::vec3(3, 3, 0), glm::vec3(3, 7, 0), 1.5f);
	Capsule::drawCapsule();*/

	// TODO: Posar les dades dins la funció i descomentar
	//glm::vec3* forces = mesh.getSpringForces(/*Passar-hi dades de la funció*/);

	// sumar gravetat

	//solver.updateParticles(mesh, forces);

	mesh.UpdateSpeed(dt);
	//mesh.PrintParticlesPos();

	int idx;
	std::vector<glm::vec3> tmpPos(mesh.width * mesh.height);
	for (int row = 0; row < mesh.height; row++) {
		for (int col = 0; col < mesh.width; col++) {
			idx = row * mesh.width + col;
			tmpPos[idx] = mesh.particles[idx].pos;
			//tmpPos[idx] = mesh.nodes[col][row].pos;
			//printf("The particle %i pos is (%f, %f, %f)\n",
			//	idx, tmpPos[idx].x, tmpPos[idx].y, tmpPos[idx].z);
		}
	}

	ClothMesh::updateClothMesh(&(tmpPos[0].x));
	LilSpheres::updateParticles(0, mesh.width * mesh.height, &(tmpPos[0].x));
}



void PhysicsCleanup() {
	//Exemple_PhysicsCleanup();



}