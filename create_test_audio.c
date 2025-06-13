#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// Simple WAV header structure
#pragma pack(push, 1)
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t chunk_size;    // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmt_size;      // 16 for PCM
    uint16_t audio_format;  // 1 for PCM
    uint16_t num_channels;  // 1 for mono, 2 for stereo
    uint32_t sample_rate;   // Sample rate
    uint32_t byte_rate;     // sample_rate * num_channels * bits_per_sample / 8
    uint16_t block_align;   // num_channels * bits_per_sample / 8
    uint16_t bits_per_sample; // 16 for 16-bit
    char data[4];           // "data"
    uint32_t data_size;     // Size of audio data
} wav_header_t;
#pragma pack(pop)

void create_wav_file(const char* filename, float frequency, float duration, int sample_rate) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Failed to create file: %s\n", filename);
        return;
    }
    
    int num_samples = (int)(sample_rate * duration);
    int data_size = num_samples * sizeof(int16_t);
    
    // Create WAV header
    wav_header_t header;
    memcpy(header.riff, "RIFF", 4);
    header.chunk_size = 36 + data_size;
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = sample_rate;
    header.byte_rate = sample_rate * sizeof(int16_t);
    header.block_align = sizeof(int16_t);
    header.bits_per_sample = 16;
    memcpy(header.data, "data", 4);
    header.data_size = data_size;
    
    // Write header
    fwrite(&header, sizeof(header), 1, file);
    
    // Generate and write audio data
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / sample_rate;
        float sample = 0.3f * sin(2.0f * M_PI * frequency * t);
        int16_t sample_16 = (int16_t)(sample * 32767.0f);
        fwrite(&sample_16, sizeof(sample_16), 1, file);
    }
    
    fclose(file);
    printf("Created %s: %.1fHz sine wave, %.1fs duration\n", filename, frequency, duration);
}

int main() {
    // Create assets/audio directory
    system("mkdir -p assets/audio");
    
    // Create test audio files
    create_wav_file("assets/audio/beep_low.wav", 220.0f, 0.5f, 44100);    // Low beep
    create_wav_file("assets/audio/beep_high.wav", 880.0f, 0.3f, 44100);   // High beep
    create_wav_file("assets/audio/tone_440.wav", 440.0f, 1.0f, 44100);    // A4 note
    create_wav_file("assets/audio/chord.wav", 523.25f, 2.0f, 44100);      // C5 note
    
    printf("Test audio files created successfully!\n");
    return 0;
}
