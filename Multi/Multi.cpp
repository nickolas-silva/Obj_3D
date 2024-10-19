/**********************************************************************************
// Multi (Código Fonte)
//
// Criação:     27 Abr 2016
// Atualização: 15 Set 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Constrói cena usando vários buffers, um por objeto
//
**********************************************************************************/

#include "DXUT.h"
#include <iostream>
#include <fstream>
#include <sstream>

// ------------------------------------------------------------------------------

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };

    //Cor dos Obj
    XMFLOAT4 CorObj = XMFLOAT4(DirectX::Colors::White);
};

// ------------------------------------------------------------------------------

class Multi : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    ID3D12PipelineState* linePL = nullptr;
    ID3D12PipelineState* trianglePL = nullptr;

    vector<Object> scene;

    Timer timer;
    bool spinning = true;

    XMFLOAT4X4 Identity = {};
    XMFLOAT4X4 View = {};
    XMFLOAT4X4 Proj = {};

    //Proj
    XMFLOAT4X4 ProjOrt = {};
    XMFLOAT4X4 FrontView = {};
    XMFLOAT4X4 LateralView = {};
    XMFLOAT4X4 UpView = {};

    float theta = 0;
    float phi = 0;
    float radius = 0;
    float lastMousePosX = 0;
    float lastMousePosY = 0;

    //-----------------------

    D3D12_VIEWPORT viewTopLeft;
    D3D12_VIEWPORT viewTopRight;
    D3D12_VIEWPORT viewBottomLeft;
    D3D12_VIEWPORT viewBottomRight;

    vector<D3D12_VIEWPORT> views;

    bool flagViewports;

    //-----------------------

    Object lines;

    //-----------------------

    XMMATRIX P;

    int selectedObj = -1;




public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();

    void loadOBJ(const std::string& filename);
};

// ------------------------------------------------------------------------------

void Multi::Init()
{
    graphics->ResetCommands();

    // -----------------------------
    // Parâmetros Iniciais da Câmera
    // -----------------------------

    // controla rotação da câmera
    theta = XM_PIDIV4;
    phi = 1.3f;
    radius = 5.0f;

    // pega última posição do mouse
    lastMousePosX = (float)input->MouseX();
    lastMousePosY = (float)input->MouseY();

    // inicializa as matrizes Identity e View para a identidade
    Identity = View = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    // inicializa a matriz de projeção
    XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        window->AspectRatio(),
        1.0f, 100.0f));

    XMStoreFloat4x4(&ProjOrt, XMMatrixOrthographicLH(5, 5, 1.0f, 100.0f));

    // ----------------------------------------
    // Criação da Geometria: Vértices e Índices
    // ----------------------------------------


    // ---------------------------------------



    // ------------viewports-----------------


    D3D12_VIEWPORT view;

    viewTopLeft.TopLeftX = 0.0f;
    viewTopLeft.TopLeftY = 0.0f;
    viewTopLeft.Width = float(window->Width() / 2);
    viewTopLeft.Height = float(window->Height() / 2);
    viewTopLeft.MinDepth = 0.0f;
    viewTopLeft.MaxDepth = 1.0f;


    viewTopRight.TopLeftX = float(window->Width() / 2);
    viewTopRight.TopLeftY = 0.0f;
    viewTopRight.Width = float(window->Width() / 2);
    viewTopRight.Height = float(window->Height() / 2);
    viewTopRight.MinDepth = 0.0f;
    viewTopRight.MaxDepth = 1.0f;


    viewBottomLeft.TopLeftX = 0.0f;
    viewBottomLeft.TopLeftY = float(window->Height() / 2);
    viewBottomLeft.Width = float(window->Width() / 2);
    viewBottomLeft.Height = float(window->Height() / 2);
    viewBottomLeft.MinDepth = 0.0f;
    viewBottomLeft.MaxDepth = 1.0f;


    viewBottomRight.TopLeftX = float(window->Width() / 2);
    viewBottomRight.TopLeftY = float(window->Height() / 2);
    viewBottomRight.Width = float(window->Width() / 2);
    viewBottomRight.Height = float(window->Height() / 2);
    viewBottomRight.MinDepth = 0.0f;
    viewBottomRight.MaxDepth = 1.0f;

    //add no vetor
    views.push_back(viewTopLeft);
    views.push_back(viewTopRight);
    views.push_back(viewBottomRight);
    views.push_back(viewBottomLeft);

    //-------------------------------------------

    //---------------linhas----------------------
    lines.submesh.indexCount = 4;

    lines.world = Identity;

    vector<Vertex> linesVertx;

    linesVertx.push_back({
        XMFLOAT3(0.0, 1.0, 0.0),
        XMFLOAT4(DirectX::Colors::White)
        });
    linesVertx.push_back({
        XMFLOAT3(0.0, -1.0, 0.0),
        XMFLOAT4(DirectX::Colors::White)
        });
    linesVertx.push_back({
        XMFLOAT3(-1.0, 0.0, 0.0),
        XMFLOAT4(DirectX::Colors::White)
        });
    linesVertx.push_back({
        XMFLOAT3(1.0, 0.0, 0.0),
        XMFLOAT4(DirectX::Colors::White)
        });

    vector<uint> linesIndx;
    linesIndx.push_back(0);
    linesIndx.push_back(1);
    linesIndx.push_back(2);
    linesIndx.push_back(3);

    lines.mesh = new Mesh();
    lines.mesh->VertexBuffer(linesVertx.data(), linesVertx.size() * sizeof(Vertex), sizeof(Vertex));
    lines.mesh->IndexBuffer(linesIndx.data(), linesIndx.size() * sizeof(uint), DXGI_FORMAT_R32_UINT);
    lines.mesh->ConstantBuffer(sizeof(ObjectConstants));

    //--------------------------------------------

    //--------------proj--------------------------

    //view matrix
    XMVECTOR pos = XMVectorSet(0, 0, -10, 1);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);

    //perspectiva
    XMMATRIX perspectiveView = XMMatrixLookAtLH(pos, target, up);

    //frente
    XMVECTOR posFront = XMVectorSet(0, 0.3, -5, 1);
    XMMATRIX frontView = XMMatrixLookAtLH(posFront, target, up);

    //cima
    XMVECTOR posCima = XMVectorSet(0, 10, 0, 1);
    XMVECTOR upUP = XMVectorSet(1, 0, 0, 0);
    XMMATRIX upView = XMMatrixLookAtLH(posCima, target, upUP);

    //lateral
    XMVECTOR posLateral = XMVectorSet(10, 0.3, 0, 1);
    XMMATRIX lateralView = XMMatrixLookAtLH(posLateral, target, up);

    XMStoreFloat4x4(&FrontView, frontView);
    XMStoreFloat4x4(&UpView, upView);
    XMStoreFloat4x4(&LateralView, lateralView);

    // carrega matriz de mundo em uma XMMATRIX
    XMMATRIX world = XMLoadFloat4x4(&lines.world);

    // constrói matriz combinada (world x view x proj)
    XMMATRIX WorldViewProj = world;

    //linhas
    ObjectConstants constants;
    XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
    lines.mesh->CopyConstants(&constants);

    BuildRootSignature();
    BuildPipelineState();

    // ---------------------------------------
    graphics->SubmitCommands();



    timer.Start();
}

// ------------------------------------------------------------------------------

void Multi::Update() {

    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    //Insere uma caixa na cena
    if (input->KeyPress('B')) {
        graphics->ResetCommands();

        Box box(2.0f, 2.0f, 2.0f);
        for (auto& v : box.vertices) v.color = XMFLOAT4(DirectX::Colors::Orange);
        // box
        Object boxObj;
        XMStoreFloat4x4(&boxObj.world,
            XMMatrixScaling(0.4f, 0.4f, 0.4f));

        boxObj.mesh = new Mesh();
        boxObj.mesh->VertexBuffer(box.VertexData(), box.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        boxObj.mesh->IndexBuffer(box.IndexData(), box.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        boxObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        boxObj.submesh.indexCount = box.IndexCount();
        scene.push_back(boxObj);

        graphics->SubmitCommands();

    }

    //Insere um cilindro na cena
    if (input->KeyPress('C')) {
        graphics->ResetCommands();

        Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20);
        for (auto& v : cylinder.vertices) v.color = XMFLOAT4(DirectX::Colors::Yellow);
        Object cylinderObj;
        XMStoreFloat4x4(&cylinderObj.world,
            XMMatrixScaling(0.5f, 0.5f, 0.5f));

        cylinderObj.mesh = new Mesh();
        cylinderObj.mesh->VertexBuffer(cylinder.VertexData(), cylinder.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        cylinderObj.mesh->IndexBuffer(cylinder.IndexData(), cylinder.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        cylinderObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        cylinderObj.submesh.indexCount = cylinder.IndexCount();
        scene.push_back(cylinderObj);
        graphics->SubmitCommands();

    }

    //Insere o plano na cena
    if (input->KeyPress('P')) {
        graphics->ResetCommands();
        Grid grid(3.0f, 3.0f, 20, 20);
        for (auto& v : grid.vertices) v.color = XMFLOAT4(DirectX::Colors::DimGray);
        Object gridObj;
        gridObj.mesh = new Mesh();
        gridObj.world = Identity;
        gridObj.mesh->VertexBuffer(grid.VertexData(), grid.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        gridObj.mesh->IndexBuffer(grid.IndexData(), grid.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        gridObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        gridObj.submesh.indexCount = grid.IndexCount();
        scene.push_back(gridObj);
        graphics->SubmitCommands();
    }

    //Insere uma esfera na cena
    if (input->KeyPress('S')) {
        graphics->ResetCommands();
        Sphere sphere(1.0f, 20, 20);
        for (auto& v : sphere.vertices) v.color = XMFLOAT4(DirectX::Colors::Crimson);
        Object sphereObj;
        XMStoreFloat4x4(&sphereObj.world,
            XMMatrixScaling(0.5f, 0.5f, 0.5f));

        sphereObj.mesh = new Mesh();
        sphereObj.mesh->VertexBuffer(sphere.VertexData(), sphere.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        sphereObj.mesh->IndexBuffer(sphere.IndexData(), sphere.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        sphereObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        sphereObj.submesh.indexCount = sphere.IndexCount();
        scene.push_back(sphereObj);
        graphics->SubmitCommands();
    }

    //Insere uma Geosphere na cena
    if (input->KeyPress('G')) {
        graphics->ResetCommands();
        GeoSphere geosphere(0.5f, 2);
        for (auto& v : geosphere.vertices) v.color = XMFLOAT4(DirectX::Colors::Crimson);
        Object geosphereObj;
        XMStoreFloat4x4(&geosphereObj.world, XMMatrixScaling(0.5f, 0.5f, 0.5f));

        geosphereObj.mesh = new Mesh();
        geosphereObj.mesh->VertexBuffer(geosphere.VertexData(), geosphere.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        geosphereObj.mesh->IndexBuffer(geosphere.IndexData(), geosphere.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        geosphereObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        geosphereObj.submesh.indexCount = geosphere.IndexCount();
        scene.push_back(geosphereObj);
        graphics->SubmitCommands();

    }

    //Insere um Quad na cena
    if (input->KeyPress('Q')) {
        graphics->ResetCommands();
        Quad quad(3.0f, 3.0f);
        for (auto& v : quad.vertices) v.color = XMFLOAT4(DirectX::Colors::Crimson);
        Object quadObj;
        XMStoreFloat4x4(&quadObj.world, XMMatrixScaling(0.5f, 0.5f, 0.5f));

        quadObj.mesh = new Mesh();
        quadObj.mesh->VertexBuffer(quad.VertexData(), quad.VertexCount() * sizeof(Vertex), sizeof(Vertex));
        quadObj.mesh->IndexBuffer(quad.IndexData(), quad.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        quadObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        quadObj.submesh.indexCount = quad.IndexCount();
        scene.push_back(quadObj);
        graphics->SubmitCommands();

    }

    //LER OS ARQUIVOS

    //BOLA - 1
    if (input->KeyPress('1')) {
        loadOBJ("ball.obj");
    }

    //CAPSULA - 2
    if (input->KeyPress('2')) {
        loadOBJ("capsule.obj");
    }

    //HOUSE - 3
    if (input->KeyPress('3')) {
        loadOBJ("house.obj");
    }

    //MONKEY - 4
    if (input->KeyPress('4')) {
        loadOBJ("monkey.obj");
    }

    //THORUS - 5
    if (input->KeyPress('5')) {
        loadOBJ("thorus.obj");
    }



    // Seleciona os objetos
    if (input->KeyPress(VK_TAB)) {
        if (scene.size() != 0) {
            selectedObj = (selectedObj + 1) % scene.size();
        }
    }

    //Deleta o objeto selecionado
    if (input->KeyPress(VK_DELETE)) {
        if (selectedObj != -1) {
            scene.erase(scene.begin() + selectedObj);
            selectedObj = -1;
        }
    }

    //troca a visão
    if (input->KeyPress('V')) {
        flagViewports = !flagViewports;
    }

    //ESCALA
    if (input->KeyDown(VK_CONTROL)) {
        if (input->KeyPress(VK_OEM_PLUS)) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixScaling(1.1f, 1.1f, 1.1f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        if (input->KeyPress(VK_OEM_MINUS)) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixScaling(0.9f, 0.9f, 0.9f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
    }

    //TRANSLAÇÃO
    if (input->KeyDown(VK_SHIFT)) {

        //X
        //positivo
        if (input->KeyPress('A')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(0.3f, 0.0f, 0.0f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('D')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(-0.3f, 0.0f, 0.0f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }

        //Y
        //positivo
        if (input->KeyPress('W')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(0.0f, 0.3f, 0.0f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('X')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(0.0f, -0.3f, 0.0f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }

        //Z
        //positivo
        if (input->KeyPress('R')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(0.0f, 0.0f, 0.3f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('F')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixTranslation(0.0f, 0.0f, -0.3f);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
    }

    //ROTAÇÃO
    if (input->KeyDown(VK_SPACE)) {
        //X
        //positivo
        if (input->KeyPress('N')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationX(5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('M')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationX(-5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }

        //Y
        //positivo
        if (input->KeyPress('K')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationY(5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('L')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationY(-5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }

        //Z
        //positivo
        if (input->KeyPress('I')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationZ(5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
        //negativo
        if (input->KeyPress('O')) {
            if (selectedObj != -1) {
                XMMATRIX world = XMLoadFloat4x4(&scene[selectedObj].world);
                world = world * XMMatrixRotationZ(-5);
                XMStoreFloat4x4(&scene[selectedObj].world, world);
            }
        }
    }

    float mousePosX = (float)input->MouseX();
    float mousePosY = (float)input->MouseY();

    if (input->KeyDown(VK_LBUTTON))
    {
        // cada pixel corresponde a 1/4 de grau
        float dx = XMConvertToRadians(0.25f * (mousePosX - lastMousePosX));
        float dy = XMConvertToRadians(0.25f * (mousePosY - lastMousePosY));

        // atualiza ângulos com base no deslocamento do mouse 
        // para orbitar a câmera ao redor da caixa
        theta += dx;
        phi += dy;

        // restringe o ângulo de phi ]0-180[ graus
        phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);
    }
    else if (input->KeyDown(VK_RBUTTON))
    {
        // cada pixel corresponde a 0.05 unidades
        float dx = 0.05f * (mousePosX - lastMousePosX);
        float dy = 0.05f * (mousePosY - lastMousePosY);

        // atualiza o raio da câmera com base no deslocamento do mouse 
        radius += dx - dy;

        // restringe o raio (3 a 15 unidades)
        radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
    }

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    // converte coordenadas esféricas para cartesianas
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    // constrói a matriz da câmera (view matrix)
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&View, view);

    // carrega matriz de projeção em uma XMMATRIX
    XMMATRIX proj = XMLoadFloat4x4(&Proj);

    // modifica matriz de mundo da esfera


    // ajusta o buffer constante de cada objeto
    for (int i = 0; i < 4; i++) {

        for (auto& obj : scene) {
            XMMATRIX viewT;
            XMMATRIX projT;

            if (i == 0) {
                viewT = XMLoadFloat4x4(&View);
                projT = XMLoadFloat4x4(&Proj);
            }
            else if (i == 1) {
                viewT = XMLoadFloat4x4(&FrontView);
                projT = XMLoadFloat4x4(&ProjOrt);
            }
            else if (i == 2) {
                viewT = XMLoadFloat4x4(&LateralView);
                projT = XMLoadFloat4x4(&ProjOrt);
            }
            else if (i == 3) {
                viewT = XMLoadFloat4x4(&UpView);
                projT = XMLoadFloat4x4(&ProjOrt);
            }
            // carrega matriz de mundo em uma XMMATRIX
            XMMATRIX world = XMLoadFloat4x4(&obj.world);

            // constrói matriz combinada (world x view x proj)
            XMMATRIX WorldViewProj = world * viewT * projT;

            // atualiza o buffer constante com a matriz combinada
            ObjectConstants constants;
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));


            if (selectedObj != -1) {
                if (&obj == &scene[selectedObj]) {
                    XMStoreFloat4(&constants.CorObj, DirectX::Colors::DarkMagenta);
                }
            }
            obj.mesh->CopyConstants(&constants, i);
        }
    }
}

// ------------------------------------------------------------------------------

void Multi::Draw()
{
    // limpa o backbuffer
    graphics->Clear(pipelineState);

    if (flagViewports) {
        //linhas
        pipelineState = linePL;

        ID3D12DescriptorHeap* descriptorHeap = lines.mesh->ConstantBufferHeap();
        graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
        graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
        graphics->CommandList()->IASetVertexBuffers(0, 1, lines.mesh->VertexBufferView());
        graphics->CommandList()->IASetIndexBuffer(lines.mesh->IndexBufferView());
        graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

        graphics->CommandList()->SetGraphicsRootDescriptorTable(0, lines.mesh->ConstantBufferHandle(0));
        graphics->CommandList()->DrawIndexedInstanced(
            lines.submesh.indexCount, 1,
            lines.submesh.startIndex,
            lines.submesh.baseVertex,
            0);

        pipelineState = trianglePL;


        for (int i = 0; i < views.size(); i++) {

            D3D12_VIEWPORT view = views[i];
            graphics->CommandList()->RSSetViewports(1, &view);

            // desenha objetos da cena
            for (auto& obj : scene) {
                // comandos de configuração do pipeline
                ID3D12DescriptorHeap* descriptorHeap = obj.mesh->ConstantBufferHeap();
                graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
                graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
                graphics->CommandList()->IASetVertexBuffers(0, 1, obj.mesh->VertexBufferView());
                graphics->CommandList()->IASetIndexBuffer(obj.mesh->IndexBufferView());
                graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                // ajusta o buffer constante associado ao vertex shader
                graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.mesh->ConstantBufferHandle(i));

                // desenha objeto
                graphics->CommandList()->DrawIndexedInstanced(
                    obj.submesh.indexCount, 1,
                    obj.submesh.startIndex,
                    obj.submesh.baseVertex,
                    0);
            }
        }
    }
    else {
        pipelineState = trianglePL;
        // desenha objetos da cena
        for (auto& obj : scene) {
            // comandos de configuração do pipeline
            ID3D12DescriptorHeap* descriptorHeap = obj.mesh->ConstantBufferHeap();
            graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
            graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
            graphics->CommandList()->IASetVertexBuffers(0, 1, obj.mesh->VertexBufferView());
            graphics->CommandList()->IASetIndexBuffer(obj.mesh->IndexBufferView());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // ajusta o buffer constante associado ao vertex shader
            graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.mesh->ConstantBufferHandle(0));

            // desenha objeto
            graphics->CommandList()->DrawIndexedInstanced(
                obj.submesh.indexCount, 1,
                obj.submesh.startIndex,
                obj.submesh.baseVertex,
                0);
        }
    }


    // apresenta o backbuffer na tela
    graphics->Present();
}

// ------------------------------------------------------------------------------

//Ler o Arquivo
void Multi::loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo OBJ!" << std::endl;

    }

    vector<Vertex> vertices;
    vector<uint> faces;
    vector<uint> facesqnt;

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        ss >> token;

        // Processa os vértices
        if (token == "v") {
            Vertex vertex;

            // Inicializa os valores de x, y e z
            float x = 0.0f, y = 0.0f, z = 0.0f;

            // Tenta ler os valores de x, y e z de forma separada
            if (ss >> x && ss >> y && ss >> z) {
                vertex.pos.x = x;
                vertex.pos.y = y;
                vertex.pos.z = z;

                // Adiciona o vértice à lista se os três valores forem lidos com sucesso
                vertices.push_back(vertex);
            }
        }
        // Processa as faces
        else if (token == "f") {

            string index;
            while (!ss.eof()) {
                ss >> index;

                size_t pos = index.find('//');

                string result = index.substr(0, pos);

                int num = std::stoi(result) - 1;
                facesqnt.push_back(num);  // OBJ usa índices baseados em 1
            }

            if (facesqnt.size() == 3) {
                faces.insert(faces.end(), facesqnt.begin(), facesqnt.end());
            }
            // Se for uma face quadrada (4 vértices), divide em dois triângulos
            else if (facesqnt.size() == 4) {
                // Primeiro triângulo: A-B-C

                faces.push_back(facesqnt[0]);  // A
                faces.push_back(facesqnt[1]);  // B
                faces.push_back(facesqnt[2]);  // C

                // Segundo triângulo: A-C-D

                faces.push_back(facesqnt[0]);  // A
                faces.push_back(facesqnt[2]);  // C
                faces.push_back(facesqnt[3]);  // D
                
            }
        }
        facesqnt.clear();
    }

    //constroi o object
    graphics->ResetCommands();
    Object object;
    XMStoreFloat4x4(&object.world, XMMatrixScaling(0.5f, 0.5f, 0.5f));

    object.mesh = new Mesh();
    object.mesh->VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex), sizeof(Vertex));
    object.mesh->IndexBuffer(faces.data(), faces.size() * sizeof(uint), DXGI_FORMAT_R32_UINT);
    object.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
    object.submesh.indexCount = faces.size();
    scene.push_back(object);
    graphics->SubmitCommands();

}

void Multi::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();

    for (auto& obj : scene)
        delete obj.mesh;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Multi::BuildRootSignature()
{
    // cria uma única tabela de descritores de CBVs
    D3D12_DESCRIPTOR_RANGE cbvTable = {};
    cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    cbvTable.NumDescriptors = 1;
    cbvTable.BaseShaderRegister = 0;
    cbvTable.RegisterSpace = 0;
    cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // define parâmetro raiz com uma tabela
    D3D12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

    // uma assinatura raiz é um vetor de parâmetros raiz
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = rootParameters;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // serializa assinatura raiz
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    if (error != nullptr)
    {
        OutputDebugString((char*)error->GetBufferPointer());
    }

    // cria uma assinatura raiz com um único slot que aponta para  
    // uma faixa de descritores consistindo de um único buffer constante
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Multi::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------

    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    // --------------------
    // ---- Rasterizer ----
    // --------------------

    D3D12_RASTERIZER_DESC rasterizer = {};
    //rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ---------------------
    // --- Color Blender ---
    // ---------------------

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // ---------------------
    // --- Depth Stencil ---
    // ---------------------

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;

    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&trianglePL));

    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&linePL));



    vertexShader->Release();
    pixelShader->Release();
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 720);
        engine->window->Color(25, 25, 25);
        engine->window->Title("Multi");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        engine->Start(new Multi());

        // finaliza execução
        delete engine;
    }
    catch (Error& e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Multi", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------

