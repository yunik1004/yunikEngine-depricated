#pragma once

#include <cstdio>
#include <AL/al.h>
#include <AL/alc.h>

namespace yunikEngine {
    class Audio {
        /***************************** PUBLIC *********************************/
        public:
        static bool init (void) {
            device = alcOpenDevice(nullptr);
            if (!device) {
                fprintf(stderr, "OpenAL Error: No sound device\n");
                return false;
            }

            context = alcCreateContext(device, nullptr);
            alcMakeContextCurrent(context);
            if (!context) {
                fprintf(stderr, "OpenAL Error: No sound context\n");
                alcCloseDevice(device);
                return false;
            }
        }

        static void deinit (void) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
            alcCloseDevice(device);
        }

        Audio (void);
        ~Audio (void);

        /**************************** PRIVATE *********************************/
        private:
        static ALCdevice* device;
        static ALCcontext* context;
    };

    /************************** INITIALIZATION ********************************/
    ALCdevice* Audio::device = nullptr;
    ALCcontext* Audio::context = nullptr;
}
