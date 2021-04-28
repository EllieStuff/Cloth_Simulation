#pragma once
#include "ParticleSystem.h"

namespace ClothMesh {
	extern const int numCols;
	extern const int numRows;
	extern const int numVerts;

	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

class Mesh : public ParticleSystem {
private:
	struct Spring {
		int p1_idx, p2_idx;
		float K_ELASTICITY, K_DAMPING;
		float REST_DIST;

		Spring(int _p1_idx, int _p2_idx, float _REST_DIST, float _K_ELASTICITY = 10000.0f, float _K_DAMPING = 0.1f)
			: p1_idx(_p1_idx), p2_idx(_p2_idx), REST_DIST(_REST_DIST), K_ELASTICITY(_K_ELASTICITY), K_DAMPING(_K_DAMPING) {
		};



	};

	int getIndex(int row, int col) {
		return row * width + col;
	};

public:
	int width, height;
	float rowDist = 0.2f, colDist = -0.2f;
	glm::vec3 margin = glm::vec3(-4.f, 8, 4.5f);	//Nota: Canviar z per a provar colisio amb parets
	std::vector<Spring> springs;
	//std::vector<std::vector<Particle>> nodes;
	//int particleSpawnerCounter = 0;

	Mesh() : width(ClothMesh::numCols), height(ClothMesh::numRows) {};
	Mesh(int _width, int _height) {
		width = _width;
		height = _height;
		InitParticles(_width * _height);
		//nodes = std::vector<std::vector<Particle>>(_height);
		// initialize mesh positions
		//int idx;
		float rowStretchRestDist = abs(rowDist);
		float colStretchRestDist = abs(colDist);
		float rowShearRestDist = sqrt(pow(rowStretchRestDist, 2) + pow(rowStretchRestDist, 2));
		float colShearRestDist = sqrt(pow(colStretchRestDist, 2) + pow(colStretchRestDist, 2));
		int blendMargin = 2;
		float rowBlendRestDist = rowStretchRestDist * blendMargin;
		float colBlendRestDist = colStretchRestDist * blendMargin;

		for (int row = 0; row < height; row++) {
			//nodes[row] = std::vector<Particle>(_width);

			//margin = glm::vec3(Utils::Randomize(0, 10), Utils::Randomize(-5, 5), Utils::Randomize(0, 10));
			for (int col = 0; col < width; col++) {
				int idx = getIndex(row, col);
				if (idx <= ClothMesh::numVerts)
				{
					particles[idx].pos = glm::vec3(row * rowDist + margin.x, margin.y, col * colDist + margin.z);
				}
				else if (idx > ClothMesh::numVerts)
				{
					particles[idx].pos = glm::vec3(-3, 7, 0.0f);
				}
				
				particles[idx].prevPos = particles[idx].pos;

				// Structurals/Stretch Springs
				if (row < height - 1) {
					springs.push_back(Spring(idx, getIndex(row + 1, col), rowStretchRestDist));
				}
				if (col < width - 1) {
					springs.push_back(Spring(idx, getIndex(row, col + 1), colStretchRestDist));
				}
				// Shear Springs
				if (row < height - 1 && col < width - 1) {
					springs.push_back(Spring(idx, getIndex(row + 1, col + 1), rowShearRestDist));
				}
				if (row > 0 && col < width - 1) {
					springs.push_back(Spring(idx, getIndex(row - 1, col + 1), colShearRestDist));
				}
				// Flexion/Bending Springs -- ToDo: is this okey?
				if (row < height - blendMargin) {
					springs.push_back(Spring(idx, getIndex(row + blendMargin, col), rowBlendRestDist));
				}
				if (col < width - blendMargin) {
					springs.push_back(Spring(idx, getIndex(row, col + blendMargin), colBlendRestDist));
				}



				//SpawnMeshParticle(glm::vec3(row * 0.3f, col * 0.2f, 0.0f));
				//nodes[row][col].pos = glm::vec3(row * rowDist + 1.5f, col * colDist + 5, 0.0f);
			}
		}
	}

	/*void SpawnMeshParticle(glm::vec3 initPos = glm::vec3(0, 0, 0), glm::vec3 initVelocity = glm::vec3(0, 0, 0)) {
		UpdateParticle(particleSpawnerCounter, initPos, initVelocity);

		spawnParticle(initPos, initVelocity);

		particleSpawnerCounter++;
	}*/

	void Mesh::UpdateSpeed(float dt)
	{
		for (int i = 0; i < 3; i++) {
			for (int i = 0; i < springs.size(); i++) {
				Spring currSpring = springs[i];
				float p2p1dist = glm::distance(particles[currSpring.p1_idx].pos, particles[currSpring.p2_idx].pos);
				glm::vec3 p2p1Vector = particles[currSpring.p1_idx].pos - particles[currSpring.p2_idx].pos;
				glm::vec3 p2p1NormalizedVector = glm::normalize(p2p1Vector);

				// ToDo Averiguar v1 - v2
				glm::vec3 force = -(currSpring.K_ELASTICITY * (p2p1dist - currSpring.REST_DIST)
					+ glm::dot(currSpring.K_DAMPING * (particles[currSpring.p1_idx].speed - particles[currSpring.p2_idx].speed), p2p1NormalizedVector))
					* p2p1NormalizedVector;

				particles[currSpring.p1_idx].totalForce += force;
				particles[currSpring.p2_idx].totalForce += -force;
				//if (i == 14)
				//{
				//	//printf("%f\n", p2p1dist - currSpring.REST_DIST);
				//	printf("X: %f - ", force.x);
				//	printf("Y: %f - ", force.y);
				//	printf("Z: %f\n\n", force.z);
				//}
			}
		}

		for (int i = 0; i < currParticles; i++) {
			//particles[i].prevPos = particles[i].pos;

			// Euler
			/*particles[i].acc = gravity / mass;
			particles[i].speed += particles[i].acc * dt;
			particles[i].pos += particles[i].speed * dt;*/

			if (i == 0) {
				i = i;
			}

			// Verlet
			if (i != 0 && i != currParticles - width) {
				Particle currParticle = particles[i];

				particles[i].acc = (particles[i].totalForce + gravity) / mass;
				particles[i].totalForce = glm::vec3(0, 0, 0);
				particles[i].prevPos = currParticle.pos;
				particles[i].pos = currParticle.pos + (currParticle.pos - currParticle.prevPos) + particles[i].acc * pow(dt/8, 2.0f);
				particles[i].speed += (particles[i].pos - particles[i].prevPos)/dt;


				CheckCollisions(i);
			}
		}
	}

	glm::vec3* getSpringForces(
		float k_elasticity,
		float k_damping,
		float rest_distance, // L
		glm::vec3 p1, glm::vec3 p2,
		glm::vec3 v1, glm::vec3 v2
	) {
		// Force applied to p1 due to a spring from p1 to p2
		glm::vec3* springForces;

		// TODO
		// ...

		return springForces;
	}

	void PrintParticlesPos() {
		printf("\n\nParticles positions:\n");
		for (int row = 0; row < width; row++) {
			for (int col = 0; col < height; col++) {
				printf("The particle %i pos is (%f, %f, %f)\n",
					getIndex(row, col), particles[getIndex(row, col)].pos.x, particles[getIndex(row, col)].pos.y, particles[getIndex(row, col)].pos.z);
				//printf("The particle %i pos is (%f, %f, %f)\n",
				//	getIndex(row, col), nodes[row][col].pos.x, nodes[row][col].pos.y, nodes[row][col].pos.z);
			}
		}
	}

};