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

#ifndef ICING_INDEX_ITERATOR_DOC_HIT_INFO_ITERATOR_TEST_UTIL_H_
#define ICING_INDEX_ITERATOR_DOC_HIT_INFO_ITERATOR_TEST_UTIL_H_

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "icing/text_classifier/lib3/utils/base/status.h"
#include "icing/absl_ports/canonical_errors.h"
#include "icing/absl_ports/str_cat.h"
#include "icing/index/hit/doc-hit-info.h"
#include "icing/index/iterator/doc-hit-info-iterator.h"
#include "icing/legacy/core/icing-string-util.h"
#include "icing/schema/section.h"
#include "icing/store/document-id.h"

namespace icing {
namespace lib {

// Dummy class to help with testing. It starts with an kInvalidDocumentId doc
// hit info until an Advance is called (like normal DocHitInfoIterators). It
// will then proceed to return the doc_hit_infos in order as Advance's are
// called. After all doc_hit_infos are returned, Advance will return a NotFound
// error (also like normal DocHitInfoIterators).
class DocHitInfoIteratorDummy : public DocHitInfoIterator {
 public:
  DocHitInfoIteratorDummy() = default;
  explicit DocHitInfoIteratorDummy(std::vector<DocHitInfo> doc_hit_infos)
      : doc_hit_infos_(std::move(doc_hit_infos)) {}

  libtextclassifier3::Status Advance() override {
    if (index_ < doc_hit_infos_.size()) {
      doc_hit_info_ = doc_hit_infos_.at(index_);
      index_++;
      return libtextclassifier3::Status::OK;
    }

    return absl_ports::ResourceExhaustedError(
        "No more DocHitInfos in iterator");
  }

  void set_hit_intersect_section_ids_mask(
      SectionIdMask hit_intersect_section_ids_mask) {
    hit_intersect_section_ids_mask_ = hit_intersect_section_ids_mask;
  }

  int32_t GetNumBlocksInspected() const override {
    return num_blocks_inspected_;
  }

  void SetNumBlocksInspected(int32_t num_blocks_inspected) {
    num_blocks_inspected_ = num_blocks_inspected;
  }

  int32_t GetNumLeafAdvanceCalls() const override {
    return num_leaf_advance_calls_;
  }

  void SetNumLeafAdvanceCalls(int32_t num_leaf_advance_calls) {
    num_leaf_advance_calls_ = num_leaf_advance_calls;
  }

  std::string ToString() const override {
    std::string ret = "<";
    for (auto& doc_hit_info : doc_hit_infos_) {
      absl_ports::StrAppend(&ret, IcingStringUtil::StringPrintf(
                                      "[%d,%d]", doc_hit_info.document_id(),
                                      doc_hit_info.hit_section_ids_mask()));
    }
    absl_ports::StrAppend(&ret, ">");
    return ret;
  }

 private:
  int32_t index_ = 0;
  int32_t num_blocks_inspected_ = 0;
  int32_t num_leaf_advance_calls_ = 0;
  std::vector<DocHitInfo> doc_hit_infos_;
};

inline std::vector<DocumentId> GetDocumentIds(DocHitInfoIterator* iterator) {
  std::vector<DocumentId> ids;
  while (iterator->Advance().ok()) {
    ids.push_back(iterator->doc_hit_info().document_id());
  }
  return ids;
}

inline std::vector<DocHitInfo> GetDocHitInfos(DocHitInfoIterator* iterator) {
  std::vector<DocHitInfo> doc_hit_infos;
  while (iterator->Advance().ok()) {
    doc_hit_infos.push_back(iterator->doc_hit_info());
  }
  return doc_hit_infos;
}

}  // namespace lib
}  // namespace icing

#endif  // ICING_INDEX_ITERATOR_DOC_HIT_INFO_ITERATOR_TEST_UTIL_H_
