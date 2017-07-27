
/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#pragma once

#include "atn/PredictionContext.h"

namespace antlr4 {
namespace atn {

  class SingletonPredictionContext;

  class ANTLR4CPP_PUBLIC ArrayPredictionContext : public PredictionContext {
  public:
    typedef std::vector<Ref<PredictionContext>> TPredictionContextContainer;
    typedef std::vector<size_t> TReturnStateContainer;

    typedef std::pair<const Ref<PredictionContext>*, const Ref<PredictionContext>*>
      TParentPredictionContextStorageRange;
    typedef std::pair<const size_t*, const size_t*> TReturnStateStorageRange;

    virtual TParentPredictionContextStorageRange getStorageRanges(
      TReturnStateStorageRange* rangeStorage = nullptr) const = 0;

    /// Parent can be empty only if full ctx mode and we make an array
    /// from EMPTY and non-empty. We merge EMPTY by using null parent and
    /// returnState == EMPTY_RETURN_STATE.
    // Also here: we use a strong reference to our parents to avoid having them freed prematurely.
    //            See also SinglePredictionContext.
    static Ref<ArrayPredictionContext> makePredictionContext(TPredictionContextContainer&& parents,
      TReturnStateContainer&& returnStates);

    static Ref<ArrayPredictionContext> makePredictionContext(TPredictionContextContainer&& parents,
      const TReturnStateStorageRange& returnStateSource);

    static Ref<ArrayPredictionContext> makePredictionContext(
      const Ref<SingletonPredictionContext>& parent);

    virtual ~ArrayPredictionContext() {}

  protected:
    ArrayPredictionContext(size_t cachedHashCode) : PredictionContext(cachedHashCode) {}
  };

} // namespace atn
} // namespace antlr4

