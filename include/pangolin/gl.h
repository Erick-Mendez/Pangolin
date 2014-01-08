/* This file is part of the Pangolin Project.
 * http://github.com/stevenlovegrove/Pangolin
 *
 * Copyright (c) 2011 Steven Lovegrove
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PANGOLIN_GL_H
#define PANGOLIN_GL_H

#include <pangolin/glinclude.h>

#if defined(HAVE_EIGEN) && !defined(__CUDACC__) //prevent including Eigen in cuda files
#define USE_EIGEN
#endif

#ifdef USE_EIGEN
#include <Eigen/Eigen>
#endif

#include <math.h>
#include <iostream>
#include <cstdlib>

namespace pangolin
{

////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////

class GlTexture
{
public:
    //! internal_format normally one of GL_RGBA8, GL_LUMINANCE8, GL_INTENSITY16
    GlTexture(GLint width, GLint height, GLint internal_format = GL_RGBA, bool sampling_linear = true, int border = 0, GLenum glformat = GL_RGBA, GLenum gltype = GL_UNSIGNED_BYTE, GLvoid* data = NULL  );
    
#if __cplusplus > 199711L
    //! Move Constructor
    GlTexture(GlTexture&& tex);
#endif
    
    //! Default constructor represents 'no texture'
    GlTexture();
    ~GlTexture();
    
    //! Reinitialise teture width / height / format
    void Reinitialise(GLint width, GLint height, GLint internal_format = GL_RGBA, bool sampling_linear = true, int border = 0, GLenum glformat = GL_RGBA, GLenum gltype = GL_UNSIGNED_BYTE, GLvoid* data = NULL );
    
    void Bind() const;
    void Unbind() const;
    
    //! data_layout normally one of GL_LUMINANCE, GL_RGB, ...
    //! data_type normally one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_FLOAT
    void Upload(const void* image, GLenum data_layout = GL_LUMINANCE, GLenum data_type = GL_FLOAT);
    
    void Download(void* image, GLenum data_layout = GL_LUMINANCE, GLenum data_type = GL_FLOAT) const;
    
    void SetLinear();
    void SetNearestNeighbour();
    
    void RenderToViewport(const bool flip) const;
    void RenderToViewport() const;
    void RenderToViewportFlipY() const;
    void RenderToViewportFlipXFlipY() const;
    
    GLint internal_format;
    GLuint tid;
    GLint width;
    GLint height;
    
private:
    // Private copy constructor
    GlTexture(const GlTexture&) {}
};

struct GlRenderBuffer
{
    GlRenderBuffer(GLint width, GLint height, GLint internal_format = GL_DEPTH_COMPONENT24);
    ~GlRenderBuffer();
    
    GLuint rbid;
};

struct GlFramebuffer
{
    GlFramebuffer();
    ~GlFramebuffer();
    
    GlFramebuffer(GlTexture& colour, GlRenderBuffer& depth);
    GlFramebuffer(GlTexture& colour0, GlTexture& colour1, GlRenderBuffer& depth);
    
    void Bind() const;
    void Unbind() const;
    
    // Attach Colour texture to frame buffer
    // Return attachment texture is bound to (e.g. GL_COLOR_ATTACHMENT0_EXT)
    GLenum AttachColour(GlTexture& tex);
    
    // Attach Depth render buffer to frame buffer
    void AttachDepth(GlRenderBuffer& rb);
    
    GLuint fbid;
    unsigned attachments;
};

enum GlBufferType
{
    GlArrayBuffer = GL_ARRAY_BUFFER,                    // VBO's, CBO's, NBO's
    GlElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,     // IBO's
#ifndef HAVE_GLES
    GlPixelPackBuffer = GL_PIXEL_PACK_BUFFER,           // PBO's
    GlPixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER
#endif
};

struct GlBuffer
{
    //! Default constructor represents 'no buffer'
    GlBuffer();
    GlBuffer(GlBufferType buffer_type, GLuint num_elements, GLenum datatype, GLuint count_per_element, GLenum gluse = GL_DYNAMIC_DRAW );
    
#if __cplusplus > 199711L
    //! Move Constructor
    GlBuffer(GlBuffer&& tex);
#endif  
    
    ~GlBuffer();
    
    void Reinitialise(GlBufferType buffer_type, GLuint num_elements, GLenum datatype, GLuint count_per_element, GLenum gluse );
    void Resize(GLuint num_elements);
    
    void Bind() const;
    void Unbind() const;
    void Upload(const GLvoid* data, GLsizeiptr size_bytes, GLintptr offset = 0);
    
    GLuint bo;
    GlBufferType buffer_type;
    GLenum gluse;
    
    GLenum datatype;
    GLuint num_elements;
    GLuint count_per_element;
private:
    GlBuffer(const GlBuffer&) {}
};

class GlSizeableBuffer
        : public pangolin::GlBuffer
{
public:
    GlSizeableBuffer(pangolin::GlBufferType buffer_type, GLuint initial_num_elements, GLenum datatype, GLuint count_per_element, GLenum gluse = GL_DYNAMIC_DRAW );
    
    void Clear();
    
#ifdef USE_EIGEN
    template<typename Derived>
    void Add(const Eigen::DenseBase<Derived>& vec);
    
    template<typename Derived>
    void Update(const Eigen::DenseBase<Derived>& vec, size_t position = 0);
#endif
    
    size_t start() const;
    
    size_t size() const;
    
protected:  
    void CheckResize(size_t num_verts);
    
    size_t NextSize(size_t min_size) const;
    
    size_t  m_num_verts;    
};

size_t GlDataTypeBytes(GLenum type);

}

// Include implementation
#include <pangolin/gl.hpp>

#endif // PANGOLIN_GL_H
