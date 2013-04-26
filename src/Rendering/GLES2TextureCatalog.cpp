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

#include "GLES2TextureCatalog.hpp"

#ifdef __APPLE__
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

using namespace Crimild;

GLES2TextureCatalog::GLES2TextureCatalog( void )
    : _boundTextureCount( 0 )
{
    
}

GLES2TextureCatalog::~GLES2TextureCatalog( void )
{
    
}

int GLES2TextureCatalog::getNextResourceId( void )
{
	GLuint textureId = 0;
	glGenTextures( 1, &textureId );
    return textureId;
}

void GLES2TextureCatalog::bind( ShaderProgram *program, Texture *texture )
{
	Catalog< Texture >::bind( program, texture );
    
	ShaderLocation *mapLocation = program->getLocation( texture->getName() );
	if ( mapLocation && mapLocation->isValid() ) {
		glActiveTexture( GL_TEXTURE0 + _boundTextureCount );
		glBindTexture( GL_TEXTURE_2D, texture->getCatalogId() );
		glUniform1i( mapLocation->getLocation(), _boundTextureCount );
        
		++_boundTextureCount;
	}
}

void GLES2TextureCatalog::unbind( ShaderProgram *program, Texture *texture )
{
	if ( _boundTextureCount > 0 ) {
		--_boundTextureCount;
		glActiveTexture( GL_TEXTURE0 + _boundTextureCount );
		glBindTexture( GL_TEXTURE_2D, _boundTextureCount );
	}
	
	Catalog< Texture >::unbind( program, texture );
}

void GLES2TextureCatalog::load( Texture *texture )
{
	Catalog< Texture >::load( texture );
    
	int textureId = texture->getCatalogId();
    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                 texture->getImage()->getWidth(), texture->getImage()->getHeight(), 0,
                 ( texture->getImage()->getBpp() == 3 ? GL_RGB : GL_RGBA ),
                 GL_UNSIGNED_BYTE,
                 ( GLvoid * ) texture->getImage()->getData() );
}

void GLES2TextureCatalog::unload( Texture *texture )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
    
	Catalog< Texture >::unload( texture );
}

