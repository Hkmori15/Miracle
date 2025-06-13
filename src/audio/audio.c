#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

static ALCdevice* device = NULL;
static ALCcontext* context = NULL;

static void check_al_error(const char* operation) {
    ALenum error = alGetError();

    if (error != AL_NO_ERROR) {
        printf("OpenAL error during %s: %d\n", operation, error);
    }
}

static void check_alc_error(ALCdevice* device, const char* operation) {
    ALCenum error = alcGetError(device);

    if (error != ALC_NO_ERROR) {
        printf("OpenAL context error during %s: %d\n", operation, error);
    }
}

/**
    * Helper func to determine OpenAL format from libsndfile info
**/

static ALenum get_openal_format(const SF_INFO* info) {
    if (info->channels == 1) {
        return AL_FORMAT_MONO16;
    } else if (info->channels == 2) {
        return AL_FORMAT_STEREO16;
    } else {
        printf("Unsupported channel count: %d\n", info->channels);

        return 0;
    }
}

int audio_init(void) {
    device = alcOpenDevice(NULL);

    if (!device) {
        printf("Failed to open OpenAL device\n");

        return -1;
    }

    context = alcCreateContext(device, NULL);

    if (!context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(device);
        device = NULL;

        return -1;
    }

    if (!alcMakeContextCurrent(context)) {
        printf("Failed to make OpenAL context current\n");
        alcDestroyContext(context);
        alcCloseDevice(device);
        device = NULL;
        context = NULL;

        return -1;
    }

    check_alc_error(device, "initialization");

    // Debug|OpenAL information
    printf("OpenAL init successfully\n");
    printf("OpenAL vendor: %s\n", alGetString(AL_VENDOR));
    printf("OpenAL version: %s\n", alGetString(AL_VERSION));
    printf("OpenAL renderer: %s\n", alGetString(AL_RENDERER));

    return 0;
}

unsigned int audio_load_sound(const char *path) {
    printf("Loading sound: %s\n", path);

    // Check if this request for test sound
    if (strcmp(path, "test_sound") == 0) {
        return audio_generate_test_sound();
    }

    SF_INFO file_info;
    memset(&file_info, 0, sizeof(file_info));

    SNDFILE* file = sf_open(path, SFM_READ, &file_info);

    if (!file) {
        printf("Failed to open audio file: %s\n", path);
        printf("Libsndfile error: %s\n", sf_strerror(NULL));

        return 0;
    }

    printf("Audio file info:\n");
    printf("  Sample rate: %d Hz\n", file_info.samplerate);
    printf("  Channels: %d\n", file_info.channels);
    printf("  Frames: %ld\n", file_info.frames);
    printf("  Duration: %.2f sec\n", (double)file_info.frames / file_info.samplerate);

    ALenum format = get_openal_format(&file_info);

    if (format == 0) {
        sf_close(file);

        return 0;
    }

    // Allocate buffer for audio data
    sf_count_t num_samples = file_info.frames * file_info.channels;
    short* samples = malloc(num_samples * sizeof(short));

    if (!samples) {
        printf("Failed to allocate mem for audio data\n");
        sf_close(file);

        return 0;
    }

    // Now we read audio data
    sf_count_t read_count = sf_read_short(file, samples, num_samples);

    if (read_count != num_samples) {
        printf("WARNING: Expected %ld samples, read %ld\n", num_samples, read_count);
    }

    sf_close(file);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    check_al_error("generate buffer");

    // Upload data to OpenAL
    alBufferData(buffer, format, samples, (ALsizei)(read_count * sizeof(short)), file_info.samplerate);
    check_al_error("buffer data");

    free(samples);

    printf("Successfully loaded audio file: %s | buffer ID: %u\n", path, buffer);

    return buffer;
}

// Func to test|debug sound
unsigned int audio_generate_test_sound(void) {
    ALuint buffer;
    alGenBuffers(1, &buffer);
    check_al_error("generate buffer");

    // Sine wave
    const int sample_rate = 44100;
    const int duration = 1; // 1 sec
    const int num_samples = sample_rate * duration;
    
    short* samples = malloc(num_samples * sizeof(short));

    if (!samples) {
        printf("Failed to allocate mem for test audio samples\n");
        alDeleteBuffers(1, &buffer);

        return 0;
    }

    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / sample_rate;
        samples[i] = (short)(32767.0f * 0.3f * sin(2.0f * 3.14159f * 440.0f * t));
    }

    alBufferData(buffer, AL_FORMAT_MONO16, samples, num_samples * sizeof(short), sample_rate);
    check_al_error("buffer data");

    free(samples);

    printf("Generated test sine wave buffer: 440Hz, 1 sec\n");

    return buffer;
}

unsigned int audio_play_sound(unsigned int buffer_id) {
    if (buffer_id == 0) {
        printf("Invalid buffer ID\n");
        
        return 0;
    }

    ALuint source;
    alGenSources(1, &source);
    check_al_error("generate source");

    alSourcei(source, AL_BUFFER, buffer_id);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alSourcePlay(source);
    check_al_error("play source");

    printf("Playing sound buffer %u with source %u\n", buffer_id, source);

    return source;
}

void audio_stop_sound(unsigned int source_id) {
    if (source_id == 0) {
        return;
    }

    alSourceStop(source_id);
    alDeleteSources(1, &source_id);
    check_al_error("stop and delete source");

    printf("Stopped and deleted sound source %u\n", source_id);
}

void audio_set_listener(float pos[3], float forward[3], float up[3]) {
    alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);

    // Set listener orientation: forward and up vectors - 3x3
    float orientation[6] = {
        forward[0], forward[1], forward[2],
        up[0], up[1], up[2]
    };

    alListenerfv(AL_ORIENTATION, orientation);

    // Set listener velocity
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    check_al_error("set listener properties");
}

void audio_set_source_position(unsigned int source_id, float pos[3]) {
    if (source_id == 0) {
        return;
    }

    alSource3f(source_id, AL_POSITION, pos[0], pos[1], pos[2]);
    check_al_error("set source position");
}

void audio_delete_buffer(unsigned int buffer_id) {
    if (buffer_id != 0) {
        alDeleteBuffers(1, &buffer_id);
        check_al_error("delete buffer");
    }
}

void audio_shutdown(void) {
    if (context) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        context = NULL;
    }

    if (device) {
        alcCloseDevice(device);
        device = NULL;
    }

    printf("OpenAL audio system shutdown complete\n");
}