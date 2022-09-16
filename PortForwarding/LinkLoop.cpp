#include "stdafx.h"
#include <string.h>
#include "LinkLoop.h"


#define	PTR_INC(ptr, size)	{if( ++ptr >= size )	ptr = 0;}

int LinkLoop::readsize()
{
	int	ret = this->wptr - this->rptr;
	if (ret < 0) ret += this->size;

	return ret;
}

int LinkLoop::writesize()
{
	int	ret = this->rptr - this->wptr;
	if (ret <= 0) ret += this->size;

	return --ret;
}


void LinkLoop::create(char* buffer, int size)
{
	this->buffer = buffer;
	this->size = size;
	this->rptr = 0;
	this->wptr = 0;

	memset(buffer, 0, size);
}

int LinkLoop::read(char* data, int size)
{
	int	ret = 0;

	for (int i = 0; i < size; i++)
	{
		if (readsize() < 1) break;
		data[i] = this->buffer[this->rptr];
		PTR_INC(this->rptr, this->size);
		ret++;
	}

	return ret;
}

int LinkLoop::write(char* data, int size)
{
	int	ret = 0;

	for (int i = 0; i < size; i++)
	{
		if (writesize() < 1) break;
		this->buffer[this->wptr] = data[i];
		PTR_INC(this->wptr, this->size);
		ret++;
	}

	return ret;
}