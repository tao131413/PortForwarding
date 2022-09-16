#pragma once

class LinkLoop
{
private:
	char *buffer;
	int	size;
	int	rptr;
	int	wptr;

public:
	int readsize();

	int writesize();

	void create(char* buffer, int size);

	int read(char* data, int size);

	int write(char* data, int size);
};