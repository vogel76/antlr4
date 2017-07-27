/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#pragma once

#include "antlr4-common.h"

namespace antlrcpp {

  class ANTLR4CPP_PUBLIC Arrays {
  public:

    static std::string listToString(const std::vector<std::string> &list, const std::string &separator);

    template <typename T>
    static bool equals(const std::vector<T> &a, const std::vector<T> &b) {
      if (a.size() != b.size())
        return false;

      return std::equal(a.cbegin(), a.cend(), b.cbegin());
    }

    template <typename T, size_t S>
    static bool equals(const std::array<T, S> &a, const std::array<T, S> &b) {
      return std::equal(a.cbegin(), a.cend(), b.cbegin());
    }

    template <typename T>
    static bool less(const std::vector<T> &a, const std::vector<T> &b) {
      return std::lexicographical_compare(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
        std::less<T>());
    }

    template <typename T, size_t S>
    static bool less(const std::array<T, S>& a, const std::array<T, S>& b)
      {
      return std::lexicographical_compare(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
        std::less<T>());
      }

    template <typename T>
    static bool less(const std::vector<Ref<T>>& a, const std::vector<Ref<T>>& b)
      {
      return std::lexicographical_compare(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
        is_less_ref<T>());
      }

    template <typename T, size_t S>
    static bool less(const std::array<Ref<T>, S>& a, const std::array<Ref<T>, S>& b)
      {
      return std::lexicographical_compare(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
        is_less_ref<T>());
      }

    template <class T>
    struct is_less_ptr
      {
      bool operator()(const T* ptr1, const T* ptr2) const
        {
        std::less<T> op;
        return op(*ptr1, *ptr2);
        }
      };

    template <class T>
    struct is_less_ref
      {
      bool operator()(const Ref<T>& ptr1, const Ref<T>& ptr2) const
        {
        std::less<T> op;
        return op(*ptr1.get(), *ptr2.get());
        }
      };

    template <class T>
    struct is_equal_ptr
      {
      bool operator()(const T* ptr1, const T* ptr2) const
        {
        if(ptr1 == ptr2)
          return true;

        return *ptr1 == *ptr2;
        }
      };

    template <class T>
    struct is_equal_ref
      {
      bool operator()(const Ref<T>& a, const Ref<T>& b) const
        {
        if (!a && !b)
          return true;
        if (!a || !b)
          return false;
        if (a == b)
          return true;

        return *a == *b;
        }
      };

    template <typename T>
    static bool equals(const std::vector<T *> &a, const std::vector<T *> &b) {
      if (a.size() != b.size())
        return false;

      return std::equal(a.cbegin(), a.cend(), b.cbegin(), is_equal_ptr<T>());
    }

    template <typename T>
    static bool equals(const std::vector<Ref<T>> &a, const std::vector<Ref<T>> &b) {
      if (a.size() != b.size())
        return false;

      return std::equal(a.cbegin(), a.cend(), b.cbegin(), is_equal_ref<T>());
    }

    template <typename T, size_t S>
    static bool equals(const std::array<Ref<T>, S> &a, const std::array<Ref<T>, S> &b) {
      if (a.size() != b.size())
        return false;

      return std::equal(a.cbegin(), a.cend(), b.cbegin(), is_equal_ref<T>());
    }

    template <typename T>
    static std::string toString(const std::vector<T> &source) {
      std::string result = "[";
      bool firstEntry = true;
      for (auto &value : source) {
        result += value.toString();
        if (firstEntry) {
          result += ", ";
          firstEntry = false;
        }
      }
      return result + "]";
    }

    template <typename T>
    static std::string toString(const std::vector<Ref<T>> &source) {
      std::string result = "[";
      bool firstEntry = true;
      for (auto &value : source) {
        result += value->toString();
        if (firstEntry) {
          result += ", ";
          firstEntry = false;
        }
      }
      return result + "]";
    }

    template <typename T>
    static std::string toString(const std::vector<T *> &source) {
      std::string result = "[";
      bool firstEntry = true;
      for (auto value : source) {
        result += value->toString();
        if (firstEntry) {
          result += ", ";
          firstEntry = false;
        }
      }
      return result + "]";
    }

  };

  template <>
  std::string Arrays::toString(const std::vector<antlr4::tree::ParseTree *> &source);
}
