/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#pragma once

#include "antlr4-common.h"

//#include <boost/dynamic_bitset.hpp>

#include <vector>

namespace antlrcpp 
{

/// Probably best would be to base this class on boost::dynamic_bitset.
class ANTLR4CPP_PUBLIC BitSet final : protected std::vector<bool>
  {
public:
  enum
    {
    BIT_CAPACITY = 1024
    };

  //using std::bitset<1024>::test;
  //using std::bitset<1024>::operator!=;
  //using std::bitset<1024>::operator|=;

  bool operator < (const BitSet& rhs) const
    {
    std::less<std::vector<bool>> op;
    return op(*this, rhs);
    }

  template <size_t S>
  BitSet& operator|=(const std::bitset<S>& _Right)
    {
    for(size_t i = 0; i < _Right.size(); ++i)
      if(_Right.test(i))
        setBit(i);

    return *this;
    }

  BitSet& operator|=(const BitSet& _Right)
    {
    assert(size() < BIT_CAPACITY);
    assert(_Right.size() < BIT_CAPACITY);

    if(_Right.size() > size())
      resize(_Right.size());

#ifdef _MSC_VER
    for(size_t i = 0; i < this->_Myvec.size(); ++i)
      {
      auto& word = this->_Myvec[i];
      const auto& word1 = _Right._Myvec[i];
      word |= word1;
      }
#else
   for(size_t i = 0; i < size(); ++i)
     if(_Right[i])
       (*this)[i] = true;
#endif
    return *this;
    }

  bool test(size_t i) const
    {
    assert(i < BIT_CAPACITY);
    if(i < size())
      return (*this)[i];

    return false;
    }

  void setBit(size_t i)
    {
    assert(i < BIT_CAPACITY);
    if(size() <= i)
      resize(i+1);

    (*this)[i] = true;
    }

  void clearBit(size_t i)
    {
    assert(i < BIT_CAPACITY);
    if(i < size())
      (*this)[i] = false;
    }

  size_t count() const
    {
    unsigned int setBits = 0;

#ifdef _MSC_VER
    for(size_t i = 0; i < this->_Myvec.size(); ++i)
      {
      auto v = this->_Myvec[i];
      unsigned int c; // c accumulates the total bits set in v
      for(c = 0; v; c++)
        v &= v - 1; // clear the least significant bit set

      setBits += c;
      }
#else
   for(const auto& b : *this)
     if(b) ++setBits;
#endif

    return setBits;
    }

  size_t bitCapacity() const
    {
    return 1024; /// Limit taken from old std::bitset instantiation
    }

  bool operator==(const BitSet& _Right) const
    {
    return static_cast<const std::vector<bool>&>(*this) == _Right;
    }
  bool operator!=(const BitSet& _Right) const
    {
    return (!(*this == _Right));
    }

  size_t nextSetBit(size_t pos) const
  {
    if(pos >= size())
      return INVALID_INDEX;

    for(size_t i = pos; i < size(); i++)
      if((*this)[i])
        return i;

    return INVALID_INDEX;
  }

  // Prints a list of every index for which the bitset contains a bit in true.
  friend std::wostream& operator << (std::wostream& os, const BitSet& obj)
    {
    os << "{";
    size_t total = obj.count();
    for(size_t i = 0; i < obj.bitCapacity(); i++)
      {
      if(obj.test(i))
        {
        os << i;
        --total;
        if(total > 1)
          {
          os << ", ";
          }
        }
      }

    os << "}";
    return os;
    }

  static std::string subStringRepresentation(const std::vector<BitSet>::iterator &begin,
    const std::vector<BitSet>::iterator &end)
    {
    std::string result;
    std::vector<BitSet>::iterator vectorIterator;

    for(vectorIterator = begin; vectorIterator != end; vectorIterator++)
      {
      result += vectorIterator->toString();
      }
    // Grab the end
    result += end->toString();

    return result;
    }

  std::string toString()
    {
    std::stringstream stream;
    stream << "{";
    bool valueAdded = false;
    for(size_t i = 0; i < size(); ++i)
      {
      if(test(i))
        {
        if(valueAdded)
          {
          stream << ", ";
          }
        stream << i;
        valueAdded = true;
        }
      }

    stream << "}";
    return stream.str();
    }
  };
}
