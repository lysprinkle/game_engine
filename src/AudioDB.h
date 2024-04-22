#ifndef AUDIODB_H
#define AUDIODB_H

#include "SDL2_mixer/SDL_mixer.h"
#include "AudioHelper.h"

#include <string>
#include <unordered_map>

class AudioDB
{
public:
    static void Init();
    static int PlayChannel(int channel, const std::string& audio_clip_name, bool does_loop);
    static int HaltChannel(int channel);

private:
    static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
};

#endif // AUDIODB_H