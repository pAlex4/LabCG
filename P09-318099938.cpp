/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Texture tableroTexture;
Texture dadoTexture;


Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Model2_M;
Model Model3_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	unsigned int tableroIndices[] = {
		8, 9, 10,
		11, 10, 9,

		4, 5, 6,
		7, 6, 5,

		0, 2, 4,
		2, 4, 6,

		0, 1, 4,
		1, 4, 5,

		1, 3, 5,
		3, 5, 7,

		2, 3, 6,
		3, 6, 7
	};

	GLfloat tableroVertices[] = {
		//sin textura
		-1.0f,	0.0f,	-1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	0.0f,	-1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-1.0f,	0.0f,	1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	0.0f,	1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,

		-1.0f,	-1.0f,	-1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	-1.0f,	-1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-1.0f,	-1.0f,	1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	-1.0f,	1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,

		//con textura
		-1.0f,	0.0f,	-1.0f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	0.0f,	-1.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-1.0f,	0.0f,	1.0f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		1.0f,	0.0f,	1.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};



	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(tableroVertices, tableroIndices, 96, 36);
	meshList.push_back(obj5);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		//top
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		//bot
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
	};


	//coordenadas
	float X[3] = { 0.0f, 0.951f, 0.588f };
	float Y = 1.0f;
	float Z[3] = { 1.0f, 0.309f, 0.809f };

	//vertices
	float A[3] = { 0.0f, Y, 0.0f };
	float B[3] = { X[0],0.0f, -Z[0] };
	float C[3] = { -X[1], 0.0f, -Z[1] };
	float D[3] = { -X[2], 0.0f, Z[2] };
	float E[3] = { X[2], 0.0f, Z[2] };
	float F[3] = { X[1], 0.0f, -Z[1] };
	float G[3] = { 0.0f, -Y, 0.0f };

	float S = 0.33f;
	float T = 0.5f;
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S			T			NX		NY		NZ
		//top 1
		A[0],	A[1],  A[2],	(0.5 * S),	(2 * T),	1.0f,	-1.0f,	1.0f,
		B[0],	B[1],  B[2],	(0 * S),	(1 * T),	1.0f,	-1.0f,	1.0f,
		C[0],	C[1],  C[2],	(1 * S),	(1 * T),	1.0f,	-1.0f,	1.0f,
		//top 2
		A[0],	A[1],  A[2],	(1.5 * S),	(2 * T),	1.0f,	-1.0f,	-1.0f,
		C[0],	C[1],  C[2],	(1 * S),	(1 * T),	1.0f,	-1.0f,	-1.0f,
		D[0],	D[1],  D[2],	(2 * S),	(1 * T),	1.0f,	-1.0f,	-1.0f,
		//top 3
		A[0],	A[1],  A[2],	(2.5 * S),	(2 * T),	1.0f,	-1.0f,	-1.0f,
		D[0],	D[1],  D[2],	(2 * S),	(1 * T),	1.0f,	-1.0f,	-1.0f,
		E[0],	E[1],  E[2],	(3 * S),	(1 * T),	1.0f,	-1.0f,	-1.0f,
		//top 4
		A[0],	A[1],  A[2],	(1 * S),	(1 * T),	-1.0f,	-1.0f,	-1.0f,
		E[0],	E[1],  E[2],	(0.5 * S),	(0 * T),	-1.0f,	-1.0f,	-1.0f,
		F[0],	F[1],  F[2],	(1.5 * S),	(0 * T),	-1.0f,	-1.0f,	-1.0f,
		//top 5
		A[0],	A[1],  A[2],	(2 * S),	(1 * T),	-1.0f,	-1.0f,	1.0f,
		F[0],	F[1],  F[2],	(1.5 * S),	(0 * T),	-1.0f,	-1.0f,	1.0f,
		B[0],	B[1],  B[2],	(2.5 * S),	(0 * T),	-1.0f,	-1.0f,	1.0f,
		//bot 1
		G[0],	G[1],  G[2],	(0.5 * S),	(0 * T),	1.0f,	1.0f,	1.0f,
		B[0],	B[1],  B[2],	(1 * S),	(1 * T),	1.0f,	1.0f,	1.0f,
		C[0],	C[1],  C[2],	(0 * S),	(1 * T),	1.0f,	1.0f,	1.0f,
		//bot 2
		G[0],	G[1],  G[2],	(1.5 * S),	(0 * T),	1.0f,	1.0f,	-1.0f,
		C[0],	C[1],  C[2],	(2 * S),	(1 * T),	1.0f,	1.0f,	-1.0f,
		D[0],	D[1],  D[2],	(1 * S),	(1 * T),	1.0f,	1.0f,	-1.0f,
		//bot 3
		G[0],	G[1],  G[2],	(2.5 * S),	(0 * T),	0.0f,	1.0f,	-1.0f,
		D[0],	D[1],  D[2],	(3 * S),	(1 * T),	0.0f,	1.0f,	-1.0f,
		E[0],	E[1],  E[2],	(2 * S),	(1 * T),	0.0f,	1.0f,	-1.0f,
		//bot 4
		G[0],	G[1],  G[2],	(1 * S),	(1 * T),	-1.0f,	1.0f,	-1.0f,
		E[0],	E[1],  E[2],	(1.5 * S),	(2 * T),	-1.0f,	1.0f,	-1.0f,
		F[0],	F[1],  F[2],	(0.5 * S),	(2 * T),	-1.0f,	1.0f,	-1.0f,
		//bot 5
		G[0],	G[1],  G[2],	(2 * S),	(1 * T),	-1.0f,	1.0f,	1.0f,
		F[0],	F[1],  F[2],	(2.5 * S),	(2 * T),	-1.0f,	1.0f,	1.0f,
		B[0],	B[1],  B[2],	(1.5 * S),	(2 * T),	-1.0f,	1.0f,	1.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 10 * 3 * 8, 30);
	meshList.push_back(dado);

	//calcAverageNormals(cubo_indices, 30, cubo_vertices, 30, 8, 5);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();

	CrearDado();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	tableroTexture = Texture("Textures/tablero.jpg");
	tableroTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado_10.png");
	dadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	
	Model2_M = Model();
	Model2_M.LoadModel("Models/Master Cycle.obj");
	Model3_M = Model();
	Model3_M.LoadModel("Models/wheel.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	int opcion = 0;
	int opcionAnt = 0;

	float movDado = 16.0f;
	float movDadoOffset = 0.25f;
	float rotDadoX = 0.0f;
	float rotDadoY = 0.0f;
	float rotDadoZ = 0.0f;
	float rotDadoOffset = 5.0f;

	float movCocheX = -65.0f;
	float movCocheZ = 0.0f;
	float rotCoche = 0.0f;
	float rotCocheOffset = 1.9f;
	movOffset = 0.6f;
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;
	bool avanzaX = false;
	bool avanzaZ = false;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//printf("movdado %.2f\n", movDado);
		//printf("deltaTime %.2f\n", deltaTime);
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//caida de dado
		{
			opcion = mainWindow.getmuevex();
			//printf("opc %d\n", opcion);
			if (opcion != opcionAnt) {
				movDado = 12.0f; rotDadoY = 0.0f; rotDadoX = 0.0f;
				opcionAnt = opcion;
			}
			if (movDado >= 3.1f) movDado -= movDadoOffset * deltaTime;
			if (opcion >= 6) rotDadoZ = 180; else rotDadoZ = 0;

			switch (opcion) {
			case 1:
				if (rotDadoY <= 360.0f + (2 * 73.0f)) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 2:
				if (rotDadoY <= 360.0f + 73.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 3:
				if (rotDadoY <= 360.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 4:
				if (rotDadoY <= 360.0f - 73.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 5:
				if (rotDadoY <= 360.0f - (2 * 73.0f)) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 6:
				if (rotDadoY <= 360.0f + 73.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 7:
				if (rotDadoY <= 360.0f + (2 * 73.0f)) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 8:
				if (rotDadoY <= 360.0f - (2 * 73.0f)) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 9:
				if (rotDadoY <= 360.0f - 73.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;

			case 10:
				if (rotDadoY <= 360.0f) rotDadoY += rotDadoOffset * deltaTime;
				else if (rotDadoX <= 50.0f) rotDadoX += rotDadoOffset * deltaTime;
				break;
			}
		}

		//movimiento de Modelo
		if(glfwGetTime() > 10.0f){
		if (avanzaZ) {
			if (movCocheZ < 5)	movCocheZ += movOffset * deltaTime;
			else if (rotCoche < 90 && rotCoche >= 0) rotCoche += rotCocheOffset * deltaTime;
			else avanzaX = true;
			rotllanta += rotllantaOffset * deltaTime;
		}
		else{
			if (movCocheZ > -55) movCocheZ -= movOffset * deltaTime;
			else if (rotCoche < 270 && rotCoche >= 180) rotCoche += rotCocheOffset * deltaTime;
			else avanzaX = false;
			rotllanta += rotllantaOffset * deltaTime;
		}
		
		if (avanzaX) {
			if (movCocheX < 10)	movCocheX += movOffset * deltaTime;
			else if (rotCoche < 180 && rotCoche >= 90) rotCoche += rotCocheOffset * deltaTime;
			else avanzaZ = false;
			rotllanta += rotllantaOffset * deltaTime;
		}
		else {
			if (movCocheX > -65) movCocheX -= movOffset * deltaTime;
			else if (rotCoche < 360 && rotCoche >= 270) rotCoche += rotCocheOffset * deltaTime;
			else avanzaZ = true;
			rotllanta += rotllantaOffset * deltaTime;
		}
		if (rotCoche > 360) rotCoche = 0.0f;
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//tablero
		float escala = 25.0f;
		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 1.01f, 0.0f));
		model = glm::scale(model, glm::vec3(escala, 1.0f, escala));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		tableroTexture.UseTexture();
		meshList[4]->RenderMesh();

		//dado
		escala = 3.0f;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, movDado, 0.0f));
		model = glm::scale(model, glm::vec3(escala, escala, escala));
		model = glm::rotate(model, rotDadoX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotDadoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotDadoZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[5]->RenderMesh();

		//modelo importado
		escala = 0.1f;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f + movCocheX, -1.0f, 25.0f + movCocheZ));
		model = glm::rotate(model, (0 * toRadians) + (rotCoche * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		modelaux = model;
		model = glm::scale(model, glm::vec3(escala, escala, escala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Model2_M.RenderModel();

		//modelos adicionlaes
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, -2.9f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escala, escala, escala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Model3_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 3.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escala, escala, escala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Model3_M.RenderModel();

		//Instancia del carro 
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();*/


		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));*/
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
