/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#include "support/Arrays.h"
#include "atn/SingletonPredictionContext.h"

#include "atn/ArrayPredictionContext.h"

#include <array>

using namespace antlr4::atn;

namespace
{

typedef std::vector<Ref<PredictionContext>> TPredictionContextContainer;
typedef std::vector<size_t> TReturnStateContainer;

template <class TData>
class ArrayPredictionContextImpl final : public ArrayPredictionContext
  {
  public:
    ArrayPredictionContextImpl(TPredictionContextContainer&& _parents,
      TReturnStateContainer&& _returnStates, size_t cachedHashCode) :
      ArrayPredictionContext(cachedHashCode),
      Data(std::move(_parents), std::move(_returnStates)) {}

    ArrayPredictionContextImpl(const Ref<PredictionContext>& parent,
      size_t returnState, size_t cachedHashCode) :
      ArrayPredictionContext(cachedHashCode),
      Data(parent, returnState) {}

    virtual ~ArrayPredictionContextImpl() {}

    virtual bool isEmpty() const override
      {
      // Since EMPTY_RETURN_STATE can only appear in the last position, we don't need to verify that size == 1.
      return Data.returnStates[0] == EMPTY_RETURN_STATE;
      }

    virtual size_t size() const override
      {
      return Data.returnStates.size();
      }

    virtual Ref<PredictionContext> getParent(size_t index) const override
      {
      return Data.parents[index];
      }

    virtual size_t getReturnState(size_t index) const override
      {
      return Data.returnStates[index];
      }

    virtual bool operator < (const PredictionContext &o) const override
      {
      const ArrayPredictionContextImpl<TData> *other =
        dynamic_cast<const ArrayPredictionContextImpl<TData>*>(&o);

      if(other == nullptr)
        {
        auto s = toString();
        auto s1 = o.toString();

        return s < s1;
        }

      if(this == other)
        return false;

      //auto s = toString();
      //auto s1 = other->toString();

      //return s < s1;

      if(antlrcpp::Arrays::less(Data.returnStates, other->Data.returnStates))
        return true;

      if(antlrcpp::Arrays::less(other->Data.returnStates, Data.returnStates))
        return false;

      return antlrcpp::Arrays::less(Data.parents, other->Data.parents);
      }

    virtual bool operator == (const PredictionContext &o) const override
      {
      if(this == &o)
        return true;

      const ArrayPredictionContextImpl<TData> *other =
        dynamic_cast<const ArrayPredictionContextImpl<TData>*>(&o);

      if(other == nullptr || hashCode() != other->hashCode())
        return false; // can't be same if hash is different

        return antlrcpp::Arrays::equals(Data.returnStates, other->Data.returnStates) &&
          antlrcpp::Arrays::equals(Data.parents, other->Data.parents);
      }

    virtual std::string toString() const override
      {
      if(isEmpty())
        return "[]";

      std::stringstream ss;
      ss << "[";
      for(size_t i = 0; i < Data.returnStates.size(); i++)
        {
        if(i > 0)
          ss << ", ";

        if(Data.returnStates[i] == EMPTY_RETURN_STATE)
          {
          ss << "$";
          continue;
          }

        ss << Data.returnStates[i];
        if(Data.parents[i] != nullptr)
          ss << " " << Data.parents[i]->toString();
        else
          ss << "nul";
        }
      ss << "]";
      return ss.str();
      }

    typedef ArrayPredictionContext::TParentPredictionContextStorageRange TParentPredictionContextStorageRange;
    typedef ArrayPredictionContext::TReturnStateStorageRange TReturnStateStorageRange;

    virtual TParentPredictionContextStorageRange getStorageRanges(
      TReturnStateStorageRange* rangeStorage = nullptr) const override final
      {
      if(rangeStorage != nullptr)
        {
        rangeStorage->first = Data.returnStates.data();
        rangeStorage->second = Data.returnStates.data() + Data.returnStates.size();
        }

      return TParentPredictionContextStorageRange(Data.parents.data(),
        Data.parents.data() + Data.parents.size());
      }

    TData Data;
  }; /// ArrayPredictionContextImpl 

template <size_t PARENT_SIZE, size_t RETURN_STATE_SIZE = PARENT_SIZE>
class StaticArrayPredictionContext final
  {
  public:
    StaticArrayPredictionContext(const Ref<PredictionContext>& parent,
      size_t returnState)
      {
      assert(1 == PARENT_SIZE);
      assert(1 == RETURN_STATE_SIZE);

      parents[0] = parent;
      returnStates[0] = returnState;
      }

    StaticArrayPredictionContext(TPredictionContextContainer&& _parents,
      TReturnStateContainer&& _returnStates)
      {
      assert(parents.size() == PARENT_SIZE);
      assert(returnStates.size() == RETURN_STATE_SIZE);

      for(size_t i = 0; i < _parents.size(); ++i)
        parents[i] = _parents[i];

      for(size_t i = 0; i < _returnStates.size(); ++i)
        returnStates[i] = _returnStates[i];
      }

    typedef std::array<Ref<PredictionContext>, PARENT_SIZE> TPredictionContextStorage;
    typedef std::array<size_t, RETURN_STATE_SIZE> TReturnStateStorage;

    TPredictionContextStorage parents;
    TReturnStateStorage       returnStates;
  };

class DynamicArrayPredictionContext final
  {
  public:
    DynamicArrayPredictionContext(TPredictionContextContainer&& _parents,
      TReturnStateContainer&& _returnStates) :
      parents(std::move(_parents)),
      returnStates(std::move(_returnStates))
      {
      }

    DynamicArrayPredictionContext(const Ref<PredictionContext>& parent, size_t returnState)
      {
      parents.emplace_back(parent);
      returnStates.emplace_back(returnState);
      }

    TPredictionContextContainer parents;
    TReturnStateContainer returnStates;
  };

class ABuilder
  {
  public:
    virtual Ref<ArrayPredictionContext> Build(TPredictionContextContainer&& parents,
      TReturnStateContainer&& returnStates, size_t cachedHashCode) = 0;
    virtual ~ABuilder() {}
  };

template <size_t ParentSize, size_t ReturnStateSize>
class TBuilder final : public ABuilder
  {
  public:
    virtual Ref<ArrayPredictionContext> Build(TPredictionContextContainer&& parents,
      TReturnStateContainer&& returnStates, size_t cachedHashCode) override
      {
      typedef StaticArrayPredictionContext<ParentSize, ReturnStateSize> TStorage;
      typedef ArrayPredictionContextImpl<TStorage> TPredictionContextImpl;

      return std::make_shared<TPredictionContextImpl>(
        std::move(parents), std::move(returnStates), cachedHashCode);
      }
  };

template <>
class TBuilder<0, 0> final : public ABuilder
  {
  public:
    virtual Ref<ArrayPredictionContext> Build(TPredictionContextContainer&& parents,
      TReturnStateContainer&& returnStates, size_t cachedHashCode) override
      {
      typedef ArrayPredictionContextImpl<DynamicArrayPredictionContext> TPredictionContextImpl;

      maxMaxParents = std::max(maxMaxParents, parents.size());
      maxReturnStates  = std::max(maxReturnStates, returnStates.size());

      return std::make_shared<TPredictionContextImpl>(
        std::move(parents), std::move(returnStates), cachedHashCode);
      }

    virtual ~TBuilder()
      {
      printf("MaxParents: %lu\n", maxMaxParents);
      printf("MaxReturnStates: %lu\n", maxReturnStates);
      }

    size_t maxMaxParents = 0;
    size_t maxReturnStates = 0;
  };

const size_t s0 = sizeof(PredictionContext);
const size_t s = sizeof(Ref<PredictionContext>);
const size_t s1 = sizeof(PredictionContext);
const size_t s2 = sizeof(ArrayPredictionContextImpl<DynamicArrayPredictionContext>);
const size_t s3 = sizeof(ArrayPredictionContextImpl<StaticArrayPredictionContext<1, 1>>);

enum
  {
  MAX_PARENT_SIZE = 32,
  MAX_RETURN_STATE_SIZE = 64,
  TOTAL = MAX_RETURN_STATE_SIZE * MAX_PARENT_SIZE
  };

std::unique_ptr<ABuilder> Builders[MAX_PARENT_SIZE][MAX_RETURN_STATE_SIZE];
std::unique_ptr<ABuilder> dynamicBuilder;

static bool Initialized = false;

template <size_t ParentSize, size_t MaxReturnStateSize>
class TRowBuilder
  {
  public:
    static void Fill()
      {
      assert(Builders[ParentSize - 1][MaxReturnStateSize - 1] == false);

      Builders[ParentSize - 1][MaxReturnStateSize - 1] =
        std::make_unique<TBuilder<ParentSize, MaxReturnStateSize>>();

      TRowBuilder<ParentSize, MaxReturnStateSize - 1>::Fill();
      }
  };

template <size_t ParentSize>
class TRowBuilder<ParentSize, 1>
  {
  public:
    static void Fill()
      {
      assert(Builders[ParentSize - 1][0] == false);

      Builders[ParentSize - 1][0] =
        std::make_unique<TBuilder<ParentSize, 1>>();
      }
  };

template <size_t MaxParentSize = MAX_PARENT_SIZE,
          size_t MaxReturnStateSize = MAX_RETURN_STATE_SIZE>
class TInitializer final
  {
  public:
    static void InitBuilders()
      {
      TRowBuilder<MaxParentSize, MaxReturnStateSize>::Fill();
      TInitializer<MaxParentSize - 1, MaxReturnStateSize>::InitBuilders();
      }
  };

template <size_t N>
class TInitializer<1, N> final
  {
  public:
    static void InitBuilders()
      {
      TRowBuilder<1, N>::Fill();
      }
  };

static ABuilder* GetBuilder(size_t parentSize, size_t returnStateSize)
  {
  if(Initialized == false)
    {
    dynamicBuilder = std::make_unique<TBuilder<0, 0>>();
    TInitializer<>::InitBuilders();
    Initialized = true;
    }

  if(parentSize > MAX_PARENT_SIZE || returnStateSize > MAX_RETURN_STATE_SIZE)
    return dynamicBuilder.get();

  return Builders[parentSize - 1][returnStateSize - 1].get();
  }

} /// namespace anonymous

Ref<ArrayPredictionContext> ArrayPredictionContext::makePredictionContext(
  TPredictionContextContainer&& parents, TReturnStateContainer&& returnStates)
  {
  assert(parents.empty() == false);
  assert(returnStates.empty() == false);
  assert(parents.size() == returnStates.size());

  size_t hashCode = calculateHashCode(parents, returnStates);

  ABuilder* builder = GetBuilder(parents.size(), returnStates.size());
  return builder->Build(std::move(parents), std::move(returnStates), hashCode);
  }

Ref<ArrayPredictionContext> ArrayPredictionContext::makePredictionContext(
  TPredictionContextContainer&& parents, const TReturnStateStorageRange& returnStateSource)
  {
  TReturnStateContainer temp;
  temp.insert(temp.end(), returnStateSource.first, returnStateSource.second);
  return makePredictionContext(std::move(parents), std::move(temp));
  }

Ref<ArrayPredictionContext> ArrayPredictionContext::makePredictionContext(
  const Ref<SingletonPredictionContext>& parent)
  {
  typedef StaticArrayPredictionContext<1, 1> TStorage;
  typedef ArrayPredictionContextImpl<TStorage> TPredictionContextImpl;

  size_t hashCode = calculateHashCode(parent->parent, parent->returnState);
  return std::make_shared<TPredictionContextImpl>(parent->parent,
    parent->returnState, hashCode);
  }

