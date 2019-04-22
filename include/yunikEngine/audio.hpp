#pragma once

#include <cstdio>
#include <cstring>
#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>

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

            setListenerPos(glm::vec3(0.0, 0.0, 0.0));
            setListenerVel(glm::vec3(0.0, 0.0, 0.0));
            setListenerOri(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));

            return true;
        }

        static void deinit (void) {
            alcMakeContextCurrent(nullptr);
            if (context) {
                alcDestroyContext(context);
            }
            if (device) {
                alcCloseDevice(device);
            }
        }

        static void setListenerPos (glm::vec3 pos) {
            ALfloat position[] = {pos.x, pos.y, pos.z};
            alListenerfv(AL_POSITION, position);
        }

        static void setListenerVel (glm::vec3 vel) {
            ALfloat velocity[] = {vel.x, vel.y, vel.z};
            alListenerfv(AL_VELOCITY, velocity);
        }

        static void setListenerOri (glm::vec3 at, glm::vec3 up) {
            ALfloat ori[] = {at.x, at.y, at.z, up.x, up.y, up.z};
            alListenerfv(AL_ORIENTATION, ori);
        }

        void setSourcePitch (float pitch) {
            alSourcef(source, AL_PITCH, pitch);
        }

        void setSourceGain (float gain) {
            alSourcef(source, AL_GAIN, gain);
        }

        void setSourcePos (glm::vec3 pos) {
            ALfloat position[] = {pos.x, pos.y, pos.z};
            alSourcefv(source, AL_POSITION, position);
        }

        void setSourceVel (glm::vec3 vel) {
            ALfloat velocity[] = {vel.x, vel.y, vel.z};
            alSourcefv(source, AL_VELOCITY, velocity);
        }

        void setSourceLooping (bool isLooping) {
            alSourcei(source, AL_LOOPING, isLooping);
        }

        void setSourceRelative (bool isRelative) {
            alSourcei(source, AL_SOURCE_RELATIVE, isRelative);
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

        bool play (void) {
            alSourcePlay(source);
            if (alGetError() != AL_NO_ERROR) {
                fprintf(stderr, "OpenAL Error: Cannot play sound\n");
                return false;
            }
            return true;
        }

        bool stop (void) {
            alSourceStop(source);
            if (alGetError() != AL_NO_ERROR) {
                fprintf(stderr, "OpenAL Error: Cannot stop sound\n");
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
            if (source) {
                alDeleteSources(1, &source);
            }
            if (buffer) {
                alDeleteBuffers(1, &buffer);
            }
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

            alSourcei(source, AL_BUFFER, buffer);
            setSourceRelative(true);
            setSourcePos(glm::vec3(0.0, 0.0, 0.0));
            setSourceVel(glm::vec3(0.0, 0.0, 0.0));

            return true;
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
