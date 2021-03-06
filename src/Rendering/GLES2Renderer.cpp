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

#include "GLES2Renderer.hpp"
#include "GLES2ShaderProgramCatalog.hpp"
#include "GLES2TextureCatalog.hpp"
#include "GLES2IndexBufferObjectCatalog.hpp"
#include "GLES2VertexBufferObjectCatalog.hpp"

#include "SimpleShaderProgram.hpp"
#include "TextureShaderProgram.hpp"

#ifdef __APPLE__
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

using namespace Crimild;

GLES2Renderer::GLES2Renderer( FrameBufferObjectPtr screenBuffer )
{
	setShaderProgramCatalog( ShaderProgramCatalogPtr( new GLES2ShaderProgramCatalog() ) );
	setVertexBufferObjectCatalog( VertexBufferObjectCatalogPtr( new GLES2VertexBufferObjectCatalog() ) );
	setIndexBufferObjectCatalog( IndexBufferObjectCatalogPtr( new GLES2IndexBufferObjectCatalog() ) );
	setTextureCatalog( TextureCatalogPtr( new GLES2TextureCatalog() ) );
    
	_fallbackPrograms[ "simple" ] = ShaderProgramPtr( new SimpleShaderProgram() );
	_fallbackPrograms[ "texture" ] = ShaderProgramPtr( new TextureShaderProgram() );
    
	setScreenBuffer( screenBuffer );
}

GLES2Renderer::~GLES2Renderer( void )
{
    
}

void GLES2Renderer::configure( void )
{
    const char *OPENGL_VERSION = ( const char * ) glGetString( GL_VERSION );
    const char *OPENGL_SHADING_LANGUAGE_VERSION = ( const char * ) glGetString( GL_SHADING_LANGUAGE_VERSION );
    const char *OPENGL_VENDOR = ( const char * ) glGetString( GL_VENDOR );
    const char *OPENGL_RENDERER = ( const char * ) glGetString( GL_RENDERER );
    
	Log::Info << "Configuring renderer"
        << "\n       OpenGL version: " << ( OPENGL_VERSION ? OPENGL_VERSION : "<unknown>" )
        << "\n       GLSL version: " << ( OPENGL_SHADING_LANGUAGE_VERSION ? OPENGL_SHADING_LANGUAGE_VERSION : "<unknown>" )
        << "\n       Vendor: " << ( OPENGL_VENDOR ? OPENGL_VENDOR : "<unknown>" )
        << "\n       Renderer: " << ( OPENGL_RENDERER ? OPENGL_RENDERER : "<unknown>" )
        << Log::End;
    
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
}

void GLES2Renderer::beginRender( void )
{
    
}

void GLES2Renderer::endRender( void )
{
    
}

void GLES2Renderer::clearBuffers( void )
{
	const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
	glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLES2Renderer::bindUniform( ShaderLocation *location, int value )
{
	if ( location && location->isValid() ) {
		glUniform1i( location->getLocation(), value );
	}
}

void GLES2Renderer::bindUniform( ShaderLocation *location, float value )
{
	if ( location && location->isValid() ) {
		glUniform1f( location->getLocation(), value );
	}
}

void GLES2Renderer::bindUniform( ShaderLocation *location, const Vector3f &vector )
{
	if ( location && location->isValid() ) {
		glUniform3fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}
}

void GLES2Renderer::bindUniform( ShaderLocation *location, const RGBAColorf &color )
{
	if ( location && location->isValid() ) {
		glUniform4fv( location->getLocation(), 1, static_cast< const GLfloat * >( color.getData() ) );
	}
}

void GLES2Renderer::bindUniform( ShaderLocation *location, const Matrix4f &matrix )
{
	if ( location && location->isValid() ) {
		glUniformMatrix4fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
	}
}

void GLES2Renderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
	GLenum type;
	switch ( primitive->getType() ) {
		case Primitive::Type::POINTS:
			type = GL_POINTS;
			break;
            
		case Primitive::Type::LINES:
			type = GL_LINES;
			break;
			
		case Primitive::Type::LINE_LOOP:
			type = GL_LINE_LOOP;
			break;
			
		case Primitive::Type::LINE_STRIP:
			type = GL_LINE_STRIP;
			break;
			
		case Primitive::Type::TRIANGLE_FAN:
			type = GL_TRIANGLE_FAN;
			break;
			
		case Primitive::Type::TRIANGLE_STRIP:
			type = GL_TRIANGLE_STRIP;
			break;
			
		case Primitive::Type::TRIANGLES:
		default:
			type = GL_TRIANGLES;
			break;
	}
    
	unsigned short *base = 0;
	glDrawElements( type,
				   primitive->getIndexBuffer()->getIndexCount(),
				   GL_UNSIGNED_SHORT,
				   ( const GLvoid * ) base );
}

ShaderProgram *GLES2Renderer::getFallbackProgram( Material *material, Geometry *geometry, Primitive *primitive )
{
	if ( material->getColorMap() ) {
		return _fallbackPrograms[ "texture" ].get();
	}
    
	return _fallbackPrograms[ "simple" ].get();
}

