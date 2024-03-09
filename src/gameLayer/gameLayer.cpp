#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <tiledRenderer.h>
#include <bullet.h>
#include <vector>

struct GameplayData
{
	glm::vec2 playerPos = {100,100};

	std::vector<Bullet> bullets;
};

GameplayData data;

gl2d::Renderer2D renderer;

constexpr int BACKGROUNDS = 4;

gl2d::Texture spaceShipsTexture;
gl2d::TextureAtlasPadding spaceShipsAtlas;

gl2d::Texture bulletsTexture;
gl2d::TextureAtlasPadding bulletsAtlas;

gl2d::Texture backgroundTexture[BACKGROUNDS];

TiledRenderer tiledRenderer[BACKGROUNDS];

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	spaceShipsTexture.loadFromFileWithPixelPadding(
		RESOURCES_PATH "spaceShip/stitchedFiles/spaceships.png", 128, true);
	spaceShipsAtlas = gl2d::TextureAtlasPadding(5, 2, spaceShipsTexture.GetSize().x, spaceShipsTexture.GetSize().y);

	bulletsTexture.loadFromFileWithPixelPadding(
		RESOURCES_PATH "spaceShip/stitchedFiles/projectiles.png", 500, true);
	bulletsAtlas = gl2d::TextureAtlasPadding(3, 2, bulletsTexture.GetSize().x, bulletsTexture.GetSize().y);
	
	backgroundTexture[0].loadFromFile(RESOURCES_PATH "background1.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "background2.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "background3.png", true);
	backgroundTexture[3].loadFromFile(RESOURCES_PATH "background4.png", true);

	for(int i = 0; i < BACKGROUNDS; i++)
		tiledRenderer[i].texture = backgroundTexture[i];

	tiledRenderer[0].paralaxStrength = 0;
	tiledRenderer[1].paralaxStrength = 0.2;
	tiledRenderer[2].paralaxStrength = 0.4;
	tiledRenderer[3].paralaxStrength = 0.7;

	return true;
}



bool gameLogic(float deltaTime)
{	
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);

#pragma endregion

#pragma region movement
	glm::vec2 move = {};

	if(
		platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up)
		)
	{
		move.y = -1;
	}
	if(
		platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down)
		)
	{
		move.y = 1;
	}
	if(
		platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left)
		)
	{
		move.x = -1;
	}
	if(
		platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right)
		)
	{
		move.x = 1;
	}

	if (move.x != 0 || move.y != 0)
	{
		move = glm::normalize(move);
		move *= deltaTime * 2000; // 2000 pixels per sec
		data.playerPos += move; 
	}
#pragma endregion

#pragma region follow

	renderer.currentCamera.follow(data.playerPos, deltaTime * 550, 1, 150, w, h);

#pragma endregion

#pragma region render background

	for(int i = 0; i < BACKGROUNDS; i++)
		tiledRenderer[i].render(renderer);

#pragma endregion

#pragma region mouse pos

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	glm::vec2 mouseDirection = mousePos - screenCenter;

	if(glm::length(mouseDirection) == 0.f)
		mouseDirection = {1,0};
	else
		mouseDirection = normalize(mouseDirection);

	float spaceShipAngle = atan2(mouseDirection.y, -mouseDirection.x);

#pragma endregion

#pragma region handle bullets

	if(platform::isLMousePressed()){
		Bullet b;

		b.position = data.playerPos;
		b.fireDirection = mouseDirection;

		data.bullets.push_back(b);
	}

	for(int i = 0; i < data.bullets.size(); i++){

		if(glm::distance(data.bullets[i].position, data.playerPos) > 5000){
			data.bullets.erase(data.bullets.begin() + i);
			i--;
			continue;
		}

		data.bullets[i].update(deltaTime);
	}

#pragma endregion

#pragma region render bullets

	for (auto &b : data.bullets)
		b.render(renderer, bulletsTexture, bulletsAtlas);

#pragma endregion

#pragma region render ship
	
	constexpr float SHIP_SIZE = 250.f;

	renderer.renderRectangle(
		{data.playerPos - glm::vec2(SHIP_SIZE / 2, SHIP_SIZE / 2), SHIP_SIZE, SHIP_SIZE},
		spaceShipsTexture,
		Colors_White, {}, glm::degrees(spaceShipAngle) + 90.f,
		spaceShipsAtlas.get(1,0));

#pragma endregion

	renderer.flush();


	//ImGui::ShowDemoWindow();

	ImGui::Begin("debug");

	ImGui::Text("Bullets count: %d", (int)data.bullets.size());

	ImGui::End();

	return true;

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
