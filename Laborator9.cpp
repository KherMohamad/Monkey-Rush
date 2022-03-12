#include "Laborator9.h"

#include <vector>
#include <string>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>

using namespace std;

Laborator9::Laborator9(): isQuad(false), isCoconut(false)
{
}

Laborator9::~Laborator9()
{
}

glm::mat4 Laborator9::RotateOX(float radians)
{
	GLfloat c = cos(radians);
	GLfloat s = sin(radians);

	return glm::mat4(
		1.f, 0.f, 0.f, 0.f,
		0.f, c, s, 0.f,
		0.f, -s, c, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

glm::mat4 Laborator9::RotateOY(float radians)
{
	GLfloat c = cos(radians);
	GLfloat s = sin(radians);

	return glm::mat4(
		c, 0.f, -s, 0.f,
		0.f, 1.f, 0.f, 0.f,
		s, 0.f, c, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

void Laborator9::generatePlatforms(vector<Platform>& platforms) {
	float pozX;
	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			pozX = -2.5f;
		}
		else if (i == 1) {
			pozX = 0.f;
		}
		else {
			pozX = 2.5f;
		}
		int choice = rand();
		int pozY = rand() % (int)(10 * JUMPHEIGHT / 3);
		float truePozY = (float)pozY / 10;
		int pozZ = -1 * rand() % 4;
		float truePozZ = (float)pozZ / 10 + PLATFORMSTARTINGZ;

		glm::vec3 position = glm::vec3(pozX, truePozY, truePozZ);
		char color;
		switch (choice % 8) {
		case 0:
			color = 'r';
			break;
		case 1:
			color = 'y';
			break;
		case 2:
			color = 'o';
			break;
		case 3:
			color = 'g';
			break;
		default:
			color = 'b';
		}
		bool hasTree = false;
		bool hasPoint = false;
		bool hasWall = false;
		choice = rand() % 8;
		if (choice % 4 == 0) {
			hasTree = true;
		}
		if (choice == 1) {
			hasPoint = true;
		}
		if (choice == 2) {
			hasWall = true;
		}
		int nrTreeLevels = 0;
		int placementX = 0;
		float placementZ = 0.f;
		float rotAngle = 0.f;
		if (hasTree) {
			nrTreeLevels = rand() % 5 + 1;
			choice = rand() % 2;
			if (choice == 0) {
				placementX = 1;
			}
			else {
				placementX = -1;
			}
			placementZ = (float)((rand() % 40 - 20) / 10);
			rotAngle = (float)((rand() % 31 - 15) / 10);
		}
		
		Platform* platform = new Platform(position, color, false, hasTree, hasPoint, hasWall,
			nrTreeLevels, placementX, placementZ, rotAngle);
		platforms.push_back(*platform);

	}
}

bool Laborator9::isOnPlatform(Platform platform, Player player) {
	if (player.getPosition().y - SPHERERADIUS < platform.getPosition().y) {
		return false;
	}
	if (player.getPosition().y - SPHERERADIUS > platform.getPosition().y + SCALEY / 2) {
		return false;
	}
	if (player.getPosition().x >= (platform.getPosition().x - SCALEX / 2) &&
		player.getPosition().x <= (platform.getPosition().x + SCALEX / 2) &&
		player.getPosition().z >= (platform.getPosition().z - SCALEZ / 2) &&
		player.getPosition().z <= (platform.getPosition().z + SCALEZ / 2)) {
		return true;
	}
	return false;
}

bool hitWall(Platform platform, Player player) {
	int pozZ = player.getPosition().z - SPHERERADIUS; 
	if ((pozZ >= (platform.getPosition().z - 0.25f)) &&
		(pozZ <= (platform.getPosition().z + 0.25f)) &&
		(player.getPosition().x >= (platform.getPosition().x - 1.f)) &&
		(player.getPosition().x <= (platform.getPosition().x + 1.f)) &&
		(player.getPosition().y <= (platform.getPosition().y + 1.f))) {
		return true;
	}
	return false;
}

bool tookCoconut(Platform platform, Player player) {
	float dist;
	glm::vec3 coconutCenter = platform.getPosition() + glm::vec3(0, 0.5f, 0);
	dist = sqrtf(pow(coconutCenter.x - player.getPosition().x, 2) +
		pow(coconutCenter.y - player.getPosition().y, 2) +
		pow(coconutCenter.z - player.getPosition().z, 2));
	if (dist <= (SPHERERADIUS + (0.5f * SPHERERADIUS))) {
		return true;
	}
	return false;
}

Mesh* Laborator9::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

void Laborator9::manageExtraPlatforms(vector<Platform>& platforms) {
	if (platforms.size() == 0) {
		return;
	}
	for (int i = 0; i < platforms.size(); i++) {
		if (platforms[i].getPosition().z >= 3.f) {
			platforms.erase(platforms.begin() + i);
		}
	}
}


void Laborator9::Init()
{
	const string textureLoc = "Source/Laboratoare/Laborator9/Textures/";

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "grass_bilboard.png").c_str(), GL_REPEAT);
		mapTextures["grass"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "crate.jpg").c_str(), GL_REPEAT);
		mapTextures["crate"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "earth.png").c_str(), GL_REPEAT);
		mapTextures["earth"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Models/Vegetation/Bamboo/bamboo.png", GL_REPEAT);
		mapTextures["bamboo"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/OIP.jfif", GL_REPEAT);
		mapTextures["wood"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/bananas.png", GL_REPEAT);
		mapTextures["monkey"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/dirtText.jpg", GL_REPEAT);
		mapTextures["dirt"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/tree.png", GL_REPEAT);
		mapTextures["treeLeaves"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/wall.png", GL_REPEAT);
		mapTextures["brickWall"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/coconut.jpg", GL_REPEAT);
		mapTextures["coconut"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/fallTreeLeaves.jpg", GL_REPEAT);
		mapTextures["fallTree"] = texture;
	}


	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/ohChristmasstree.png", GL_REPEAT);
		mapTextures["winterTree"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Textures/springTree.jpg", GL_REPEAT);
		mapTextures["springTree"] = texture;
	}

	{
		mapTextures["random"] = CreateRandomTexture(32, 32);
	}

	// Load meshes
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("bamboo");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Vegetation/Bamboo/", "bamboo.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	// Create a simple quad
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(-PYRAMIDWIDTH / 2, -PYRAMIDHEIGHT / 2, PYRAMIDWIDTH / 2), //front left vertex
			glm::vec3(PYRAMIDWIDTH / 2, -PYRAMIDHEIGHT / 2, PYRAMIDWIDTH / 2), //front right vertex
			glm::vec3(-PYRAMIDWIDTH / 2, -PYRAMIDHEIGHT / 2, -PYRAMIDWIDTH / 2),//back left vertex
			glm::vec3(PYRAMIDWIDTH / 2, -PYRAMIDHEIGHT / 2, -PYRAMIDWIDTH / 2), //back right vertex
			glm::vec3(0.0f, PYRAMIDHEIGHT / 2, 0.0f)
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0)
		};

		// TODO : Complete texture coordinates for the square
		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 0.0f),   //coltul din stanga jos
			glm::vec2(0.0f, 1.0f),  //coltul din stanga sus
			glm::vec2(1.0f, 1.0f),  //coltul din dreapta sus
			glm::vec2(1.0f, 0.0f)  //coltul din dreapta jos
		};

		vector<unsigned short> indices =
		{
			4, 1, 0,
			4, 2, 0,
			4, 2, 3,
			4, 3, 1,
			0, 2, 1,
			1, 2, 3
		};

		Mesh* mesh = new Mesh("pyramid");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0, 0, 0), 
			glm::vec3(1, 0, 0), 
			glm::vec3(1, 1, 0), 
			glm::vec3(0, 1, 0), 
			glm::vec3(0, 0, 1), 
			glm::vec3(1, 0, 1), 
			glm::vec3(1, 1, 1), 
			glm::vec3(0, 1, 1)
		};

		vector<glm::vec3> normals
		{ 
			glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(0, 1, 1)
		};

		// TODO : Complete texture coordinates for the square
		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 0.0f),   //coltul din stanga jos
			glm::vec2(0.0f, 1.0f),  //coltul din stanga sus
			glm::vec2(1.0f, 1.0f),  //coltul din dreapta sus
			glm::vec2(1.0f, 0.0f)  //coltul din dreapta jos
		};

		vector<unsigned short> indices =
		{
			0, 5, 1,	
			0, 4, 5,	
			0, 7, 4,	
			0, 3, 7,
			3, 6, 7,
			3, 2, 6,
			2, 5, 6,
			2, 1, 5,
			0, 1, 2,
			0, 2, 3,
			4, 6, 5,
			4, 7, 6
		};

		Mesh* mesh = new Mesh("trunk");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}
		

	{
		Mesh* mesh = CreateSquare("innerBar", glm::vec3(0, 0, 0), 1, glm::vec3(1, 0, 0), true);
		meshes["innerBar"] = mesh;

		mesh = CreateSquare("outerBar", glm::vec3(0, 0, 0), 1, glm::vec3(0, 0, 0), true);
		meshes["outerBar"] = mesh;
	}

	platformSpeed = 0.5f;
	jumpSpeed = 2.4f;
	fallSpeed = -1.2f;
	jumpDist = 0.f;
	jumpState = 'n';
	score = 0.f;
	isThirdPerson = false;
	cameraFlipped = false;
	platformOn = -1;
	season = 0;
	seasonStart = 0.f;

	player = new Player(glm::vec3(0, SPHERERADIUS, 0), MAXFUEL, 0.f);
	std::srand(time(0));
	Platform* basePlatform = new Platform(glm::vec3(0, 0, 0), 'n', false, false, false, false, 0, 0, 0.f, 0);
	platforms.push_back(*basePlatform);
	generatePlatforms(platforms);


	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderTema3");
		shader->AddShader("Source/Laboratoare/Laborator9/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator9/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Laborator9::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);	
}

void Laborator9::Update(float deltaTimeSeconds)
{
	bool isOnSpecialPlatform = false;

	if (Engine::GetElapsedTime() - seasonStart >= 12.f) {
		season++;
		season %= 4;
		seasonStart = Engine::GetElapsedTime();
	}
	

	
	if (player->getPosition().y < -3.f) {
		cout << "GAME OVER!" << endl << "FINAL SCORE: " << (int)score * 100 << endl;
		this->Exit();
	}

	
	score += platformSpeed * deltaTimeSeconds;



	if (platforms[platforms.size() - 1].getPosition().z >= 0.f) {
		generatePlatforms(platforms);
	}
	manageExtraPlatforms(platforms);

		for (int i = 0; i < platforms.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, platforms[i].getPosition());
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.25f, 4));
			platforms[i].setPosition(platforms[i].getPosition() + glm::vec3(0, 0, platformSpeed * deltaTimeSeconds));

			// Attention! The RenderMesh function overrides the usual RenderMesh that we used until now
			// It uses the viewMatrix from Laborator::Camera instance and the local projectionMatrix
			if (isOnPlatform(platforms[i], *player)) {
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema3"], modelMatrix, mapTextures["random"], nullptr);
				
			}
			else {
				
					RenderSimpleMesh(meshes["box"], shaders["ShaderTema3"], modelMatrix, mapTextures["dirt"], nullptr);
				
			}
			if (platforms[i].getHasTree()) {
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, platforms[i].getPosition() +
					glm::vec3(0.75f * platforms[i].getPlacementX() , 0, platforms[i].getPlacementZ()));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 2.f, 0.2f));
				RenderSimpleMesh(meshes["trunk"], shaders["ShaderTema3"], modelMatrix, mapTextures["wood"], nullptr);
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, platforms[i].getPosition() +
					glm::vec3(0.75f * platforms[i].getPlacementX() + 0.05f, 2.3f, platforms[i].getPlacementZ()));
				float translationFactor = PYRAMIDHEIGHT - 0.5f;
				modelMatrix *= RotateOY(platforms[i].getRotAngle());
				for (int j = 0; j < platforms[i].getNrTreeLevels(); j++) {
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.75f, 0.75f));
					translationFactor *= 0.75f;
					switch (season) {
					case 0:
						RenderSimpleMesh(meshes["pyramid"], shaders["ShaderTema3"], modelMatrix, mapTextures["treeLeaves"], nullptr);
						break;
					case 1:
						RenderSimpleMesh(meshes["pyramid"], shaders["ShaderTema3"], modelMatrix, mapTextures["fallTree"], nullptr);
						break;
					case 2:
						RenderSimpleMesh(meshes["pyramid"], shaders["ShaderTema3"], modelMatrix, mapTextures["winterTree"], nullptr);
						break;
					case 3:
						RenderSimpleMesh(meshes["pyramid"], shaders["ShaderTema3"], modelMatrix, mapTextures["springTree"], nullptr);
						break;
					}
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, translationFactor, 0));
				}
			}
			if (platforms[i].getHasWall()) {
				if (hitWall(platforms[i], *player)) {
					cout << "MONKEY HIT WALL!" << endl << "FINAL SCORE: " << (int)score * 100 << endl;
					this->Exit();
				}
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, platforms[i].getPosition() + glm::vec3(0, 0.6f, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 1, 0.5f));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema3"], modelMatrix, mapTextures["brickWall"], nullptr);
			}
			if (platforms[i].getHasPoint()) {
				if (tookCoconut(platforms[i], *player)) {
					score += 1.f;
					platforms[i].setHasPoint(false);
				}
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, platforms[i].getPosition() + glm::vec3(0, 0.5f, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.5f));
				isCoconut = true;
				RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema3"], modelMatrix, mapTextures["coconut"], nullptr);
				isCoconut = false;
			}
		}

		isOnAnyPlatform = false;
	
		for (int i = 0; i < platforms.size(); i++) {
			isOnAnyPlatform |= isOnPlatform(platforms[i], *player);
			if (isOnPlatform(platforms[i], *player)) {
				platformOn = i;
			}
		}

		if (!isOnAnyPlatform && player->getPosition().y > -3.f && jumpState!= 'u') {
			player->setPosition(player->getPosition() + glm::vec3(0, fallSpeed * deltaTimeSeconds, 0));
		}
		if (jumpState == 'u' && ((player->getPosition().y - jumpStartY) <= player->getJumpHeight())) {
			player->setPosition(player->getPosition() + glm::vec3(0, jumpSpeed * deltaTimeSeconds, 0));
		}
		if (player->getPosition().y > player->getJumpHeight() + jumpStartY) {
			jumpState = 'd';
		}
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, player->getPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f));
		if (cameraFlipped) {
			modelMatrix *= RotateOX(ROTATIONDELTA);
		}
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema3"], modelMatrix, mapTextures["monkey"], nullptr);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-4, 2.9f, -1.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(player->getFuelLeft() * 0.5f, 0.2f, 0));

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-4, 2.9f, -1.f));
		
		
		
		/*RenderMesh(meshes["innerBar"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-4, 2.9f, -1.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(player->getFuelLeft() * 0.5f, 0.25f, 0));

		RenderMesh(meshes["outerBar"], shaders["VertexColor"], modelMatrix);*/
}

void Laborator9::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator9::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform1i(glGetUniformLocation(shader->program, "is_quad"), isQuad);
	glUniform1i(glGetUniformLocation(shader->program, "is_coconut"), isCoconut);
	if (isCoconut) {
		glUniform1f(glGetUniformLocation(shader->program, "time_elapsed"), (float)Engine::GetElapsedTime());
	}

	if (texture1)
	{
		//TODO : activate texture location 0
		//TODO : Bind the texture1 ID
		//TODO : Send texture uniform value
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
	}

	if (texture2)
	{
		//TODO : activate texture location 1
		//TODO : Bind the texture2 ID
		//TODO : Send texture uniform value
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

Texture2D* Laborator9::CreateRandomTexture(unsigned int width, unsigned int height)
{
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];
	unsigned int colorOptions = 1000;

	// TODO: generate random texture data
	for (int i = 0; i < size; i++) {
		data[i] = rand() % 1000;
	}

	// Generate and bind the new texture ID

	// TODO: Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// TODO: Use glTextImage2D to set the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// TODO: Generate texture mip-maps
	glGenerateMipmap(GL_TEXTURE_2D);
	CheckOpenGLError();

	// Save the texture into a wrapper Texture2D class for using easier later during rendering phase
	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator9::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	}
	if (window->KeyHold(GLFW_KEY_W) && platformSpeed <= 3.f) {
		platformSpeed += 1.5f * deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_S) && platformSpeed >= 0.4f && platformSpeed <= 3.f) {
		platformSpeed -= 2 * deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		player->setPosition(player->getPosition() + glm::vec3(-2 * deltaTime, 0, 0));
		if (!isOnAnyPlatform) {
			player->setPosition(player->getPosition() + glm::vec3(0, -fallSpeed * deltaTime, 0));
		}
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		player->setPosition(player->getPosition() + glm::vec3(2 * deltaTime, 0, 0));
		if (!isOnAnyPlatform) {
			player->setPosition(player->getPosition() + glm::vec3(0, -fallSpeed * deltaTime, 0));
		}
	}
	if (window->KeyHold(GLFW_KEY_SPACE)) {
		if (jumpDist <= JUMPHEIGHT + player->getPosition().y) {
			jumpDist += 3 * deltaTime;
		}
	}
}

void Laborator9::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Laborator9::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_SPACE && isOnAnyPlatform) {

		player->setJumpHeight(jumpDist);
		jumpDist = 0.f;
		jumpState = 'u';
		jumpStartY = player->getPosition().y;
	}
}

void Laborator9::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator9::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator9::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator9::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator9::OnWindowResize(int width, int height)
{
}
