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

            MyForwardIterator( pointer ptr = nullptr ) : m_ptr{ptr}  {};
            self_type& operator=( const self_type& other ) {
                m_ptr = other.m_ptr;
                return *this;
            }
            MyForwardIterator( const self_type& other ) : m_ptr{ other.m_ptr } {}
            reference operator*( ) const {
                return *m_ptr;
            }
            self_type& operator++( ) {
                m_ptr++;
                return *this;
            }; // ++it;
            self_type operator++( int ) {
                auto old {*this};
                m_ptr++;
                return old;
            }; // it++;

            self_type& operator--( ) {
                --m_ptr;
                return *this;
            }
            self_type operator--( int ) {
                auto old {*this};
                m_ptr--;
                return old;
            }
            
            friend self_type operator+( difference_type difference, self_type vec) {
                return self_type{difference + vec.m_ptr};
            };
            friend self_type operator+( self_type vec, difference_type difference ) {
                return self_type{vec.m_ptr + difference };
            };
            friend difference_type operator-( self_type vec1, self_type vec2 ) {
                return vec1.m_ptr - vec2.m_ptr;
            }
            // friend self_type operator-( difference_type difference, self_type vec ) {
            //     return self_type{difference - vec.m_ptr};
            // }
            friend self_type operator-( self_type vec, difference_type difference ) {
                return self_type{vec.m_ptr - difference};
            }
            bool operator==( const self_type& other) const {
                return other.m_ptr == m_ptr;
            };
            bool operator!=( const self_type& other) const {
                return other.m_ptr != m_ptr;
            };

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

            using iterator = MyForwardIterator< value_type >; //!< The iterator, instantiated from a template class.
            using const_iterator = MyForwardIterator< const value_type >; //!< The const_iterator, instantiated from a template class.

        public:
            //=== [I] SPECIAL MEMBERS (6 OF THEM)
            explicit vector( size_type value = 0 )
                : m_end {value},
                  m_capacity {value},
                  m_storage {new T[value]} {

            };
            /**
             * @brief Called when the vector is destruct, does nothing because the unique_pointer deletes the data automaticly
             */
            virtual ~vector( void ) {};
            vector( const vector & vec) 
                : m_end {vec.m_end},
                  m_capacity {vec.m_capacity},
                  m_storage {new T[m_end]} {
                std::copy(vec.cbegin(), vec.cend(), m_storage.get());
            };
            /**
             * @brief Contructs a vector with the values of a initializer list
             *
             * @param ilist the initializer list to get the values from
             */
            vector( std::initializer_list<T> ilist )
                : m_end {ilist.size()},
                  m_capacity {m_end},
                  m_storage {new T[m_end]} {
                std::copy(ilist.begin(), ilist.end(), m_storage.get());
            }
            
            template < typename InputItr >
            vector( InputItr first, InputItr last) {
                auto auxiliaryFirst = first;
                int counter {0};
                while(auxiliaryFirst != last) {
                    auxiliaryFirst++;
                    counter++;
                };
                m_storage = std::unique_ptr<T[]>( new T[counter] );
                m_end = counter;
                m_capacity = counter;
                std::copy(first, last, m_storage.get());
            };

            /**
             * @brief Copies the values of vec to this vector
             *
             * @param vec the vector to copy the values from
             *
             * @return this vector with the new values
             */
            vector & operator=( const vector & vec ) {
                if ( this != &vec ) {
                    if ( m_capacity != vec.m_capacity ) {
                        m_storage.reset();
                        m_storage = std::unique_ptr<T[]>( new T[vec.m_capacity] );
                    }
                    std::copy(vec.cbegin(), vec.cend(), m_storage.get());

                    m_end = vec.m_end;
                    m_capacity = vec.m_capacity;
                }

                return *this;
            }
            /**
             * @brief Copies the values of ilist to this vector
             *
             * @param ilist the initializer list to copy the values from
             *
             * @return this vector with the new values
             */
            vector & operator=( std::initializer_list<T> ilist ) {
                if (m_capacity < ilist.size()) {
                    m_storage.reset();
                    m_storage = std::unique_ptr<T[]>( new T[ilist.size()] );
                    m_capacity = ilist.size();
                }
                m_end = ilist.size();
                std::copy(ilist.begin(), ilist.end(), m_storage.get());

                return *this;
            }

            //=== [II] ITERATORS
            iterator begin( void ) {
                return iterator{m_storage.get()};
            };
            iterator end( void ) {
                return iterator{m_storage.get() + m_end};
            };
            /**
             * @return a const iterator to the begin of the vector
             */
            const_iterator cbegin( void ) const {
                return const_iterator{m_storage.get()};
            }
            /**
             * @return a const iterator to the position after the end of the vector
             */
            const_iterator cend( void ) const {
                return const_iterator{m_storage.get() + m_end};
            }

            // [III] Capacity
            /**
             * @return the size of the vector
             */
            size_type size( void ) const {
                return m_end;
            }
            size_type capacity( void ) const {
                return m_capacity;
            };
            /**
             * @return whether the vector is empty or not 
             */
            bool empty( void ) const {
                return m_end == 0;
            }

            // [IV] Modifiers
            void clear( void ) {
                m_end = 0;
            };
            void push_front( const_reference value) {
                // Verificar se ha espaco para novo elemento.
                if (m_end + 1 >= m_capacity) {
                    m_capacity*=2;
                    std::unique_ptr<T[]> new_storage {new T[m_capacity]};
                    // Copies values of the vector to the begining of the new storage
                    std::copy(begin(), end(), new_storage.get() + 1);
                    m_end++;
                    m_storage[0] = value;
                } else {
                    // Realizar a insercao de fato.
                    m_storage[ m_end ] = value;
                    m_end++;
                }
            };
            void push_back( const_reference value ) {
                // Verificar se ha espaco para novo elemento.
                if (m_end == m_capacity) {
                    m_capacity*=2;
                    std::unique_ptr<T[]> new_storage {new T[m_capacity]};
                    // Copies values of the vector to the begining of the new storage
                    std::copy(begin(), end(), new_storage.get());
                    m_end++;
                    m_storage[m_end - 1] = value;
                } else {
                    // Realizar a insercao de fato.
                    m_storage[ m_end ] = value;
                    m_end++;
                }
            };
            /**
             * @brief removes the last element of the vector
             */
            void pop_back( void ) {
                if (m_end == 0)
                    throw std::runtime_error("pop_back(): cannot use this method on an empty vector");
                m_end--;
            }
            void pop_front( void ) {
                if (m_end == 0)
                    throw std::runtime_error("pop_back(): cannot use this method on an empty vector");
                for ( auto i {0u}; i < m_end - 1; i++ ) {
                    m_storage[i] = m_storage[i + 1];
                }
                m_end--;
            };

            // does not work if pos_ > m_end
            /**
             * @brief Inserts value at pos
             *
             * @param pos the position to insert
             * @param value the value to be inserted
             *
             * @return the new position of value
             */
            iterator insert( iterator pos , const_reference value ) {
                auto pos_ {(size_type)std::distance(begin(), pos)};
                create_space( pos_, 1 );
                m_storage[pos_] = value;

                return begin() + pos_;
            }
            /**
             * @brief Inserts value at pos
             *
             * @param pos the position to insert
             * @param value the value to be inserted
             *
             * @return the new position of value
             */
            iterator insert( const_iterator pos, const_reference value ) {
                auto pos_ {(size_type)std::distance(cbegin(), pos)};
                create_space( pos_, 1 );
                m_storage[pos_] = value;

                return begin() + pos_;
            }

            /**
             * @brief Insert the values of the range [first, last) at pos
             *
             * @tparam InputItr an iterator type
             * @param pos the position to insert the values
             * @param first an iterator to the begining of the range
             * @param last an iterator to the position after the end of the range
             *
             * @return the new position of the first value inserted
             */
            template < typename InputItr >
            iterator insert( iterator pos, InputItr first, InputItr last ) {
                auto pos_ {(size_type)std::distance(begin(), pos)};
                create_space( pos_, std::distance(first, last) );
                std::copy(first, last, begin() + pos_);

                return begin() + pos_;
            }
            /**
             * @brief Insert the values of the range [first, last) at pos
             *
             * @tparam InputItr an iterator type
             * @param pos the position to insert the values
             * @param first an iterator to the begining of the range
             * @param last an iterator to the position after the end of the range
             *
             * @return the new position of the first value inserted
             */
            template < typename InputItr >
            iterator insert( const_iterator pos, InputItr first, InputItr last ) {
                auto pos_ {(size_type)std::distance(cbegin(), pos)};
                create_space( pos_, std::distance(first, last) );
                std::copy(first, last, begin() + pos_);

                return begin() + pos_;
            }

            /**
             * @brief Insert the values of ilist at pos
             *
             * @param pos the position to insert the values
             * @param ilist the initializer list to get the values from
             *
             * @return the new position of the first value inserted
             */
            iterator insert( iterator pos, const std::initializer_list< value_type >& ilist ) {
                auto pos_ {(size_type)std::distance(begin(), pos)};
                create_space( pos_, ilist.size() );
                std::copy(ilist.begin(), ilist.end(), begin() + pos_);

                return begin() + pos_;
            }
            /**
             * @brief Insert the values of ilist at pos
             *
             * @param pos the position to insert the values
             * @param ilist the initializer list to get the values from
             *
             * @return the new position of the first value inserted
             */
            iterator insert( const_iterator pos, const std::initializer_list< value_type >& ilist ) {
                auto pos_ {(size_type)std::distance(cbegin(), pos)};
                create_space( pos_, ilist.size() );
                std::copy(ilist.begin(), ilist.end(), begin() + pos_);

                return begin() + pos_;
            }

            void reserve( size_type value) {
                if(value > m_capacity) {
                    m_capacity*=2;
                    std::unique_ptr<T[]> new_storage {new T[m_capacity]};
                    // Copies values of the vector to the begining of the new storage
                    std::copy(begin(), end(), new_storage.get());
                }
            };
            /**
             * @brief Adjusts the capacity of the array to be equal to the size
             */
            void shrink_to_fit( void ) {
                if (m_end != m_capacity) {
                    std::unique_ptr<T[]> new_storage {new T[m_end]};
                    std::copy(begin(), end(), new_storage.get());
                    m_storage = std::move(new_storage);
                    m_capacity = m_end;
                }
            }

            void assign( size_type count_, const_reference value_ );
            void assign( const std::initializer_list<T>& ilist );
            template < typename InputItr >
            void assign( InputItr first, InputItr last );

            iterator erase( iterator first, iterator last ) {
                int counter = std::distance( first, last );
                auto auxiliaryFirst = first;
                while(last != this->end()) {
                    *auxiliaryFirst = *last;
                    auxiliaryFirst++;
                    last++;
                }
                m_end -= counter;
                return first; 
            };
            iterator erase( const_iterator first, const_iterator last ) {
                int counter = std::distance( first, last );
                auto auxiliaryFirst = first;
                while(last != this->end()) {
                    *auxiliaryFirst = *last;
                    auxiliaryFirst++;
                    last++;
                }
                m_end -= counter;
                return first; 
            };

            iterator erase( const_iterator pos ) {
                auto auxiliaryPos = pos;
                while(auxiliaryPos != this->end() - 1) {
                    *auxiliaryPos = *(auxiliaryPos + 1);
                    auxiliaryPos++;
                }
                m_end--;
                return pos;            
            };
            iterator erase( iterator pos ) {
                auto auxiliaryPos = pos;
                while(auxiliaryPos != this->end() - 1) {
                    *auxiliaryPos = *(auxiliaryPos + 1);
                    auxiliaryPos++;
                }
                m_end--;
                return pos;
            };

            // [V] Element access
            /**
             * @return a const reference to the last value of the vector
             */
            const_reference back( void ) const {
                if (m_end == 0)
                    throw std::runtime_error("back(): cannot use this method on an empty vector");
                return m_storage[m_end - 1];
            }
            const_reference front( void ) const {
                if ( empty() )
                    throw std::length_error ("front(): cannot use this method on an empty vecotr.");
                return m_storage[0];
            };
            /**
             * @return a reference to the last value of the vector
             */
            reference back( void ) {
                if (m_end == 0)
                    throw std::runtime_error("back(): cannot use this method on an empty vector");
                return m_storage[m_end - 1];
            }
            reference front( void ){
                if ( empty() )
                    throw std::length_error ("front(): cannot use this method on an empty vecotr.");
                return m_storage[0];    
            };
            /**
             * @brief Gets the value at pos without bound check
             *
             * @param pos the position to get the value from
             *
             * @return a const reference to the value at pos
             */
            const_reference operator[]( size_type pos ) const {
                return m_storage[pos];
            }
            /**
             * @brief Gets the value at pos without bound check
             *
             * @param pos the position to get the value from
             *
             * @return a reference to the value at pos
             */
            reference operator[]( size_type pos ) {
                return m_storage[pos];
            }

            const_reference at( size_type value ) const {
                if (!(value < size())) {
                    throw std::out_of_range("at(): Invalid position, there are no elements in this position");
                }
                return m_storage.get()[value];
            };

            reference at( size_type value) {
                if (!(value < size())) {
                    throw std::out_of_range("at(): Invalid position, there are no elements in this position");
                }
                return m_storage.get()[value];
            };

            pointer data( void ) {
                return m_storage.get();
            };
            const_reference data( void ) const {
                return m_storage.get();
            };

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
            bool full( void ) const {
                return m_end == m_capacity;
            };
            /**
             * @brief Creates an empty space of size size at position pos (this is an auxiliary method to insert)
             *
             * @see insert()
             * @param pos the postion to create an empty space
             * @param size the size of the empty space
             */
            void create_space( size_type pos, size_type size ) {
                auto new_end {m_end + size};
                if (new_end > m_capacity) {
                    do m_capacity *= 2; while (new_end > m_capacity);

                    std::unique_ptr<T[]> new_storage {new T[m_capacity]};
                    // Copies the first part of the vector to the begining of the new storage
                    std::copy(begin(), begin() + pos, new_storage.get());
                    // Copies the last part of the vector to the end of the new storage
                    std::copy(begin() + pos, end(), new_storage.get() + pos + size);
                    m_storage = std::move(new_storage);
                } else {
                    // Copies the last part of the vector to the end
                    for (auto i {m_end - 1}; i >= pos; i--)
                        m_storage[i + size] = m_storage[i];
                }
                m_end = new_end;
            }

            size_type m_end;                //!< The list's current size (or index past-last valid element).
            size_type m_capacity;           //!< The list's storage capacity.
            std::unique_ptr<T[]> m_storage; //!< The list's data storage area.
            // T *m_storage;                   //!< The list's data storage area.
    };

    // [VI] Operators
    /**
     * @brief check if two vector are equal, i.e., have the same size and the same values
     *
     * @tparam T any type
     * @param vec1 the first vector to check the equality
     * @param vec2 the seconf vector to check the equality
     *
     * @return whether vec1 is equal to vec2
     */
    template <typename T>
    bool operator==( const vector<T>& vec1, const vector<T>& vec2 ) {
        if (vec1.size() != vec2.size())
            return false;
        for (auto i {0u}; i < vec1.size(); i++) {
            if (vec1[i] != vec2[i])
                return false;
        }
        return true;
    }
    template <typename T>
    bool operator!=( const vector<T> & vec1, const vector<T>& vec2) {
        bool oneElement = false;
        if (vec1.size() != vec2.size()){
            return true;
        }
        for (auto i {0u}; i < vec1.size(); i++) {
            if (vec1[i] != vec2[i])
                oneElement = true;
        }
        return oneElement;
    };

} // namespace sc.
#endif
