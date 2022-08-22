// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// Create a struct for directional light
struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};

// Create a struct for point light
struct PointLight
{
    // Position of light
    vec3 mPosition;
    // Diffuse color
    vec3 mDiffuseColor;
    // Specular color
    vec3 mSpecColor;
    // Radius of Inffluence
    float mRadius;
};

// Uniforms for lighting
// Camera position (in world space)
uniform vec3 uCameraPos;
// Specular power for this surface
uniform float uSpecPower;
// Ambient light level
uniform vec3 uAmbientLight;

// Directional Light
uniform DirectionalLight uDirLight;
// Array of  point lights
uniform PointLight uPointLightArr[4];

void main()
{
	// Surface normal
	vec3 N = normalize(fragNormal);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
    vec3 Diffuse = vec3(0.0,0.0,0.0);
    vec3 Specular = vec3(0.0,0.0,0.0);
    
    // Check directional light
	if (NdotL > 0)
	{
		Diffuse += uDirLight.mDiffuseColor * NdotL;
		Specular += uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
	}
    // Check point lights
    for (int i = 0; i<uPointLightArr.length(); i++)
    {
        // If the pixel is within the radius of the point light
        if (sqrt(pow((fragWorldPos.x-uPointLightArr[i].mPosition.x),2)+
                 pow((fragWorldPos.y-uPointLightArr[i].mPosition.y),2)+
                 pow((fragWorldPos.z-uPointLightArr[i].mPosition.z),2))
            <= uPointLightArr[i].mRadius)
        {
            Diffuse += uPointLightArr[i].mDiffuseColor * dot(N, normalize(uPointLightArr[i].mPosition - fragWorldPos));
            Specular += uPointLightArr[i].mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
        }
    }
    
    Phong += Diffuse + Specular; // If Diffuse and/or Specular are 0.0,0.0,0.0, Phong remains unchanged

	// Final color is texture color times phong light (alpha = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
