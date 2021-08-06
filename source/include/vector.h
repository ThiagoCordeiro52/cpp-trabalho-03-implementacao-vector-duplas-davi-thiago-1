#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <exception>    // std::out_of_range
#include <iostream>     // std::cout, std::endl
#include <memory>       // std::unique_ptr
#include <iterator>     // std::advance, std::begin(), std::end(), std::ostream_iterator
#include <algorithm>    // std::copy, std::equal, std::fill
#include <initializer_list> // std::initializer_list
#include <cassert>      // assert()
#include <limits>       // std::numeric_limits<T>
#include <cstddef>      // std::size_t

/// Sequence container namespace.
namespace sc {
    /// Implements tha infrastrcture to support a bidirectional iterator.
    template < class T >
    class MyForwardIterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
        public:
            typedef MyForwardIterator self_type;   //!< Alias to iterator.
            // Below we have the iterator_traits common interface
            typedef std::ptrdiff_t difference_type; //!< Difference type used to calculated distance between iterators.
            typedef T value_type;           //!< Value type the iterator points to.
            typedef T* pointer;             //!< Pointer to the value type.
            typedef T& reference;           //!< Reference to the value type.
            typedef const T& const_reference;           //!< Reference to the value type.
            typedef std::bidirectional_iterator_tag iterator_category; //!< Iterator category.

            // TODO: create all methods here.

        private:
            pointer m_ptr; //!< The raw pointer.
    };

    /// This class implements the ADT list with dynamic array.
    /*!
     * sc::vector is a sequence container that encapsulates dynamic size arrays.
     *
     * The elements are stored contiguously, which means that elements can
     * be accessed not only through iterators, but also using offsets to
     * regular pointers to elements.
     * This means that a pointer to an element of a vector may be passed to
     * any function that expects a pointer to an element of an array.
     *
     * \tparam T The type of the elements.
     */
    template < typename T >
    class vector
    {
        //=== Aliases
        public:
            using size_type = unsigned long; //!< The size type.
            using value_type = T;            //!< The value type.
            using pointer = value_type*;     //!< Pointer to a value stored in the container.
            using reference = value_type&;   //!< Reference to a value stored in the container.
            using const_reference = const value_type&; //!< Const reference to a value stored in the container.

            // using iterator = MyForwardIterator< value_type >; //!< The iterator, instantiated from a template class.
            // using const_iterator = MyForwardIterator< const value_type >; //!< The const_iterator, instantiated from a template class.

        public:
            //=== [I] SPECIAL MEMBERS (6 OF THEM)
            explicit vector( size_type value = 0 )
                : m_end {value},
                  m_capacity {value},
                  m_storage {new T[value]} {

            };
            virtual ~vector( void ) {};
            vector( const vector & vetor) 
                : m_end {vetor.size()},
                  m_capacity {vetor.m_capacity},
                  m_storage {new T[m_end]} {
                std::copy(vetor.cbegin(), vetor.cend(), m_storage.get());
            };
            vector( std::initializer_list<T> values )
                : m_end {values.size()},
                  m_capacity {m_end},
                  m_storage {new T[m_end]} {
                std::copy(values.begin(), values.end(), m_storage.get());
            }

            vector( vector && );
            template < typename InputItr >
            vector( InputItr, InputItr );

            vector & operator=( const vector & );
            vector & operator=( vector && );

            //=== [II] ITERATORS
            iterator begin( void );
            iterator end( void );
            const_iterator cbegin( void ) const;
            const_iterator cend( void ) const;

            // [III] Capacity
            size_type size( void ) const {
                return m_end;
            }
            size_type capacity( void ) const {
                return m_capacity;
            };
            bool empty( void ) const {
                return m_end == 0;
            }

            // [IV] Modifiers
            void clear( void );
            void push_front( const_reference );
            void push_back( const_reference );
            void pop_back( void );
            void pop_front( void );

            iterator insert( iterator pos_ , const_reference value_ );
            iterator insert( const_iterator pos_ , const_reference value_ );

            template < typename InputItr >
            iterator insert( iterator pos_ , InputItr first_, InputItr last_ );
            template < typename InputItr >
            iterator insert( const_iterator pos_ , InputItr first_, InputItr last_ );

            iterator insert( iterator pos_, const std::initializer_list< value_type >& ilist_ );
            iterator insert( const_iterator pos_, const std::initializer_list< value_type >& ilist_ );

            void reserve( size_type );
            void shrink_to_fit( void );

            void assign( size_type count_, const_reference value_ );
            void assign( const std::initializer_list<T>& ilist );
            template < typename InputItr >
            void assign( InputItr first, InputItr last );

            iterator erase( iterator first, iterator last );
            iterator erase( const_iterator first, const_iterator last );

            iterator erase( const_iterator pos );
            iterator erase( iterator pos );

            // [V] Element access
            const_reference back( void ) const;
            const_reference front( void ) const;
            reference back( void );
            reference front( void );
            const_reference operator[]( size_type pos ) const {
                return m_storage.get()[pos];
            }
            reference operator[]( size_type pos ) {
                return m_storage.get()[pos];
            }

            const_reference at( size_type ) const;
            reference at( size_type );
            pointer data( void );
            const_reference data( void ) const;

            // [VII] Friend functions.
            friend std::ostream & operator<<( std::ostream & os_, const vector<T> & v_ )
            {
                // O que eu quero imprimir???
                os_ << "{ ";
                for( auto i{0u} ; i < v_.m_capacity ; ++i )
                {
                    if ( i == v_.m_end ) os_ << "| ";
                    os_ << v_.m_storage[ i ] << " ";
                }
                os_ << "}, m_end=" << v_.m_end << ", m_capacity=" << v_.m_capacity;

                return os_;
            }
            friend void swap( vector<T> & first_, vector<T> & second_ )
            {
                // enable ADL
                using std::swap;

                // Swap each member of the class.
                swap( first_.m_end,      second_.m_end      );
                swap( first_.m_capacity, second_.m_capacity );
                swap( first_.m_storage,  second_.m_storage  );
            }

        private:
            bool full( void ) const;

            size_type m_end;                //!< The list's current size (or index past-last valid element).
            size_type m_capacity;           //!< The list's storage capacity.
            std::unique_ptr<T[]> m_storage; //!< The list's data storage area.
            // T *m_storage;                   //!< The list's data storage area.
    };

    // [VI] Operators
    template <typename T>
    bool operator==( const vector<T> &, const vector<T>& );
    template <typename T>
    bool operator!=( const vector<T> &, const vector<T>& );

} // namespace sc.
#endif
