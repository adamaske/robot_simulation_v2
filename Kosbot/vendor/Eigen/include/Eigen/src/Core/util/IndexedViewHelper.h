// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2017 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EIGEN_INDEXED_VIEW_HELPER_H
#define EIGEN_INDEXED_VIEW_HELPER_H

// IWYU pragma: private
#include "../InternalHeaderCheck.h"

namespace Eigen {

namespace internal {
struct symbolic_last_tag {};

struct all_t {};

}  // namespace internal

namespace placeholders {

typedef symbolic::SymbolExpr<internal::symbolic_last_tag> last_t;

/** \var last
 * \ingroup Core_Module
 *
 * Can be used as a parameter to Eigen::seq and Eigen::seqN functions to symbolically reference the last
 * element/row/columns of the underlying vector or matrix once passed to DenseBase::operator()(const RowIndices&, const
 * ColIndices&).
 *
 * This symbolic placeholder supports standard arithmetic operations.
 *
 * A typical usage example would be:
 * \code
 * using namespace Eigen;
 * using Eigen::placeholders::last;
 * VectorXd v(n);
 * v(seq(2,last-2)).setOnes();
 * \endcode
 *
 * \sa end
 */
static constexpr const last_t last;

typedef symbolic::AddExpr<symbolic::SymbolExpr<internal::symbolic_last_tag>,
                          symbolic::ValueExpr<Eigen::internal::FixedInt<1>>>
    lastp1_t;
typedef Eigen::internal::all_t all_t;

/** \var lastp1
 * \ingroup Core_Module
 *
 * Can be used as a parameter to Eigen::seq and Eigen::seqN functions to symbolically
 * reference the last+1 element/row/columns of the underlying vector or matrix once
 * passed to DenseBase::operator()(const RowIndices&, const ColIndices&).
 *
 * This symbolic placeholder supports standard arithmetic operations.
 * It is essentially an alias to last+fix<1>.
 *
 * \sa last
 */
#ifdef EIGEN_PARSED_BY_DOXYGEN
static constexpr auto lastp1 = last + fix<1>;
#else
// Using a FixedExpr<1> expression is important here to make sure the compiler
// can fully optimize the computation starting indices with zero overhead.
static constexpr lastp1_t lastp1(last + fix<1>());
#endif

/** \var end
 * \ingroup Core_Module
 * \sa lastp1
 */
static constexpr lastp1_t end = lastp1;

/** \var all
 * \ingroup Core_Module
 * Can be used as a parameter to DenseBase::operator()(const RowIndices&, const ColIndices&) to index all rows or
 * columns
 */
static constexpr Eigen::internal::all_t all;

}  // namespace placeholders

namespace internal {

// Evaluate a symbolic expression or constant given the "size" of an object, allowing
// any symbols like `last` to be evaluated.  The default here assumes a dynamic constant.
template <typename Expr, int SizeAtCompileTime, typename EnableIf = void>
struct SymbolicExpressionEvaluator {
  static constexpr Index ValueAtCompileTime = Undefined;
  static Index eval(const Expr& expr, Index /*size*/) { return static_cast<Index>(expr); }
};

// Symbolic expression with size known at compile-time.
template <typename Expr, int SizeAtCompileTime>
struct SymbolicExpressionEvaluator<Expr, SizeAtCompileTime, std::enable_if_t<symbolic::is_symbolic<Expr>::value>> {
  static constexpr Index ValueAtCompileTime =
      Expr::Derived::eval_at_compile_time(Eigen::placeholders::last = fix<SizeAtCompileTime - 1>);
  static Index eval(const Expr& expr, Index /*size*/) {
    return expr.eval(Eigen::placeholders::last = fix<SizeAtCompileTime - 1>);
  }
};

// Symbolic expression with dynamic size.
template <typename Expr>
struct SymbolicExpressionEvaluator<Expr, Dynamic, std::enable_if_t<symbolic::is_symbolic<Expr>::value>> {
  static constexpr Index ValueAtCompileTime = Undefined;
  static Index eval(const Expr& expr, Index size) { return expr.eval(Eigen::placeholders::last = size - 1); }
};

// Fixed int.
template <int N, int SizeAtCompileTime>
struct SymbolicExpressionEvaluator<FixedInt<N>, SizeAtCompileTime, void> {
  static constexpr Index ValueAtCompileTime = static_cast<Index>(N);
  static Index eval(const FixedInt<N>& /*expr*/, Index /*size*/) { return ValueAtCompileTime; }
};

//--------------------------------------------------------------------------------
// Handling of generic indices (e.g. array)
//--------------------------------------------------------------------------------

// Potentially wrap indices in a type that is better-suited for IndexedView evaluation.
template <typename Indices, int NestedSizeAtCompileTime, typename EnableIf = void>
struct IndexedViewHelperIndicesWrapper {
  using type = Indices;
  static const type& CreateIndexSequence(const Indices& indices, Index /*nested_size*/) { return indices; }
};

// Extract compile-time and runtime first, size, increments.
template <typename Indices, typename EnableIf = void>
struct IndexedViewHelper {
  static constexpr Index FirstAtCompileTime = Undefined;
  static constexpr Index SizeAtCompileTime = array_size<Indices>::value;
  static constexpr Index IncrAtCompileTime = Undefined;

  static constexpr Index first(const Indices& indices) { return static_cast<Index>(indices[0]); }
  static constexpr Index size(const Indices& indices) { return index_list_size(indices); }
  static constexpr Index incr(const Indices& /*indices*/) { return Undefined; }
};

//--------------------------------------------------------------------------------
// Handling of ArithmeticSequence
//--------------------------------------------------------------------------------

template <Index FirstAtCompileTime_, Index SizeAtCompileTime_, Index IncrAtCompileTime_>
class ArithmeticSequenceRange {
 public:
  static constexpr Index FirstAtCompileTime = FirstAtCompileTime_;
  static constexpr Index SizeAtCompileTime = SizeAtCompileTime_;
  static constexpr Index IncrAtCompileTime = IncrAtCompileTime_;

  constexpr ArithmeticSequenceRange(Index first, Index size, Index incr) : first_{first}, size_{size}, incr_{incr} {}
  constexpr Index operator[](Index i) const { return first() + i * incr(); }
  constexpr Index first() const noexcept { return first_.value(); }
  constexpr Index size() const noexcept { return size_.value(); }
  constexpr Index incr() const noexcept { return incr_.value(); }

 private:
  variable_if_dynamicindex<Index, int(FirstAtCompileTime)> first_;
  variable_if_dynamic<Index, int(SizeAtCompileTime)> size_;
  variable_if_dynamicindex<Index, int(IncrAtCompileTime)> incr_;
};

template <typename FirstType, typename SizeType, typename IncrType, int NestedSizeAtCompileTime>
struct IndexedViewHelperIndicesWrapper<ArithmeticSequence<FirstType, SizeType, IncrType>, NestedSizeAtCompileTime,
                                       void> {
  static constexpr Index EvalFirstAtCompileTime =
      SymbolicExpressionEvaluator<FirstType, NestedSizeAtCompileTime>::ValueAtCompileTime;
  static constexpr Index EvalSizeAtCompileTime =
      SymbolicExpressionEvaluator<SizeType, NestedSizeAtCompileTime>::ValueAtCompileTime;
  static constexpr Index EvalIncrAtCompileTime =
      SymbolicExpressionEvaluator<IncrType, NestedSizeAtCompileTime>::ValueAtCompileTime;

  static constexpr Index FirstAtCompileTime =
      (int(EvalFirstAtCompileTime) == Undefined) ? Index(DynamicIndex) : EvalFirstAtCompileTime;
  static constexpr Index SizeAtCompileTime =
      (int(EvalSizeAtCompileTime) == Undefined) ? Index(Dynamic) : EvalSizeAtCompileTime;
  static constexpr Index IncrAtCompileTime =
      (int(EvalIncrAtCompileTime) == Undefined) ? Index(DynamicIndex) : EvalIncrAtCompileTime;

  using Indices = ArithmeticSequence<FirstType, SizeType, IncrType>;
  using type = ArithmeticSequenceRange<FirstAtCompileTime, SizeAtCompileTime, IncrAtCompileTime>;

  static type CreateIndexSequence(const Indices& indices, Index nested_size) {
    Index first =
        SymbolicExpressionEvaluator<FirstType, NestedSizeAtCompileTime>::eval(indices.firstObject(), nested_size);
    Index size =
        SymbolicExpressionEvaluator<SizeType, NestedSizeAtCompileTime>::eval(indices.sizeObject(), nested_size);
    Index incr =
        SymbolicExpressionEvaluator<IncrType, NestedSizeAtCompileTime>::eval(indices.incrObject(), nested_size);
    return type(first, size, incr);
  }
};

template <Index FirstAtCompileTime_, Index SizeAtCompileTime_, Index IncrAtCompileTime_>
struct IndexedViewHelper<ArithmeticSequenceRange<FirstAtCompileTime_, SizeAtCompileTime_, IncrAtCompileTime_>, void> {
 public:
  using Indices = ArithmeticSequenceRange<FirstAtCompileTime_, SizeAtCompileTime_, IncrAtCompileTime_>;
  static constexpr Index FirstAtCompileTime = Indices::FirstAtCompileTime;
  static constexpr Index SizeAtCompileTime = Indices::SizeAtCompileTime;
  static constexpr Index IncrAtCompileTime = Indices::IncrAtCompileTime;
  static Index first(const Indices& indices) { return indices.first(); }
  static Index size(const Indices& indices) { return indices.size(); }
  static Index incr(const Indices& indices) { return indices.incr(); }
};

//--------------------------------------------------------------------------------
// Handling of a single index.
//--------------------------------------------------------------------------------

template <Index ValueAtCompileTime>
class SingleRange {
 public:
  static constexpr Index FirstAtCompileTime = ValueAtCompileTime;
  static constexpr Index SizeAtCompileTime = Index(1);
  static constexpr Index IncrAtCompileTime = Index(1);  // Needs to be 1 to be treated as block-like.

  constexpr SingleRange(Index v) noexcept : value_(v) {}
  constexpr Index operator[](Index) const noexcept { return first(); }
  constexpr Index first() const noexcept { return value_.value(); }
  constexpr Index size() const noexcept { return SizeAtCompileTime; }
  constexpr Index incr() const noexcept { return IncrAtCompileTime; }

 private:
  variable_if_dynamicindex<Index, int(ValueAtCompileTime)> value_;
};

template <typename T>
struct is_single_range : public std::false_type {};

template <Index ValueAtCompileTime>
struct is_single_range<SingleRange<ValueAtCompileTime>> : public std::true_type {};

template <typename SingleIndex, int NestedSizeAtCompileTime>
struct IndexedViewHelperIndicesWrapper<
    SingleIndex, NestedSizeAtCompileTime,
    std::enable_if_t<std::is_integral<SingleIndex>::value || symbolic::is_symbolic<SingleIndex>::value>> {
  static constexpr Index EvalValueAtCompileTime =
      SymbolicExpressionEvaluator<SingleIndex, NestedSizeAtCompileTime>::ValueAtCompileTime;
  static constexpr Index ValueAtCompileTime =
      (int(EvalValueAtCompileTime) == Undefined) ? Index(DynamicIndex) : EvalValueAtCompileTime;
  using type = SingleRange<ValueAtCompileTime>;
  static type CreateIndexSequence(const SingleIndex& index, Index nested_size) {
    return type(SymbolicExpressionEvaluator<SingleIndex, NestedSizeAtCompileTime>::eval(index, nested_size));
  }
};

template <int N, int NestedSizeAtCompileTime>
struct IndexedViewHelperIndicesWrapper<FixedInt<N>, NestedSizeAtCompileTime, void> {
  using type = SingleRange<Index(N)>;
  static type CreateIndexSequence(const FixedInt<N>& /*index*/) { return type(Index(N)); }
};

template <Index ValueAtCompileTime>
struct IndexedViewHelper<SingleRange<ValueAtCompileTime>, void> {
  using Indices = SingleRange<ValueAtCompileTime>;
  static constexpr Index FirstAtCompileTime = Indices::FirstAtCompileTime;
  static constexpr Index SizeAtCompileTime = Indices::SizeAtCompileTime;
  static constexpr Index IncrAtCompileTime = Indices::IncrAtCompileTime;

  static constexpr Index first(const Indices& indices) { return indices.first(); }
  static constexpr Index size(const Indices& /*indices*/) { return SizeAtCompileTime; }
  static constexpr Index incr(const Indices& /*indices*/) { return IncrAtCompileTime; }
};

//--------------------------------------------------------------------------------
// Handling of all
//--------------------------------------------------------------------------------

// Convert a symbolic 'all' into a usable range type
template <Index SizeAtCompileTime_>
class AllRange {
 public:
  static constexpr Index FirstAtCompileTime = Index(0);
  static constexpr Index SizeAtCompileTime = SizeAtCompileTime_;
  static constexpr Index IncrAtCompileTime = Index(1);
  constexpr AllRange(Index size) : size_(size) {}
  constexpr Index operator[](Index i) const noexcept { return i; }
  constexpr Index first() const noexcept { return FirstAtCompileTime; }
  constexpr Index size() const noexcept { return size_.value(); }
  constexpr Index incr() const noexcept { return IncrAtCompileTime; }

 private:
  variable_if_dynamic<Index, int(SizeAtCompileTime)> size_;
};

template <int NestedSizeAtCompileTime>
struct IndexedViewHelperIndicesWrapper<all_t, NestedSizeAtCompileTime, void> {
  using type = AllRange<Index(NestedSizeAtCompileTime)>;
  static type CreateIndexSequence(const all_t& /*indices*/, Index nested_size) { return type(nested_size); }
};

template <Index SizeAtCompileTime_>
struct IndexedViewHelper<AllRange<SizeAtCompileTime_>, void> {
  using Indices = AllRange<SizeAtCompileTime_>;
  static constexpr Index FirstAtCompileTime = Indices::FirstAtCompileTime;
  static constexpr Index SizeAtCompileTime = Indices::SizeAtCompileTime;
  static constexpr Index IncrAtCompileTime = Indices::IncrAtCompileTime;

  static Index first(const Indices& indices) { return indices.first(); }
  static Index size(const Indices& indices) { return indices.size(); }
  static Index incr(const Indices& indices) { return indices.incr(); }
};

}  // end namespace internal

}  // end namespace Eigen

#endif  // EIGEN_INDEXED_VIEW_HELPER_H
