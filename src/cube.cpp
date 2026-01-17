#include "cube.h"
cube::cube(
    ID3D11Device* dxdevice,
    ID3D11DeviceContext* dxdevice_context)
    : Model(dxdevice, dxdevice_context) {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    // FRONT (+Z)
    Vertex v0, v1, v2, v3;
    v0.Position = { -0.5f, -0.5f,  0.5f }; v0.Normal = { 0, 0, 1 }; v0.TexCoord = { 0,0 };
    v1.Position = { 0.5f, -0.5f,  0.5f }; v1.Normal = { 0, 0, 1 }; v1.TexCoord = { 1,0 };
    v2.Position = { 0.5f,  0.5f,  0.5f }; v2.Normal = { 0, 0, 1 }; v2.TexCoord = { 1,1 };
    v3.Position = { -0.5f,  0.5f,  0.5f }; v3.Normal = { 0, 0, 1 }; v3.TexCoord = { 0,1 };

    // BACK (–Z)
    Vertex v4, v5, v6, v7;
    v4.Position = { 0.5f, -0.5f, -0.5f }; v4.Normal = { 0, 0,-1 }; v4.TexCoord = { 0,0 };
    v5.Position = { -0.5f, -0.5f, -0.5f }; v5.Normal = { 0, 0,-1 }; v5.TexCoord = { 1,0 };
    v6.Position = { -0.5f,  0.5f, -0.5f }; v6.Normal = { 0, 0,-1 }; v6.TexCoord = { 1,1 };
    v7.Position = { 0.5f,  0.5f, -0.5f }; v7.Normal = { 0, 0,-1 }; v7.TexCoord = { 0,1 };

    // RIGHT (+X)
    Vertex v8, v9, v10, v11;
    v8.Position = { 0.5f, -0.5f,  0.5f }; v8.Normal = { 1, 0, 0 }; v8.TexCoord = { 0,0 };
    v9.Position = { 0.5f, -0.5f, -0.5f }; v9.Normal = { 1, 0, 0 }; v9.TexCoord = { 1,0 };
    v10.Position = { 0.5f,  0.5f, -0.5f }; v10.Normal = { 1, 0, 0 }; v10.TexCoord = { 1,1 };
    v11.Position = { 0.5f,  0.5f,  0.5f }; v11.Normal = { 1, 0, 0 }; v11.TexCoord = { 0,1 };

    // LEFT (–X)
    Vertex v12, v13, v14, v15;
    v12.Position = { -0.5f, -0.5f, -0.5f }; v12.Normal = { -1, 0, 0 }; v12.TexCoord = { 0,0 };
    v13.Position = { -0.5f, -0.5f,  0.5f }; v13.Normal = { -1, 0, 0 }; v13.TexCoord = { 1,0 };
    v14.Position = { -0.5f,  0.5f,  0.5f }; v14.Normal = { -1, 0, 0 }; v14.TexCoord = { 1,1 };
    v15.Position = { -0.5f,  0.5f, -0.5f }; v15.Normal = { -1, 0, 0 }; v15.TexCoord = { 0,1 };

    // TOP (+Y)
    Vertex v16, v17, v18, v19;
    v16.Position = { -0.5f, 0.5f,  0.5f }; v16.Normal = { 0, 1, 0 }; v16.TexCoord = { 0,0 };
    v17.Position = { 0.5f, 0.5f,  0.5f }; v17.Normal = { 0, 1, 0 }; v17.TexCoord = { 1,0 };
    v18.Position = { 0.5f, 0.5f, -0.5f }; v18.Normal = { 0, 1, 0 }; v18.TexCoord = { 1,1 };
    v19.Position = { -0.5f, 0.5f, -0.5f }; v19.Normal = { 0, 1, 0 }; v19.TexCoord = { 0,1 };

    // BOTTOM (–Y)
    Vertex v20, v21, v22, v23;
    v20.Position = { -0.5f,-0.5f, -0.5f }; v20.Normal = { 0,-1, 0 }; v20.TexCoord = { 0,0 };
    v21.Position = { 0.5f,-0.5f, -0.5f }; v21.Normal = { 0,-1, 0 }; v21.TexCoord = { 1,0 };
    v22.Position = { 0.5f,-0.5f,  0.5f }; v22.Normal = { 0,-1, 0 }; v22.TexCoord = { 1,1 };
    v23.Position = { -0.5f,-0.5f,  0.5f }; v23.Normal = { 0,-1, 0 }; v23.TexCoord = { 0,1 };

    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
    vertices.push_back(v9);
    vertices.push_back(v10);
    vertices.push_back(v11);
    vertices.push_back(v12);
    vertices.push_back(v13);
    vertices.push_back(v14);
    vertices.push_back(v15);
    vertices.push_back(v16);
    vertices.push_back(v17);
    vertices.push_back(v18);
    vertices.push_back(v19);
    vertices.push_back(v20);
    vertices.push_back(v21);
    vertices.push_back(v22);
    vertices.push_back(v23);

    //triangle 1
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);

    //triangle 2
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    //triangle 3
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(7);

    //triangle 4
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(7);

    //triangle 5
    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(11);

    //triangle 6
    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(11);

    //triangle 7
    indices.push_back(12);
    indices.push_back(13);
    indices.push_back(15);

    //triangle 8
    indices.push_back(13);
    indices.push_back(14);
    indices.push_back(15);

    //triangle 9
    indices.push_back(16);
    indices.push_back(17);
    indices.push_back(19);

    //triangle 10
    indices.push_back(17);
    indices.push_back(18);
    indices.push_back(19);

    //triangle 11
    indices.push_back(20);
    indices.push_back(21);
    indices.push_back(23);

    //triangle 12
    indices.push_back(21);
    indices.push_back(22);
    indices.push_back(23);

    // Vertex array descriptor
    D3D11_BUFFER_DESC vertexbufferDesc{ 0 };
    vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexbufferDesc.CPUAccessFlags = 0;
    vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexbufferDesc.MiscFlags = 0;
    vertexbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
    // Data resource
    D3D11_SUBRESOURCE_DATA vertexData = { 0 };
    vertexData.pSysMem = &vertices[0];
    // Create vertex buffer on device using descriptor & data
    dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
    SETNAME(m_vertex_buffer, "VertexBuffer");

    //  Index array descriptor
    D3D11_BUFFER_DESC indexbufferDesc = { 0 };
    indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexbufferDesc.CPUAccessFlags = 0;
    indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexbufferDesc.MiscFlags = 0;
    indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
    // Data resource
    D3D11_SUBRESOURCE_DATA indexData{ 0 };
    indexData.pSysMem = &indices[0];
    // Create index buffer on device using descriptor & data
    dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
    SETNAME(m_index_buffer, "IndexBuffer");

    m_number_of_indices = (unsigned int)indices.size();

    }

    void cube::Render() const {

        // Bind our vertex buffer
        const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
        const UINT32 offset = 0;
        m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

        // Bind our index buffer
        m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

        // Make the drawcall
        m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);
}
