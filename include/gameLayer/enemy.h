#pragma once
#include <gl2d/gl2d.h>

constexpr float enemyShipSize = 250.f;

struct Enemy
{

    glm::uvec2 type = {}; //used to index into texture atlas
    glm::vec2 position = {};

    float speed = 1500.f;
    float turnSpeed = 3.f;

    
    float firedTime = 1.f;

    float fireTimeReset = 0.2;
    float fireRange = 1.5;

    glm::vec2 viewDirection = {1,0};

    void render(gl2d::Renderer2D &renderer, gl2d::Texture &sprites, gl2d::TextureAtlasPadding &atlas);

    bool update(float deltaTimem, glm::vec2 playerPos);
};