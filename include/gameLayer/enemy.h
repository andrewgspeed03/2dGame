#pragma once
#include <gl2d/gl2d.h>

struc Enemy
{

    glm::uvet2 type = {} //used to index into texture atlas
    glm::vec2 position = {};

    glm::vec2 viewDirection = {1,0};

    void render(gl2d::Renderer2D &renderer, gl2d::TexturePositions);

};