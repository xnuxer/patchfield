#include "pdmodule.h"

#include "../libpd/jni/z_jni_native_hooks.h"
#include "audio_module.h"
#include "utils/buffer_size_adapter.h"

#include <stddef.h>

static void process_pd(void *context, int sample_rate, int buffer_frames,
    int input_channels, const float *input_buffer,
    int output_channels, float *output_buffer) {
  libpd_sync_process_raw(input_buffer, output_buffer);
}

JNIEXPORT void JNICALL
Java_com_noisepages_nettoyeur_patchbay_pd_PdModule_pdInitAudio
(JNIEnv *env, jobject obj, jint input_channels, jint output_channels,
 jint sample_rate) {
  libpd_sync_init_audio(input_channels, output_channels, sample_rate);
}

JNIEXPORT jboolean JNICALL
Java_com_noisepages_nettoyeur_patchbay_pd_PdModule_hasTimedOut
(JNIEnv *env, jobject obj, jlong p) {
  buffer_size_adapter *bsa = (buffer_size_adapter *) p;
  return am_has_timed_out(bsa_get_runner(bsa));
}

JNIEXPORT jint JNICALL
Java_com_noisepages_nettoyeur_patchbay_pd_PdModule_getProtocolVersion
(JNIEnv *env, jobject obj) {
  return PATCHBAY_PROTOCOL_VERSION;
}

JNIEXPORT jlong JNICALL
Java_com_noisepages_nettoyeur_patchbay_pd_PdModule_configureModule
(JNIEnv *env, jobject obj, jint version, jint token, jint index,
 jint host_buffer_size, jint user_buffer_size,
 jint input_channels, jint output_channels) {
  return (jlong) bsa_create(version, token, index,
      host_buffer_size, user_buffer_size,
      input_channels, output_channels,
      process_pd, NULL);     
}

JNIEXPORT void JNICALL
Java_com_noisepages_nettoyeur_patchbay_pd_PdModule_release
(JNIEnv *env, jobject obj, jlong p) {
  bsa_release((buffer_size_adapter *) p);
}
