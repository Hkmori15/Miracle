#ifndef AUDIO_H
#define AUDIO_H

/**
   * Initialize the audio system
   * @return 0 on success and -1 on failure
**/

int audio_init(void);

/**
   * Load a sound file and return buffer ID
   * @param path Path to audio file - supports: WAV, OGG, FLAC, MP3 etc
   * @return Buffer ID on success and 0 on failure
**/

unsigned int audio_load_sound(const char* path);

/**
   * Generate a test sine wave sound
   * @return Buffer ID on success and 0 on failure
   * Was use for debugging :D
**/

unsigned int audio_generate_test_sound(void);

/**
   * Play a sound from buffer
   * @param buffer_id Buffer ID returned from audio_load_sound()
   * @return Source ID on success and 0 on failure
**/

unsigned int audio_play_sound(unsigned int buffer_id);

/**
   * Stop a playing sound
   * @param source_id Source ID returned from audio_play_sound()
**/

void audio_stop_sound(unsigned int source_id);

/**
   * Set listener pos and orientation for example 3D effect
   * @param pos Listener position
   * @param forward Forward vector
   * @param up Up vector
**/

void audio_set_listener(float pos[3], float forward[3], float up[3]);

/**
   * Set sound source pos
   * @param source_id Source ID
   * @param pos Position
**/

void audio_set_source_position(unsigned int source_id, float pos[3]);

/**
   * Delete an audio buffer
   * @param buffer_id Buffer ID to delete
**/

void audio_delete_buffer(unsigned int buffer_id);

/**
   * Shutdown the audio system
**/

void audio_shutdown(void);

#endif // AUDIO_H
