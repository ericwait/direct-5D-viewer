////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_VEC
#define INCLUDE_VEC

#include "Defines.h"

#include <type_traits>
#include <stdexcept>

#undef min
#undef max

#ifdef __CUDACC__
#define MIXED_PREFIX __host__ __device__
#else
#define MIXED_PREFIX
#endif

/// Helper type definition, forces template match only on fundamental scalar types.
template <typename T>
using EnableIfScalar = typename std::enable_if<std::is_fundamental<T>::value>::type;

/// @class Vec
/// @brief A class template for representing a 3D vector with basic vector operations.
/// @tparam T The data type of the vector elements (e.g., int, float, double).
template<typename T>
class Vec
{
public:
	/// @brief Union for accessing vector elements.
	union
	{
		T e[3];  ///< Array representation of the vector elements.
		struct  
		{
			T x;  ///< X component of the vector.
			T y;  ///< Y component of the vector.
			T z;  ///< Z component of the vector.
		};
	};

	/// @brief Provides access to modify an element of the vector by index.
    /// @param i The index of the element (0-based, valid indices are 0, 1, and 2).
    /// @return A reference to the element at index `i`.
    /// @throw std::out_of_range if `i` is out of bounds (>= 3).
    MIXED_PREFIX T& operator[](size_t i) {
        if (i >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return e[i];
    }

    /// @brief Provides read-only access to an element of the vector by index.
    /// @param i The index of the element (0-based, valid indices are 0, 1, and 2).
    /// @return A const reference to the element at index `i`.
    /// @throw std::out_of_range if `i` is out of bounds (>= 3).
    MIXED_PREFIX const T& operator[](size_t i) const {
        if (i >= 3) {
            throw std::out_of_range("Index out of range");
        }
        return e[i];
    }

	/// @brief Default constructor. Initializes the vector elements to zero.
	template <typename U = T, typename = EnableIfScalar<U>>
	MIXED_PREFIX Vec() : x(0), y(0), z(0) {}

	/// @brief Constructor that initializes all elements with the same value.
	/// @param val The value to initialize each element with.
	template <typename U = T, typename = EnableIfScalar<U>>
	MIXED_PREFIX Vec(T val)
		: x(val), y(val), z(val) {}

	/// @brief Constructor that initializes vector elements from an array.
	/// @param elems Pointer to an array of 3 elements.
	template <typename U, typename = EnableIfScalar<U>>
	MIXED_PREFIX Vec(const U* elems)
		:	x(static_cast<T>(elems[0])),
			y(static_cast<T>(elems[1])),
			z(static_cast<T>(elems[2]))
	{}

	/// @brief Copy constructor from another Vec with possibly a different type.
	/// @param other The vector to copy from.
	template <typename U, typename = EnableIfScalar<U>>
	MIXED_PREFIX Vec(const Vec<U>& other)
		:	x(static_cast<T>(other.x)),
			y(static_cast<T>(other.y)),
			z(static_cast<T>(other.z))
	{}

	/// @brief Constructor that initializes the vector with specific x, y, and z values.
	/// @param x The x component.
	/// @param y The y component.
	/// @param z The z component.
	template <typename U = T, typename = EnableIfScalar<U>>
	MIXED_PREFIX Vec(T x, T y, T z)
		: x(x), y(y), z(z) {}

	/// @brief Negates each element of the vector.
	/// @return A new vector with each element negated.
	MIXED_PREFIX Vec<T> operator- () const
	{
		Vec<T> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = -e[i];

		return outVec;
	}

	/// @brief Adds a scalar to each element of the vector.
	/// @tparam U The type of the scalar.
	/// @param adder The scalar value to add.
	/// @return A new vector with each element incremented by the scalar.
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator+ (U adder) const
	{
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = e[i] + adder;

		return outVec;
	}

	/// @brief Subtracts a scalar from each element of the vector.
	/// @tparam U The type of the scalar.
	/// @param subtractor The scalar value to subtract.
	/// @return A new vector with each element decremented by the scalar.
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator- (U subtractor) const
	{
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = e[i] - subtractor;

		return outVec;
	}

	/// @brief Divides each element of the vector by a scalar.
	/// @tparam U The type of the scalar.
	/// @param divisor The scalar value to divide by.
	/// @return A new vector with each element divided by the scalar.
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator/ (U divisor) const
	{
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = e[i] / divisor;

		return outVec;
	}

	/// @brief Multiplies each element of the vector by a scalar.
	/// @tparam U The type of the scalar.
	/// @param mult The scalar value to multiply by.
	/// @return A new vector with each element multiplied by the scalar.
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator* (U mult) const
	{
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = e[i] * mult;

		return outVec;
	}

	/**
	 * @brief Adds each element of the vector with the corresponding element of another vector.
	 * @tparam U The type of the elements in the other vector.
	 * @param other The vector whose elements are to be added.
	 * @return A new vector with each element incremented by the corresponding element of the other vector.
	 */
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator+(const Vec<U>& other) const {
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i) {
			outVec.e[i] = e[i] + static_cast<typename std::common_type<T, U>::type>(other.e[i]);
		}
		return outVec;
	}

	/**
	 * @brief Subtracts each element of the vector by the corresponding element of another vector.
	 * @tparam U The type of the elements in the other vector.
	 * @param other The vector whose elements are to be subtracted.
	 * @return A new vector with each element decremented by the corresponding element of the other vector.
	 */
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator-(const Vec<U>& other) const {
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i) {
			outVec.e[i] = e[i] - static_cast<typename std::common_type<T, U>::type>(other.e[i]);
		}
		return outVec;
	}

	/**
	 * @brief Multiplies each element of the vector by the corresponding element of another vector.
	 * @tparam U The type of the elements in the other vector.
	 * @param other The vector whose elements are to be multiplied.
	 * @return A new vector with each element multiplied by the corresponding element of the other vector.
	 */
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator*(const Vec<U>& other) const {
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i) {
			outVec.e[i] = e[i] * static_cast<typename std::common_type<T, U>::type>(other.e[i]);
		}
		return outVec;
	}

	/**
	 * @brief Divides each element of the vector by the corresponding element of another vector.
	 * @tparam U The type of the elements in the other vector.
	 * @param other The vector whose elements are to be divided.
	 * @return A new vector with each element divided by the corresponding element of the other vector.
	 */
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> operator/(const Vec<U>& other) const {
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i) {
			outVec.e[i] = e[i] / static_cast<typename std::common_type<T, U>::type>(other.e[i]);
		}
		return outVec;
	}


	/// @brief Raises each element of the vector to a power.
	/// @tparam U The type of the power.
	/// @param pw The power to raise each element to.
	/// @return A new vector with each element raised to the specified power.
	template<typename U>
	MIXED_PREFIX Vec<typename std::common_type<T, U>::type> pwr(U pw) const
	{
		Vec<typename std::common_type<T, U>::type> outVec;
		for (int i = 0; i < 3; ++i)
			outVec.e[i] = pow(static_cast<double>(e[i]), pw);

		return outVec;
	}

	/**
     * @brief Adds a scalar to each element of the vector in place.
     * @tparam U The type of the scalar.
     * @param scalar The scalar value to add.
     * @return A reference to the current vector after modification.
     * 
     * This operator adds the scalar value to each element of the vector, modifying the vector in place.
     */
    template <typename U, typename = EnableIfScalar<U>>
    MIXED_PREFIX Vec<T>& operator+= (U scalar) {
        for (int i = 0; i < 3; ++i) {
            e[i] += scalar;
        }
        return *this;
    }

    /**
     * @brief Adds the corresponding elements of another vector to this vector in place.
     * @tparam U The type of the elements in the other vector.
     * @param other The vector whose elements are to be added.
     * @return A reference to the current vector after modification.
     * 
     * This operator adds the corresponding elements of another vector to this vector,
     * modifying the vector in place.
     */
    template <typename U>
    MIXED_PREFIX Vec<T>& operator+= (const Vec<U>& other) {
        for (int i = 0; i < 3; ++i) {
            e[i] += static_cast<T>(other.e[i]);
        }
        return *this;
    }

	/**
	 * @brief Compares this vector with another vector for equality.
	 * @tparam U The type of the elements in the other vector.
	 * @param other The vector to compare with.
	 * @return True if all corresponding elements are equal, false otherwise.
	 */
	template <typename U>
	MIXED_PREFIX bool operator==(const Vec<U>& other) const {
		return x == static_cast<T>(other.x) &&
			y == static_cast<T>(other.y) &&
			z == static_cast<T>(other.z);
	}

	/// @brief Computes the product of all vector elements (x * y * z).
	/// @return The product of the vector elements.
	MIXED_PREFIX T product() const
	{
		return x * y * z;
	}

	/// @brief Computes the sum of all vector elements (x + y + z).
	/// @return The sum of the vector elements.
	MIXED_PREFIX T sum() const
	{
		return x + y + z;
	}

	/// @brief Finds the maximum value among the vector's elements.
	/// @return The maximum element value.
	MIXED_PREFIX T maxValue() const
	{
		return (x > y) ? ((x > z) ? (x) : (z)) : ((y > z) ? (y) : (z));
	}

	/// @brief Finds the minimum value among the vector's elements.
	/// @return The minimum element value.
	MIXED_PREFIX T minValue() const
	{
		return (x < y) ? ((x < z) ? (x) : (z)) : ((y < z) ? (y) : (z));
	}

	/// @brief Computes the Euclidean distance to another vector.
	/// @param other The other vector.
	/// @return The Euclidean distance.
	MIXED_PREFIX double EuclideanDistanceTo(const Vec<T>& other)
	{
		return sqrt(static_cast<double>((SQR(x - other.x) + SQR(y - other.y) + SQR(z - other.z))));
	}

		/// @brief Computes the squared length of the vector (x^2 + y^2 + z^2).
	/// @return The squared length of the vector.
	MIXED_PREFIX double lengthSqr()
	{
		return SQR(x) + SQR(y) + SQR(z);
	}

	/// @brief Computes the length (magnitude) of the vector.
	/// @return The length of the vector.
	MIXED_PREFIX double length()
	{
		return sqrt(lengthSqr());
	}

	/// @brief Normalizes the vector (scales it to have length 1).
	/// @return A new vector that is the normalized version of this vector.
	MIXED_PREFIX Vec<double> normal()
	{
		return (*this) / length();
	}

	/// @brief Computes the cross product of two vectors.
	/// @tparam U Type of the first vector.
	/// @tparam V Type of the second vector.
	/// @param a The first vector.
	/// @param b The second vector.
	/// @return A new vector that is the cross product of the two vectors.
	template <typename U, typename V>
	static MIXED_PREFIX Vec<typename std::common_type<U, V>::type> cross(const Vec<U>& a, const Vec<V>& b)
	{
		Vec<typename std::common_type<U, V>::type> o;

		o.x = a.y * b.z - a.z * b.y;
		o.y = -(a.x * b.z - a.z * b.x);
		o.z = a.x * b.y - a.y * b.x;

		return o;
	}

	/// @brief Computes the dot product of two vectors.
	/// @tparam U Type of the first vector.
	/// @tparam V Type of the second vector.
	/// @param a The first vector.
	/// @param b The second vector.
	/// @return The dot product of the two vectors.
	template <typename U, typename V>
	static MIXED_PREFIX typename std::common_type<U, V>::type dot(const Vec<U>& a, const Vec<V>& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
};

// Scalar-vector binary operator definitions

/// @brief Adds a scalar to each element of the vector.
/// @tparam U The type of the scalar.
/// @tparam V The type of the vector elements.
/// @param scalar The scalar value to add.
/// @param vec The vector.
/// @return A new vector with each element incremented by the scalar.
template <typename U, typename V, typename = EnableIfScalar<U>>
MIXED_PREFIX Vec<typename std::common_type<U, V>::type> operator+(U scalar, const Vec<V>& vec)
{
	return (vec + scalar);
}

/// @brief Subtracts each element of the vector from a scalar.
/// @tparam U The type of the scalar.
/// @tparam V The type of the vector elements.
/// @param scalar The scalar value.
/// @param vec The vector.
/// @return A new vector where each element is the result of the scalar minus the corresponding vector element.
template <typename U, typename V, typename = EnableIfScalar<U>>
MIXED_PREFIX Vec<typename std::common_type<U, V>::type> operator-(U scalar, const Vec<V>& vec)
{
	return vec.subFrom(scalar);
}

/// @brief Multiplies each element of the vector by a scalar.
/// @tparam U The type of the scalar.
/// @tparam V The type of the vector elements.
/// @param scalar The scalar value.
/// @param vec The vector.
/// @return A new vector where each element is multiplied by the scalar.
template <typename U, typename V, typename = EnableIfScalar<U>>
MIXED_PREFIX Vec<typename std::common_type<U, V>::type> operator*(U scalar, const Vec<V>& vec)
{
	return (vec * scalar);
}

/// @brief Divides a scalar by each element of the vector.
/// @tparam U The type of the scalar.
/// @tparam V The type of the vector elements.
/// @param scalar The scalar value.
/// @param vec The vector.
/// @return A new vector where each element is the result of the scalar divided by the corresponding vector element.
template <typename U, typename V, typename = EnableIfScalar<U>>
MIXED_PREFIX Vec<typename std::common_type<U, V>::type> operator/(U scalar, const Vec<V>& vec)
{
	return vec.divideRight(scalar);
}



#endif