#pragma once

#include <cstdio>
#include <cstring>
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

        static Audio* create (void) {
            auto newAudio = new Audio();
            if (!newAudio->isValid) {
                delete newAudio;
                return nullptr;
            }
            return newAudio;
        }

        void destroy (void) {
            delete this;
        }

        bool loadWAV (FILE* fp) {
            if (!fp) {
                fprintf(stderr, "Error: WAV file does not exist\n");
                return false;
            }

            char type[5];
            unsigned int size, chunkSize;
            short formatType;
            unsigned int avgBytesPerSec;
            short bytesPerSample;

            if (fread(type, sizeof(char), 4, fp) != 4 || strcmp(type, "RIFF") != 0) {
                fprintf(stderr, "Error: WAV file is not RIFF\n");
                return false;
            }
            fread(&size, sizeof(unsigned int), 1, fp);
            if (fread(type, sizeof(char), 4, fp) != 4 || strcmp(type, "WAVE") != 0) {
                fprintf(stderr, "Error: WAV file is not WAVE\n");
                return false;
            }
            fread(&chunkSize, sizeof(unsigned int), 1, fp);
            fread(&formatType, sizeof(short), 1, fp);
            fread(&channels, sizeof(short), 1, fp);
            fread(&sampleRate, sizeof(unsigned int), 1, fp);
            fread(&avgBytesPerSec, sizeof(unsigned int), 1, fp);
            fread(&bytesPerSample, sizeof(short), 1, fp);
            fread(&bitsPerSample, sizeof(short), 1, fp);
            if (fread(type, sizeof(char), 4, fp) != 4 || strcmp(type, "data") != 0) {
                fprintf(stderr, "Error: Missing data at WAV file\n");
            }
            fread(&dataSize, sizeof(unsigned int), 1, fp);

            if (buf) {
                delete[] buf;
            }
            
            buf = new unsigned char[dataSize];
            fread(buf, sizeof(unsigned char), dataSize, fp);

            if (!bufferData()) {
                delete[] buf;
                return false;
            }

            return true;
        }

        /**************************** PRIVATE *********************************/
        private:
        Audio (void) {
            alGenBuffers(1, &buffer);
            alGenSources(1, &source);
            if (alGetError() != AL_NO_ERROR) {
                fprintf(stderr, "OpenAL Error: Cannot generate source\n");
                return;
            }

            isValid = true;
        }

        ~Audio (void) {
            delete[] buf;
        }

        bool bufferData (void) {
            ALenum format = 0;
            if (bitsPerSample == 8) {
                if (channels == 1) {
                    format = AL_FORMAT_MONO8;
                } else if (channels == 2) {
                    format = AL_FORMAT_STEREO8;
                }
            } else if (bitsPerSample == 16) {
                if (channels == 1) {
                    format = AL_FORMAT_MONO16;
                } else if (channels == 2) {
                    format = AL_FORMAT_STEREO16;
                }
            }

            if (!format) {
                fprintf(stderr, "Error: Wrong BitsPerSample at WAV file\n");
                return false;
            }

            alBufferData(buffer, format, buf, dataSize, sampleRate);
            if (alGetError() != AL_NO_ERROR) {
                fprintf(stderr, "OpenAL Error: Error loading ALBuffer\n");
                return false;
            }
        }

        bool isValid = false;

        static ALCdevice* device;
        static ALCcontext* context;

        ALuint source;
        ALuint buffer;

        short channels;
        unsigned int sampleRate;
        short bitsPerSample;
        unsigned int dataSize;
        unsigned char* buf = nullptr;
    };

    /************************** INITIALIZATION ********************************/
    ALCdevice* Audio::device = nullptr;
    ALCcontext* Audio::context = nullptr;
}
