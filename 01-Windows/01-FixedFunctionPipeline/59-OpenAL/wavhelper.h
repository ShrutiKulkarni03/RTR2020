// wav file helper
#include <iostream>
#include <fstream>
#include <cstring>

bool isBigEndian()
{
    int a = 1;
    return !((char *)&a)[0];
}

int convertToInt(char *buffer, int len)
{
    int a = 0;
    if(!isBigEndian())
    {
        for(int i = 0; i < len; i++)
            ((char *)&a)[i] = buffer[i];
    }
    else
    {
        for(int i = 0; i < len; i++)
            ((char *)&a)[3-i] = buffer[i];
    }
	return a;
}

char *loadWav(const char *fn, int *chan, int* samplerate, int *bps, int *size)
{
    char buffer[4];
    std::ifstream in(fn, std::ios::binary);
    in.read(buffer, 4);
    if(strncmp(buffer, "RIFF", 4)!= 0)
    {
        printf("\nThis is not a valid WAVE file..\n");
        return NULL;
    }

    in.read(buffer, 4);
    in.read(buffer, 4);  // WAVE
    in.read(buffer, 4);  // fmt
    in.read(buffer, 4);  // 16
    in.read(buffer, 2);  // 1
    
    in.read(buffer, 2); 
    *chan = convertToInt(buffer, 2);
    
    in.read(buffer, 4);
    *samplerate = convertToInt(buffer, 4);

    in.read(buffer, 4);
    in.read(buffer, 2);
    
    in.read(buffer, 2);
    *bps = convertToInt(buffer, 2);

    in.read(buffer, 4);

    in.read(buffer, 4);
    *size = convertToInt(buffer, 4);

    char *data = new char[*size];
    in.read(data, *size);
    return data;
}


