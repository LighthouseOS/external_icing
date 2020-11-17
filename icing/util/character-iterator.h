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

#ifndef ICING_UTIL_CHARACTER_ITERATOR_H_
#define ICING_UTIL_CHARACTER_ITERATOR_H_

#include "icing/util/i18n-utils.h"

namespace icing {
namespace lib {

class CharacterIterator {
 public:
  explicit CharacterIterator(std::string_view text)
      : CharacterIterator(text, 0, 0) {}

  CharacterIterator(std::string_view text, int utf8_index, int utf16_index)
      : text_(text), utf8_index_(utf8_index), utf16_index_(utf16_index) {}

  // Moves from current position to the character that includes the specified
  // UTF-8 index.
  // REQUIRES: desired_utf8_index <= text_.length()
  // desired_utf8_index is allowed to point one index past the end, but no
  // further.
  bool AdvanceToUtf8(int desired_utf8_index);

  // Moves from current position to the character that includes the specified
  // UTF-8 index.
  // REQUIRES: 0 <= desired_utf8_index
  bool RewindToUtf8(int desired_utf8_index);

  // Advances current position to desired_utf16_index.
  // REQUIRES: desired_utf16_index <= text_.utf16_length()
  // desired_utf16_index is allowed to point one index past the end, but no
  // further.
  bool AdvanceToUtf16(int desired_utf16_index);

  // Rewinds current position to desired_utf16_index.
  // REQUIRES: 0 <= desired_utf16_index
  bool RewindToUtf16(int desired_utf16_index);

  int utf8_index() const { return utf8_index_; }
  int utf16_index() const { return utf16_index_; }

  bool operator==(const CharacterIterator& rhs) const {
    return text_ == rhs.text_ && utf8_index_ == rhs.utf8_index_ &&
           utf16_index_ == rhs.utf16_index_;
  }

 private:
  std::string_view text_;
  int utf8_index_;
  int utf16_index_;
};

}  // namespace lib
}  // namespace icing

#endif  // ICING_UTIL_CHARACTER_ITERATOR_H_