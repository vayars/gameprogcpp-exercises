// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <GL/glew.h>

Renderer::Renderer(Game* game)
	:mGame(game)
	,mSpriteShader(nullptr)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 6)", 100, 100,
		static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	// Make sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	return true;
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
    mShaders.at("BasicMesh")->Unload();
    delete mShaders.at("BasicMesh");
	mShaders.at("Phong")->Unload();
	delete mShaders.at("Phong");
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// Destroy meshes
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw()
{
	// Set the clear color to light grey
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    // Set the BasicMesh shader active
    mShaders.at("BasicMesh")->SetActive();
    // Update view-projection matrix
    mShaders.at("BasicMesh")->SetMatrixUniform("uViewProj", mView * mProjection);
    for (auto mc : mMeshComps)
    {
        //SDL_Log("%s", mc->GetShaderName().c_str());
        if (mc->GetShaderName() == "BasicMesh")
        {
            //SDL_Log("Drawing MeshComponent");
            mc->Draw(mShaders.at("BasicMesh"));
        }
    }
    
	// Set the Phong shader active
    mShaders.at("Phong")->SetActive();
	// Update view-projection matrix
    mShaders.at("Phong")->SetMatrixUniform("uViewProj", mView * mProjection);
	// Update lighting uniforms
	SetLightUniforms(mShaders.at("Phong"));
	for (auto mc : mMeshComps)
	{
        //SDL_Log("%s", mc->GetShaderName().c_str());
        if (mc->GetShaderName() == "Phong")
        {
            //SDL_Log("Drawing MeshComponent");
            mc->Draw(mShaders.at("Phong"));
        }
	}

	// Draw all sprite components
	// Disable depth buffering
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	mMeshComps.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
	mMeshComps.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName)
{
	Mesh* m = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			mMeshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	// Create basic mesh shader
    Shader* basicShader = new Shader();
    if (!basicShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
    {
        return false;
    }
    // Create Phong mesh shader
	Shader* phongShader = new Shader();
	if (!phongShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}

	basicShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	basicShader->SetMatrixUniform("uViewProj", mView * mProjection);
    mShaders.emplace("BasicMesh", basicShader);
	
    phongShader->SetActive();
    phongShader->SetMatrixUniform("uViewProj", mView * mProjection);
    mShaders.emplace("Phong", phongShader);
    
    return true;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// Camera position is from inverted view
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection",
		mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor",
		mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor",
		mDirLight.mSpecColor);
    // Point light 1
    shader->SetVectorUniform("uPointLightArr[0].mPosition", mPointLightArr[0].mPosition);
    shader->SetVectorUniform("uPointLightArr[0].mDiffuseColor", mPointLightArr[0].mDiffuseColor);
    shader->SetVectorUniform("uPointLightArr[0].mSpecColor",
        mPointLightArr[0].mSpecColor);
    shader->SetFloatUniform("uPointLightArr[0].mRadius", mPointLightArr[0].mRadius);
    // Point light 2
    shader->SetVectorUniform("uPointLightArr[1].mPosition", mPointLightArr[1].mPosition);
    shader->SetVectorUniform("uPointLightArr[1].mDiffuseColor", mPointLightArr[1].mDiffuseColor);
    shader->SetVectorUniform("uPointLightArr[1].mSpecColor",
        mPointLightArr[1].mSpecColor);
    shader->SetFloatUniform("uPointLightArr[1].mRadius", mPointLightArr[1].mRadius);
    // Point light 3
    shader->SetVectorUniform("uPointLightArr[2].mPosition", mPointLightArr[2].mPosition);
    shader->SetVectorUniform("uPointLightArr[2].mDiffuseColor", mPointLightArr[2].mDiffuseColor);
    shader->SetVectorUniform("uPointLightArr[2].mSpecColor",
        mPointLightArr[2].mSpecColor);
    shader->SetFloatUniform("uPointLightArr[2].mRadius", mPointLightArr[2].mRadius);
    // Point light 4
    shader->SetVectorUniform("uPointLightArr[3].mPosition", mPointLightArr[3].mPosition);
    shader->SetVectorUniform("uPointLightArr[3].mDiffuseColor", mPointLightArr[3].mDiffuseColor);
    shader->SetVectorUniform("uPointLightArr[3].mSpecColor",
        mPointLightArr[3].mSpecColor);
    shader->SetFloatUniform("uPointLightArr[3].mRadius", mPointLightArr[3].mRadius);
}
