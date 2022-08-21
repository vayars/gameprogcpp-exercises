// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec3 fragVerColor;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

void main()
{
    // TODO: Set fragVerColor alpha based on texture alpha
        // 1.0 colors the transparent texture pixels
        // 0.0 makes the whole sprite slightly transparent
	// Sample color from texture and average it with the vertex color
    outColor = mix(vec4(fragVerColor, 1.0), texture(uTexture, fragTexCoord), 0.5);

}
