/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TextureShaderProgram.hpp"

using namespace Crimild;

const char *texture_vs = { CRIMILD_TO_STRING(
     attribute vec3 position;
     attribute vec2 textureCoords;
     
     uniform mat4 projMatrix;
     uniform mat4 viewMatrix;
     uniform mat4 modelMatrix;
     
     varying vec2 uv;
     
     void main( void )
     {
         uv = textureCoords;
         gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0); 
     }
 )};

const char *texture_fs = { CRIMILD_TO_STRING(
     precision highp float;
                                             
     uniform sampler2D colorMap;
     
     varying vec2 uv;
     
     void main( void ) 
     {
         vec4 color = texture2D( colorMap, vec2(uv.s, uv.t ) );
         if ( color.a <= 0.1 ) {
             discard;
         }
         
         gl_FragColor = color;
     }
)};

TextureShaderProgram::TextureShaderProgram( void )
    : ShaderProgram( VertexShaderPtr( new VertexShader( texture_vs ) ), FragmentShaderPtr( new FragmentShader( texture_fs ) ) )
{
    ShaderLocationPtr colorMapLocation( new ShaderLocation( ShaderLocation::Type::UNIFORM, "colorMap" ) );
    registerLocation( colorMapLocation );
}

TextureShaderProgram::~TextureShaderProgram( void )
{ 
}

