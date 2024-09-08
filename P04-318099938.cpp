/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere 
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh *cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res,float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res+2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh *cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh *piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(30, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();
	
	

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelaux1(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelaux2(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía

	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
				
		//para reiniciar la matriz de modelo con valor de la matriz identidad
		model = glm::mat4(1.0);

		// //Traslación inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -6.0f));

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		
		//CONEJO
		{
		//PARAMETROS:			x,		y,		z,		angulo
		float cuerpo[4]		= { 4.0f,	2.0f,	2.0f,	15.0f };
		
		float pataTraSup[4] = { 0.6f,	1.0f,	0.5f,	30.0f };
		float pataTraInf[4] = { 0.25f,	2.25f,	0.5F,	105.0f};
		
		float pataDelSup[4] = { 0.5f,	1.6f,	0.5f,	5.0f };
		float pataDelInf[4] = { 0.4,	0.7f,	0.5f,	80.0f };
		
		float cabeza[4]		= { 1.0f,	1.6f,	1.5f,	15.0f };
		
		float orejaInf[4]	= { 0.25f,	0.75f,	0.5f,	25.0f };
		float orejaSup[4]	= { 0.25f,	1.5f,	0.5f,	95.0f };
		
		float nariz[4]		= { 0.15f,	0.35f,	0.2f,	90.0f };

		float escala = 0.25f;

		float colorEsfera = 0.0f;

		//CUERPO
		{
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-cuerpo[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		
		modelaux1 = model;//save

		model = glm::scale(model, glm::vec3(cuerpo[0], cuerpo[1], cuerpo[2]));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		
		meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

		model = modelaux1;//load
		}

		//PATAS TRASERAS
		for (int i = -1; i < 2; i++) {
			//ARTICULACION pataTraSup
			{
				model = glm::translate(model, glm::vec3(cuerpo[0] * (2.0f / 6.0f), -cuerpo[1] * (1.0f / 6.0f), i * cuerpo[2] / 2));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(pataTraSup[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			//ESFERA pataTraSup
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala, escala, escala));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//PATA pataTraSup
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataTraSup[1] / 2, 0.0f));
				model = glm::scale(model, glm::vec3(pataTraSup[0], pataTraSup[1], pataTraSup[2]));

				color = glm::vec3(0.26f, 0.26f, 0.26f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

				model = modelaux2;//load
			}

			//ARTICULACION pataTraInf
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataTraSup[1], 0.0f));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-pataTraInf[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			//ESFERA pataTraInf
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala, escala, escala));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//PATA pataTraInf
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataTraInf[1] / 2, 0.0f));
				model = glm::scale(model, glm::vec3(pataTraInf[0], pataTraInf[1], pataTraInf[2]));

				color = glm::vec3(0.6f, 0.6f, 0.6f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

				model = modelaux2;//load
			}

			model = modelaux1;//load
			i++;
		}
		

		//PATAS DElANTERO
		for (int i = -1; i < 2; i++) {
			//ARTICULACION pataDelSup
			{
				model = glm::translate(model, glm::vec3(-cuerpo[0] * (2.0f / 6.0f), -cuerpo[1] * (1.0f / 6.0f), i * cuerpo[2] / 2));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(pataDelSup[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			//ESFERA pataDelSup
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala, escala, escala));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//PATA pataDelSup
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataDelSup[1] / 2, 0.0f));
				model = glm::scale(model, glm::vec3(pataDelSup[0], pataDelSup[1], pataDelSup[2]));

				color = glm::vec3(0.26f, 0.26f, 0.26f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

				model = modelaux2;//load
			}

			//ARTICULACION pataDelInf
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataDelSup[1], 0.0f));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-pataDelInf[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			//ESFERA pataDelInf
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala, escala, escala));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//PATA pataDelInf
			{
				model = glm::translate(model, glm::vec3(0.0f, -pataDelInf[1] / 2, 0.0f));
				model = glm::scale(model, glm::vec3(pataDelInf[0], pataDelInf[1], pataDelInf[2]));

				color = glm::vec3(0.6f, 0.6f, 0.6f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

				model = modelaux2;//load
			}


			model = modelaux1;//load
			i++;
		}

		//COLA
		{
		model = glm::translate(model, glm::vec3(cuerpo[0] / 2, -cuerpo[1] / 4, 0.0f));
		model = glm::scale(model, glm::vec3(0.46f, 0.46f, 0.46f));

		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux1;//load
		}

		//CABEZA
		{
		model = glm::translate(model, glm::vec3(-cuerpo[0] / 2, cuerpo[1] / 2, 0.0f));
		model = glm::rotate(model, glm::radians(cabeza[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.2f, 0.2f, 0.0f));

		modelaux1 = model;//save

		model = glm::scale(model, glm::vec3(cabeza[0], cabeza[1], cabeza[2]));

		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

		model = modelaux1;//load
		}

		//OREJAS
		for (int i = -1; i < 2; i++) {
			//ARTICULACION inf
			{
				model = glm::translate(model, glm::vec3(cabeza[0] / 2, cabeza[1] / 2, i * cabeza[2] / 4));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-orejaInf[3]), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-orejaInf[3]), glm::vec3(i * -60.0f, 0.0f, 0.0f));
			}

			//ESFERA inf
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala / 2, escala / 2, escala / 2));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//OREJA inf
			{
			model = glm::translate(model, glm::vec3(0.0f, orejaInf[1] / 2, 0.0f));
			model = glm::scale(model, glm::vec3(orejaInf[0], orejaInf[1], orejaInf[2]));

			color = glm::vec3(0.8f, 0.8f, 0.8f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

			model = modelaux2;//load
			}

			//ARTICULACION sup
			{
				model = glm::translate(model, glm::vec3(0.0f, orejaInf[1], 0.0f));

				model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-orejaSup[3]), glm::vec3(0.0f, 0.0f, 1.0f));
			}

			//ESFERA sup
			{
				modelaux2 = model;//save

				model = glm::scale(model, glm::vec3(escala / 2, escala / 2, escala / 2));

				color = glm::vec3(colorEsfera, colorEsfera, colorEsfera);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();

				model = modelaux2;//load
			}

			//OREJA sup
			{
				model = glm::translate(model, glm::vec3(0.0f, orejaSup[1] / 2, 0.0f));
				model = glm::scale(model, glm::vec3(orejaSup[0], orejaSup[1], orejaSup[2]));

				modelaux2 = model;//save

				color = glm::vec3(0.3f, 0.3f, 0.3f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

				model = modelaux2;//load
			}

			model = modelaux1;//load
			i++;
		}

		//NARIZ
		{
		model = glm::translate(model, glm::vec3(-cabeza[0] / 2, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(nariz[3]), glm::vec3(0.0f, 0.0f, 1.0f));

		model = glm::translate(model, glm::vec3(0.0f, nariz[2] / 2, 0.0f));
		model = glm::scale(model, glm::vec3(nariz[0], nariz[1], nariz[2]));

		color = glm::vec3(0.26f, 0.26f, 0.26f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular
		}
		}

		//GRUA
		{
		//para reiniciar la matriz de modelo con valor de la matriz identidad
		model = glm::mat4(1.0);

		// //Traslaci?n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -6.0f));

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		modelaux = model;//save

		//BASE

		model = glm::scale(model, glm::vec3(4.0f, 2.0f, 1.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[0]->RenderMesh(); //dibuja cubo, pir?mide triangular, pir?mide base cuadrangular

		//LLANTA 1

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(-1.25f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 2.0f, 0.7f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(0.25f, 0.25f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[2]->RenderMeshGeometry(); //dibuja cubo, pir?mide triangular, pir?mide base cuadrangular

		//LLANTA 2

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(1.25f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 2.0f, 0.7f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(0.25f, 0.25f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[2]->RenderMeshGeometry(); //dibuja cubo, pir?mide triangular, pir?mide base cuadrangular

		model = modelaux;//load

		//ESFERITA DE ARTICULACION 1

		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		/*
		modelaux = model;//save

		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		//sp.render();
		*/

		//BRAZO 1

		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));

		modelaux = model;//save

		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[0]->RenderMesh(); //dibuja cubo, pir?mide triangular, pir?mide base cuadrangular


		//ESFERITA DE ARTICULACION 2

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));

		modelaux = model;//save

		/*
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();
		*/

		//BRAZO 2
		model = modelaux;//load

		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		modelaux = model;//save

		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[0]->RenderMesh(); //dibuja cubo y pir?mide triangular


		//ESFERITA DE ARTICULACION 3

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));

		modelaux = model;//save

		/*
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();
		*/

		//BRAZO 3

		model = modelaux;//load

		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		modelaux = model;//save

		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[0]->RenderMesh(); //dibuja cubo y pir?mide triangular

		//ESFERITA DE ARTICULACION 4

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));

		modelaux = model;//save

		/*
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();
		*/

		//CABINA

		model = modelaux;//load

		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		meshList[0]->RenderMesh(); //dibuja cubo y pir?mide triangular
		}



		/*
		//BRAZO 1
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));

		modelaux = model;//save
		
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));		
		
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		
		meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular

		
		//ESFERITA DE ARTICULACION 2
		
		model = modelaux;//load
		
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		
		modelaux = model;//save
		
		/*
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();

		//BRAZO 2
		model = modelaux;//load
		
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		modelaux = model;//save

		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular
		*/

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

	
		