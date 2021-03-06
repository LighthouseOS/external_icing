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

// Various portability macros, type definitions, and inline functions.

#ifndef ICING_TEXT_CLASSIFIER_LIB3_UTILS_BASE_PORT_H_
#define ICING_TEXT_CLASSIFIER_LIB3_UTILS_BASE_PORT_H_

namespace libtextclassifier3 {

#if defined(__GNUC__) && \
    (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))

// For functions we want to force inline.
// Introduced in gcc 3.1.
#define TC3_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))

// For functions we don't want to inline, e.g., to keep code size small.
#define TC3_ATTRIBUTE_NOINLINE __attribute__((noinline))

#elif defined(_MSC_VER)
#define TC3_ATTRIBUTE_ALWAYS_INLINE __forceinline
#else

// Other compilers will have to figure it out for themselves.
#define TC3_ATTRIBUTE_ALWAYS_INLINE
#define TC3_ATTRIBUTE_NOINLINE
#endif

}  // namespace libtextclassifier3

#endif  // ICING_TEXT_CLASSIFIER_LIB3_UTILS_BASE_PORT_H_
