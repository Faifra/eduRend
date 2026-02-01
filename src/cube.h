#pragma once
#include "Model.h"
#include "Texture.h"

class cube : public Model

{
    unsigned m_number_of_indices = 0;

public:

    cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

    virtual void Render() const;

    ~cube();

private:
    Texture m_diffuseTexture;
};

// skapa cub, lägg in i scen, 12 trianglar 24 vertices