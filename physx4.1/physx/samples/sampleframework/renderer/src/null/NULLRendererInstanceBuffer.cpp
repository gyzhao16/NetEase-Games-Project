//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2021 NVIDIA Corporation. All rights reserved.

#include <RendererConfig.h>

#include "NULLRendererInstanceBuffer.h"
#include <RendererInstanceBufferDesc.h>

using namespace SampleRenderer;

NullRendererInstanceBuffer::NullRendererInstanceBuffer(const RendererInstanceBufferDesc &desc) :
	RendererInstanceBuffer(desc)
{	
	m_bufferSize   = (PxU32)(desc.maxInstances * m_stride);
	m_buffer       = new PxU8[m_bufferSize];

	RENDERER_ASSERT(m_buffer, "Failed to create Vertex Buffer Object.");
		
	m_maxInstances = desc.maxInstances;
}

NullRendererInstanceBuffer::~NullRendererInstanceBuffer(void)
{
	if(m_buffer)
	{
		delete [] m_buffer;
		m_buffer = NULL;
	}
}


void *NullRendererInstanceBuffer::lock(void)
{
	return m_buffer;
}

void NullRendererInstanceBuffer::unlock(void)
{

}

void NullRendererInstanceBuffer::bind(PxU32 streamID, PxU32 firstInstance) const
{
}

void NullRendererInstanceBuffer::unbind(PxU32 streamID) const
{

}

