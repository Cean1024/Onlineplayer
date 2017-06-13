#ifndef SOUNDALSA_H
#define SOUNDALSA_H
#include <alsa/asoundlib.h>
#include "globaflags.h"

class soundalsa
{
public:
    soundalsa();
    ~soundalsa();
    int sound_read();
    void sound_write(int);
private:
    snd_mixer_t * mixer;
    snd_mixer_elem_t *pcm_element;
};

#endif // SOUNDALSA_H
