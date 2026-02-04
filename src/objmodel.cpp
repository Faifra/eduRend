#include "OBJModel.h"

OBJModel::OBJModel(
    const std::string& objfile,
    ID3D11Device* dxdevice,
    ID3D11DeviceContext* dxdevice_context)
    : Model(dxdevice, dxdevice_context)
{
    // Load the OBJ
    OBJLoader* mesh = new OBJLoader();
    mesh->Load(objfile);

    // Build index list
    std::vector<unsigned> indices;
    unsigned int indexOffset = 0;

    for (auto& dc : mesh->Drawcalls)
    {
        for (auto& tri : dc.Triangles)
            indices.insert(indices.end(), tri.VertexIndices, tri.VertexIndices + 3);

        unsigned int indexSize = (unsigned int)dc.Triangles.size() * 3;
        int materialIndex = dc.MaterialIndex > -1 ? dc.MaterialIndex : -1;

        m_index_ranges.push_back({ indexOffset, indexSize, 0, materialIndex });
        indexOffset = (unsigned int)indices.size();
    }

    // ------------------------------------------------------------
    // NORMAL MAPPING: Compute Tangent & Binormal for each triangle
    // ------------------------------------------------------------
    auto compute_TB = [](Vertex& v0, Vertex& v1, Vertex& v2)
        {
            vec3f p0 = v0.Position;
            vec3f p1 = v1.Position;
            vec3f p2 = v2.Position;

            vec2f uv0 = v0.TexCoord;
            vec2f uv1 = v1.TexCoord;
            vec2f uv2 = v2.TexCoord;

            vec3f edge1 = p1 - p0;
            vec3f edge2 = p2 - p0;

            float du1 = uv1.x - uv0.x;
            float dv1 = uv1.y - uv0.y;
            float du2 = uv2.x - uv0.x;
            float dv2 = uv2.y - uv0.y;

            float r = 1.0f / (du1 * dv2 - du2 * dv1);

            vec3f tangent = (edge1 * dv2 - edge2 * dv1) * r;
            vec3f binormal = (edge2 * du1 - edge1 * du2) * r;

            tangent = normalize(tangent);
            binormal = normalize(binormal);

            v0.Tangent = v1.Tangent = v2.Tangent = tangent;
            v0.Binormal = v1.Binormal = v2.Binormal = binormal;
        };

    for (int i = 0; i < indices.size(); i += 3)
    {
        compute_TB(
            mesh->Vertices[indices[i + 0]],
            mesh->Vertices[indices[i + 1]],
            mesh->Vertices[indices[i + 2]]);
    }

    // ------------------------------------------------------------
    // Create vertex buffer
    // ------------------------------------------------------------
    D3D11_BUFFER_DESC vertexbufferDesc = {};
    vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexbufferDesc.ByteWidth = (UINT)(mesh->Vertices.size() * sizeof(Vertex));

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = &(mesh->Vertices)[0];

    dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
    SETNAME(m_vertex_buffer, "VertexBuffer");

    // ------------------------------------------------------------
    // Create index buffer
    // ------------------------------------------------------------
    D3D11_BUFFER_DESC indexbufferDesc = {};
    indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = &indices[0];

    dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
    SETNAME(m_index_buffer, "IndexBuffer");

    // ------------------------------------------------------------
    // Copy materials and load textures
    // ------------------------------------------------------------
    append_materials(mesh->Materials);

    std::cout << "Loading textures..." << std::endl;
    for (auto& material : m_materials)
    {
        HRESULT hr;

        // Diffuse
        if (material.DiffuseTextureFilename.size())
        {
            hr = LoadTextureFromFile(
                dxdevice,
                material.DiffuseTextureFilename.c_str(),
                &material.DiffuseTexture);
        }

        // Normal map
        if (material.NormalTextureFilename.size())
        {
            hr = LoadTextureFromFile(
                dxdevice,
                material.NormalTextureFilename.c_str(),
                &material.NormalTexture);
        }
    }
    std::cout << "Done." << std::endl;

    SAFE_DELETE(mesh);
}

void OBJModel::Render() const
{
    const UINT32 stride = sizeof(Vertex);
    const UINT32 offset = 0;

    m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
    m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

    for (auto& indexRange : m_index_ranges)
    {
        const Material& material = m_materials[indexRange.MaterialIndex];

        // Bind diffuse ? t0
        m_dxdevice_context->PSSetShaderResources(0, 1, &material.DiffuseTexture.TextureView);

        // Bind normal ? t1
        m_dxdevice_context->PSSetShaderResources(1, 1, &material.NormalTexture.TextureView);

        m_dxdevice_context->DrawIndexed(indexRange.Size, indexRange.Start, 0);
    }
}

OBJModel::~OBJModel()
{
    for (auto& material : m_materials)
    {
        SAFE_RELEASE(material.DiffuseTexture.TextureView);
        SAFE_RELEASE(material.NormalTexture.TextureView);
    }
}