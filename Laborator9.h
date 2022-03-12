#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include "../../../Visual Studio/Platform.h"
#include "../../../Visual Studio/Player.h"

#define JUMPHEIGHT 3.5f
#define PLATFORMSTARTINGZ -5.f;
#define SPHERERADIUS 0.5f
#define SCALEX 2.f
#define SCALEY 0.25f
#define SCALEZ 4.f
#define MAXFUEL 10.f
#define FUELQUANT 1.5f
#define ROTATIONDELTA 0.5f
#define PYRAMIDWIDTH 1.f
#define PYRAMIDHEIGHT 2.f

class Laborator9 : public SimpleScene
{
	public:
		Laborator9();
		~Laborator9();

		void Init() override;

	private:
		glm::mat4 RotateOX(float radians);
		glm::mat4 RotateOY(float radians);
		void generatePlatforms(std::vector<Platform>& platforms);
		bool isOnPlatform(Platform platform, Player player);
		Mesh* CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill);
		void manageExtraPlatforms(std::vector<Platform>& platforms);
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL);
		Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		std::unordered_map<std::string, Texture2D*> mapTextures;
		GLuint randomTextureID;
		bool isQuad;
		bool isCoconut;
		std::vector<Platform> platforms;
		float platformSpeed;
		float jumpDist;
		float jumpSpeed;
		char jumpState;
		float fallSpeed;
		Player* player;
		bool isOnAnyPlatform;
		float score;
		float platformOn;
		bool isThirdPerson;
		bool cameraFlipped;
		float jumpStartY;
		int season;
		float seasonStart;
		
};
