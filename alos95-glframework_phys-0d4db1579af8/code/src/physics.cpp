#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\glm.hpp>
#include "ParticleSystem.h"
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


namespace ClothMesh {
	extern const int numCols;
	extern const int numRows;

	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

class Mesh : public ParticleSystem {
private:
	struct Spring {
		Mesh* m;
		int p1_idx, p2_idx;
		float K_ELASTICITY = 0.8f, K_DAMPING = 0.4f;
		float REST_DIST = 0.5f;

		Spring(Mesh _m, int _p1_idx, int _p2_idx, float _K_ELASTICITY, float _K_DAMPING, float _REST_DIST)
			: m(&_m), // <---- es correcte?!?!?!?! <:'O
			p1_idx(_p1_idx), p2_idx(_p2_idx), K_ELASTICITY(_K_ELASTICITY), K_DAMPING(_K_DAMPING), REST_DIST(_REST_DIST) {
		};



	};

	int getIndex(int row, int col) {
		return row * width + col;
	};

public:
	int width, height;
	std::vector<std::vector<Particle>> nodes;
	//int particleSpawnerCounter = 0;

	Mesh() : width(ClothMesh::numRows), height(ClothMesh::numCols) {};
	Mesh(int _width, int _height) {
		width = _width;
		height = _height;
		InitParticles(_width * _height);
		nodes = std::vector<std::vector<Particle>>(_height);

		// initialize mesh positions
		for (int row = 0; row < nodes.size(); row++) {
			nodes[row] = std::vector<Particle>(_width);

			for (int col = 0; col < nodes[row].size(); col++) {
				//int a = getIndex(row, col);
				//particles[getIndex(row, col)].pos = glm::vec3(row * 0.3f, col * 0.2f, 0.0f);
				//SpawnMeshParticle(glm::vec3(row * 0.3f, col * 0.2f, 0.0f));
				nodes[row][col].pos = glm::vec3(row * 0.3f, col * 0.2f, 0.0f);
			}
		}


	}

	/*void SpawnMeshParticle(glm::vec3 initPos = glm::vec3(0, 0, 0), glm::vec3 initVelocity = glm::vec3(0, 0, 0)) {
		UpdateParticle(particleSpawnerCounter, initPos, initVelocity);

		spawnParticle(initPos, initVelocity);

		particleSpawnerCounter++;
	}*/

	glm::vec3 *getSpringForces(
		float k_elasticity, 
		float k_damping, 
		float rest_distance, // L
		glm::vec3 p1, glm::vec3 p2,
		glm::vec3 v1, glm::vec3 v2
	) {
		// Force applied to p1 due to a spring from p1 to p2
		glm::vec3 *springForces;

		// TODO
		// ...

		return springForces;
	}

	void PrintParticlesPos() {
		printf("\n\nParticles positions:\n");
		for (int row = 0; row < nodes.size(); row++) {
			for (int col = 0; col < nodes[row].size(); col++) {
				//printf("The particle %i pos is (%f, %f, %f)\n", 
				//	getIndex(row, col), particles[getIndex(row, col)].pos.x, particles[getIndex(row, col)].pos.y, particles[getIndex(row, col)].pos.z);
				printf("The particle %i pos is (%f, %f, %f)\n",
					getIndex(row, col), nodes[row][col].pos.x, nodes[row][col].pos.y, nodes[row][col].pos.z);
			}
		}
	}

};

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
	//renderSphere = renderCapsule = true;
	//Sphere::setupSphere(glm::vec3(-2, 5, 0), 2.f);
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

	/*Sphere::updateSphere(glm::vec3(-2, 5, 0), 2.f);
	Sphere::drawSphere();
	Capsule::updateCapsule(glm::vec3(3, 3, 0), glm::vec3(3, 7, 0), 1.5f);
	Capsule::drawCapsule();*/

	// TODO: Posar les dades dins la funció i descomentar
	//glm::vec3* forces = mesh.getSpringForces(/*Passar-hi dades de la funció*/);

	// sumar gravetat

	//solver.updateParticles(mesh, forces);

	//mesh.UpdateSpeed(dt);
	mesh.PrintParticlesPos();

	std::vector<glm::vec3> tmpPos(mesh.width * mesh.height);
	for (int row = 0; row < mesh.width; row++) {
		for (int col = 0; col < mesh.height; col++) {
			int idx = row * mesh.width + col;
			tmpPos[idx] = mesh.nodes[col][row].pos;
		}
	}

	ClothMesh::updateClothMesh(&(tmpPos[0].x));
	LilSpheres::updateParticles(0, mesh.width * mesh.height, &(tmpPos[0].x));
}



void PhysicsCleanup() {
	//Exemple_PhysicsCleanup();



}