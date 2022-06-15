#include <audio.h>
#include <SDL2/SDL.h>
#include <log.h>
#include <stdbool.h>

#define AUDIO_SAMPLE_RATE 48000
#define SYSTEM_SAMPLE_FORMAT AUDIO_F32SYS
#define SYSTEM_SAMPLE_SIZE 4
#define BYTES_PER_HALF_SECOND ((AUDIO_SAMPLE_RATE / 2) * SYSTEM_SAMPLE_SIZE)

static SDL_AudioStream* audio_stream = NULL;
SDL_mutex* audio_stream_mutex;
SDL_AudioSpec audio_spec;
SDL_AudioSpec request;
SDL_AudioDeviceID audio_dev;

INLINE void lock_audiostream_mutex() {
  SDL_LockMutex(audio_stream_mutex);
}

INLINE void unlock_audiostream_mutex() {
  SDL_UnlockMutex(audio_stream_mutex);
}

void audio_callback(void* userdata, Uint8* stream, int length) {
  int gotten = 0;
  lock_audiostream_mutex();
  int available = SDL_AudioStreamAvailable(audio_stream);

  if (available > 0) {
    gotten = SDL_AudioStreamGet(audio_stream, stream, length);
  }
  unlock_audiostream_mutex();

  int gotten_samples = (int)(gotten / sizeof(float));
  float* out = (float*)stream;
  out += gotten_samples;

  for (int i = gotten_samples; i < length / sizeof(float); i++) {
    float sample = 0;
    *out++ = sample;
  }
}

void init_audio() {
  adjust_sample_rate(AUDIO_SAMPLE_RATE);
  memset(&request, 0, sizeof(request));

  request.freq = AUDIO_SAMPLE_RATE;
  request.format = SYSTEM_SAMPLE_FORMAT;
  request.channels = 2;
  request.samples = 1024;
  request.callback = audio_callback;
  request.userdata = NULL;

  audio_dev = SDL_OpenAudioDevice(NULL, 0, &request, &audio_spec, 0);

  if (audio_dev == 0) {
    logfatal("Failed to initialize SDL audio: %s", SDL_GetError());
  }

  SDL_PauseAudioDevice(audio_dev, false);

  audio_stream_mutex = SDL_CreateMutex();
  if (!audio_stream_mutex) {
    logfatal("Unable to initialize mutex");
  }
}

void adjust_sample_rate(int sample_rate) {
  lock_audiostream_mutex();
  if (audio_stream != NULL) {
    SDL_FreeAudioStream(audio_stream);
  }

  audio_stream = SDL_NewAudioStream(AUDIO_S16SYS, 2, sample_rate, SYSTEM_SAMPLE_FORMAT, 2, AUDIO_SAMPLE_RATE);
  unlock_audiostream_mutex();
}

void push_sample(s16 left, s16 right) {
  s16 samples[2] = {
    left,
    right
  };

  int available_bytes = SDL_AudioStreamAvailable(audio_stream);
  if (available_bytes < BYTES_PER_HALF_SECOND) {
    SDL_AudioStreamPut(audio_stream, samples, 2 * sizeof(s16));
  }
}