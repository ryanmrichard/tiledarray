#ifndef NUMERIC_H_
#define NUMERIC_H_

#include <coordinate_system.h>
#include <iosfwd>
#include <cmath>
#include <functional>
#include <boost/operators.hpp>
#include <boost/array.hpp>

namespace TiledArray {

  // Forward declaration of TiledArray Permutation.
  template <unsigned int DIM>
  class Permutation;
  template <unsigned int DIM, typename T>
  boost::array<T,DIM> operator^(const Permutation<DIM>&, const boost::array<T, static_cast<std::size_t>(DIM) >&);
  template <unsigned int DIM, typename T>
  boost::array<T,DIM> operator ^=(boost::array<T, static_cast<std::size_t>(DIM) >&, const Permutation<DIM>&);

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  class ArrayCoordinate;

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  bool operator<(const ArrayCoordinate<T,DIM,Tag,CS>&, const ArrayCoordinate<T,DIM,Tag,CS>&);

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  bool operator==(const ArrayCoordinate<T,DIM,Tag,CS>& c1, const ArrayCoordinate<T,DIM,Tag,CS>& c2);

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  std::ostream& operator<<(std::ostream& output, const ArrayCoordinate<T,DIM,Tag,CS>& c);

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  ArrayCoordinate<T,DIM,Tag,CS> operator^(const Permutation<DIM>& P, const ArrayCoordinate<T,DIM,Tag,CS>& C);

  /// Array Coordinate Tag strut: It is used to ensure type safety between different tiling domains.
  template<unsigned int Level>
  struct LevelTag { };

  /// ArrayCoordinate represents coordinates of a point in a DIM-dimensional orthogonal lattice).
  ///
  /// The purpose of Tag is to create multiple instances of the class
  /// with identical mathematical behavior but distinct types to allow
  /// overloading in end-user classes.
  template <typename I, unsigned int DIM, typename Tag, typename CS = CoordinateSystem<DIM> >
  class ArrayCoordinate :
      boost::addable< ArrayCoordinate<I,DIM,Tag,CS>,                // point + point
      boost::subtractable< ArrayCoordinate<I,DIM,Tag,CS>,           // point - point
      boost::less_than_comparable1< ArrayCoordinate<I,DIM,Tag,CS>,  // point < point
      boost::equality_comparable1< ArrayCoordinate<I,DIM,Tag,CS>,   // point == point
      boost::incrementable< ArrayCoordinate<I,DIM,Tag,CS>,          // point++
      boost::decrementable< ArrayCoordinate<I,DIM,Tag,CS>           // point--
      > > > > > >
  {
  public:
	typedef ArrayCoordinate<I,DIM,Tag,CS> ArrayCoordinate_;
    typedef I index;
    typedef I volume;
    typedef CS coordinate_system;
    typedef boost::array<index,DIM> Array;
    typedef typename Array::iterator iterator;
    typedef typename Array::const_iterator const_iterator;
    static const unsigned int dim() { return DIM; }

    // Constructors/Destructor
    ArrayCoordinate(const index& init_value = 0) { r_.assign(init_value); }
    template <typename InIter>
    ArrayCoordinate(InIter start, InIter finish) { std::copy(start,finish,r_.begin()); }
    ArrayCoordinate(const Array& init_values) : r_(init_values) { } // no throw
    ArrayCoordinate(const ArrayCoordinate& a) : r_(a.r_) { } // no throw
    /// Variable argument list constructor.
    ArrayCoordinate(const index c0, const index c1, ...) {
      va_list ap;
      va_start(ap, c1);

      r_[0] = c0;
      r_[1] = c1;
      for(unsigned int i = 2; i < dim(); ++i)
        r_[i] = va_arg(ap, index);

      va_end(ap);
    }
    ~ArrayCoordinate() {}

    static ArrayCoordinate_ make(const index c0, ...) {
      ArrayCoordinate_ result;
      va_list ap;
      va_start(ap, c0);

      result.r_[0] = c0;
      for(unsigned int i = 1; i < dim(); ++i)
        result.r_[i] = va_arg(ap, index);

      va_end(ap);

      return result;
    }

    /// Returns an iterator to the first coordinate
    iterator begin() {
      return r_.begin();
    }

    /// Returns a constant iterator to the first coordinate.
    const_iterator begin() const {
      return r_.begin();
    }

    /// Returns an iterator to one element past the last coordinate.
    iterator end() {
      return r_.end();
    }

    /// Returns a constant iterator to one element past the last coordinate.
    const_iterator end() const {
      return r_.end();
    }

    /// Assignment operator
    ArrayCoordinate_&
    operator =(const ArrayCoordinate_& c) {
      std::copy(c.r_.begin(), c.r_.end(), r_.begin());

      return (*this);
    }

    ArrayCoordinate_& operator++() {
      const unsigned int lsdim = * coordinate_system::begin();
      ++(r_[lsdim]);
      return *this;
    }

    ArrayCoordinate_& operator--() {
      const unsigned int lsdim = * coordinate_system::begin();
      --(r_[lsdim]);
      return *this;
    }

    /// Add operator
    ArrayCoordinate_& operator+=(const ArrayCoordinate_& c) {
      for(unsigned int d = 0; d < dim(); ++d)
        r_[d] += c.r_[d];
      return *this;
    }

    /// Subtract operator
    ArrayCoordinate_ operator-=(const ArrayCoordinate_& c) {
      for(unsigned int d = 0; d < dim(); ++d)
        r_[d] -= c.r_[d];
      return *this;
    }

    ArrayCoordinate_ operator -() const {
      ArrayCoordinate_ ret;
      for(unsigned int d = 0; d < dim(); ++d)
        ret.r_[d] = -r_[d];
      return ret;
    }

    const index& operator[](size_t d) const
    {
#ifdef NDEBUG
      return r_[d];
#else
      return r_.at(d);
#endif
    }

    index& operator[](size_t d)
    {
#ifdef NDEBUG
      return r_[d];
#else
      return r_.at(d);
#endif
    }

    const Array& data() const {
      return r_;
    }

    Array& data() {
      return r_;
    }

    const ArrayCoordinate_ operator ^= (const Permutation<DIM>& p) {
      r_ = p ^ r_;
      return *this;
    }

    template <typename Archive>
    void serialize(const Archive& ar) {
      ar & r_;
    }

    friend bool operator < <>(const ArrayCoordinate_&, const ArrayCoordinate_&);
    friend bool operator == <>(const ArrayCoordinate_&, const ArrayCoordinate_&);
    friend std::ostream& operator << <>(std::ostream&, const ArrayCoordinate_&);

  private:
    /// last dimension is least significant
    Array r_;
  };

  template <typename Coord>
  Coord make_coord(const typename Coord::index c0, ...) {
    Coord result;
    va_list ap;
    va_start(ap, c0);

    result[0] = c0;
    for(unsigned int i = 1; i < Coord::dim(); ++i)
      result[i] = va_arg(ap, typename Coord::index);

    va_end(ap);

    return result;
  }

  /// Swap the data of c1 with c2.
  template <typename T, unsigned int DIM, typename Tag, typename CS>
  void swap(ArrayCoordinate<T,DIM,Tag,CS>& c1, ArrayCoordinate<T,DIM,Tag,CS>& c2) { // no throw
    boost::swap(c1.data(), c2.data());
  }

  namespace detail {
    /// Compare each element in the array to make sure it is
    template <typename T, unsigned int DIM, typename CS, typename L = std::less<T> >
    struct Less {
      bool operator ()(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
        L l;
        for(unsigned int i = 0; i < DIM; ++i)
          if(! l(a1[i], a2[i]))
            return false;
        return true; // all members of c1 are less than c2
      }
    }; // struct less

    template <typename T, unsigned int DIM, typename CS>
    bool less(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
      Less<T,DIM,CS> l;
      return l(a1, a2);
    }

    template <typename T, unsigned int DIM, typename CS, typename L>
    bool less(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
      Less<T,DIM,CS,L> l;
      return l(a1, a2);
    }

    /// Compare ArrayCoordinates Lexicographically.
    template <typename T, unsigned int DIM, typename CS, typename L = std::less<T> >
    struct LexLess {
      bool operator ()(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
        // Get order iterators.
        typename CS::const_iterator it = CS::begin();
        const typename CS::const_iterator end = CS::end();
        L l;
        for(; it != end; ++it) {
          if(l(a2[*it], a1[*it]))
            return false;
          else if(l(a1[*it], a2[*it]))
            return true;
        }
        return false; // all elements were equal
      }
    }; // struct LexLess

    template <typename T, unsigned int DIM, typename CS>
    bool lex_less(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
      LexLess<T,DIM,CS> l;
      return l(a1, a2);
    }

    template <typename T, unsigned int DIM, typename CS, typename L>
    bool lex_less(const boost::array<T,DIM>& a1, const boost::array<T,DIM>& a2) {
      LexLess<T,DIM,CS, L> l;
      return l(a1, a2);
    }
  } // namespace detail

  /// Compare ArrayCoordinates Lexicographically.
  template <typename T, unsigned int DIM, typename Tag, typename CS >
  bool operator<(const ArrayCoordinate<T,DIM,Tag,CS>& c1, const ArrayCoordinate<T,DIM,Tag,CS>& c2) {
    return detail::lex_less<T,DIM,CS>(c1.data(),c2.data());
  }

  template <typename T, unsigned int DIM, typename Tag, typename CS>
  bool operator==(const ArrayCoordinate<T,DIM,Tag,CS>& c1, const ArrayCoordinate<T,DIM,Tag,CS>& c2) {
    return c1.r_ == c2.r_;
  }

  /// Permute an ArrayCoordinate
  template <typename T, unsigned int DIM, typename Tag, typename CS>
  ArrayCoordinate<T,DIM,Tag,CS> operator ^(const Permutation<DIM>& perm, const ArrayCoordinate<T,DIM,Tag,CS>& c) {
    ArrayCoordinate<T,DIM,Tag,CS> result(c);
    return result ^= perm;
  }

  template <typename I, unsigned int DIM, typename Tag, typename CS>
  std::ostream& operator<<(std::ostream& output, const ArrayCoordinate<I,DIM,Tag,CS>& c) {
    output << "{";
    for(unsigned int dim = 0; dim < DIM - 1; ++dim)
      output << c[dim] << ", ";
    output << c[DIM - 1] << "}";
    return output;
  }


/*
  /// compute the volume of the orthotope bounded by the origin and C
  template <typename T, unsigned int DIM, typename Tag, typename CS>
  typename ArrayCoordinate<T,DIM,Tag,CS>::volume volume(const ArrayCoordinate<T,DIM,Tag,CS>& C) {
    return volume<T,DIM>(C.data());
  }
*/
  /// compute dot product between 2 arrays
  template <typename I, unsigned long int DIM>
  I dot_product(const boost::array<I,DIM>& A, const boost::array<I,DIM>& B) {
    I result = 0;
    for(unsigned int dim = 0; dim < DIM; ++dim)
      result += A[dim] * B[dim];
    return result;
  }

} // namespace TiledArray

#endif /*NUMERIC_H_*/
