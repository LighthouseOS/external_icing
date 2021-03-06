// Copyright (C) 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Utility class that provides similar calls like JNIEnv, but performs
// additional checks on them, so that it's harder to use them incorrectly.

#ifndef ICING_TEXT_CLASSIFIER_LIB3_UTILS_JAVA_JNI_HELPER_H_
#define ICING_TEXT_CLASSIFIER_LIB3_UTILS_JAVA_JNI_HELPER_H_

#include <jni.h>

#include <string>

#include "icing/text_classifier/lib3/utils/base/status.h"
#include "icing/text_classifier/lib3/utils/base/statusor.h"
#include "icing/text_classifier/lib3/utils/java/jni-base.h"

#define TC3_ENSURE_LOCAL_CAPACITY_OR_RETURN             \
  if (!EnsureLocalCapacity(env, 1)) {                   \
    TC3_LOG(ERROR) << "EnsureLocalCapacity(1) failed."; \
    return {Status::UNKNOWN};                           \
  }

#define TC3_NO_EXCEPTION_OR_RETURN      \
  if (JniExceptionCheckAndClear(env)) { \
    return {Status::UNKNOWN};           \
  }

#define TC3_NOT_NULL_OR_RETURN \
  if (result == nullptr) {     \
    return {Status::UNKNOWN};  \
  }

#define TC3_DEFINE_VARIADIC_SCOPED_LOCAL_REF_ENV_METHOD(                   \
    METHOD_NAME, RETURN_TYPE, INPUT_TYPE, POST_CHECK)                      \
  template <typename T = RETURN_TYPE>                                      \
  static StatusOr<ScopedLocalRef<T>> METHOD_NAME(                          \
      JNIEnv* env, INPUT_TYPE object, jmethodID method_id, ...) {          \
    TC3_ENSURE_LOCAL_CAPACITY_OR_RETURN;                                   \
                                                                           \
    va_list args;                                                          \
    va_start(args, method_id);                                             \
    ScopedLocalRef<T> result(                                              \
        reinterpret_cast<T>(env->METHOD_NAME##V(object, method_id, args)), \
        env);                                                              \
    POST_CHECK                                                             \
    va_end(args);                                                          \
                                                                           \
    TC3_NO_EXCEPTION_OR_RETURN;                                            \
    return result;                                                         \
  }

#define TC3_JNI_NO_CHECK \
  {}

namespace libtextclassifier3 {

class JniHelper {
 public:
  // Misc methods.
  static StatusOr<ScopedLocalRef<jclass>> FindClass(JNIEnv* env,
                                                    const char* class_name);

  static StatusOr<ScopedLocalRef<jclass>> GetObjectClass(JNIEnv* env,
                                                         jobject object);

  template <typename T = jobject>
  static StatusOr<ScopedLocalRef<T>> GetObjectArrayElement(JNIEnv* env,
                                                           jobjectArray array,
                                                           jsize index);
  static StatusOr<jmethodID> GetMethodID(JNIEnv* env, jclass clazz,
                                         const char* method_name,
                                         const char* signature);
  static StatusOr<jmethodID> GetStaticMethodID(JNIEnv* env, jclass clazz,
                                               const char* method_name,
                                               const char* signature);

  static StatusOr<jfieldID> GetFieldID(JNIEnv* env, jclass clazz,
                                       const char* field_name,
                                       const char* signature);
  static StatusOr<jfieldID> GetStaticFieldID(JNIEnv* env, jclass clazz,
                                             const char* field_name,
                                             const char* signature);

  static StatusOr<ScopedLocalRef<jobject>> GetStaticObjectField(
      JNIEnv* env, jclass class_name, jfieldID field_id);
  static StatusOr<jint> GetStaticIntField(JNIEnv* env, jclass class_name,
                                          jfieldID field_id);

  // New* methods.
  TC3_DEFINE_VARIADIC_SCOPED_LOCAL_REF_ENV_METHOD(NewObject, jobject, jclass,
                                                  TC3_NOT_NULL_OR_RETURN);
  static StatusOr<ScopedLocalRef<jobjectArray>> NewObjectArray(
      JNIEnv* env, jsize length, jclass element_class,
      jobject initial_element = nullptr);
  static StatusOr<ScopedLocalRef<jbyteArray>> NewByteArray(JNIEnv* env,
                                                           jsize length);
  static StatusOr<ScopedLocalRef<jintArray>> NewIntArray(JNIEnv* env,
                                                         jsize length);
  static StatusOr<ScopedLocalRef<jstring>> NewStringUTF(JNIEnv* env,
                                                        const char* bytes);
  static StatusOr<ScopedLocalRef<jfloatArray>> NewFloatArray(JNIEnv* env,
                                                             jsize length);

  static StatusOr<jsize> GetArrayLength(JNIEnv* env, jarray array);

  static Status SetObjectArrayElement(JNIEnv* env, jobjectArray array,
                                      jsize index, jobject val);

  static Status GetByteArrayRegion(JNIEnv* env, jbyteArray array, jsize start,
                                   jsize len, jbyte* buf);

  static Status SetByteArrayRegion(JNIEnv* env, jbyteArray array, jsize start,
                                   jsize len, const jbyte* buf);

  static Status SetIntArrayRegion(JNIEnv* env, jintArray array, jsize start,
                                  jsize len, const jint* buf);

  static Status SetFloatArrayRegion(JNIEnv* env, jfloatArray array, jsize start,
                                    jsize len, const jfloat* buf);

  // Call* methods.
  TC3_DEFINE_VARIADIC_SCOPED_LOCAL_REF_ENV_METHOD(CallObjectMethod, jobject,
                                                  jobject, TC3_JNI_NO_CHECK);
  TC3_DEFINE_VARIADIC_SCOPED_LOCAL_REF_ENV_METHOD(CallStaticObjectMethod,
                                                  jobject, jclass,
                                                  TC3_JNI_NO_CHECK);
  static Status CallVoidMethod(JNIEnv* env, jobject object, jmethodID method_id,
                               ...);
  static StatusOr<bool> CallBooleanMethod(JNIEnv* env, jobject object,
                                          jmethodID method_id, ...);
  static StatusOr<int32> CallIntMethod(JNIEnv* env, jobject object,
                                       jmethodID method_id, ...);
  static StatusOr<int64> CallLongMethod(JNIEnv* env, jobject object,
                                        jmethodID method_id, ...);
  static StatusOr<float> CallFloatMethod(JNIEnv* env, jobject object,
                                         jmethodID method_id, ...);
  static StatusOr<double> CallDoubleMethod(JNIEnv* env, jobject object,
                                           jmethodID method_id, ...);

  template <class T>
  static StatusOr<T> CallStaticIntMethod(JNIEnv* env,
                                         bool print_exception_on_error,
                                         jclass clazz, jmethodID method_id,
                                         ...);
};

template <typename T>
StatusOr<ScopedLocalRef<T>> JniHelper::GetObjectArrayElement(JNIEnv* env,
                                                             jobjectArray array,
                                                             jsize index) {
  TC3_ENSURE_LOCAL_CAPACITY_OR_RETURN;
  ScopedLocalRef<T> result(
      reinterpret_cast<T>(env->GetObjectArrayElement(array, index)), env);

  TC3_NO_EXCEPTION_OR_RETURN;
  return result;
}

template <class T>
StatusOr<T> JniHelper::CallStaticIntMethod(JNIEnv* env,
                                           bool print_exception_on_error,
                                           jclass clazz, jmethodID method_id,
                                           ...) {
  va_list args;
  va_start(args, method_id);
  jint result = env->CallStaticIntMethodV(clazz, method_id, args);
  va_end(args);

  if (JniExceptionCheckAndClear(env, print_exception_on_error)) {
    return {Status::UNKNOWN};
  }

  return result;
}

// Converts Java byte[] object to std::string.
StatusOr<std::string> JByteArrayToString(JNIEnv* env, jbyteArray array);

// Converts Java String object to UTF8-encoded std::string.
StatusOr<std::string> JStringToUtf8String(JNIEnv* env, jstring jstr);

}  // namespace libtextclassifier3

#endif  // ICING_TEXT_CLASSIFIER_LIB3_UTILS_JAVA_JNI_HELPER_H_
