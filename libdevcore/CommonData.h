/*
    This file is part of ethcoreminer.

    ethcoreminer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ethcoreminer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ethcoreminer.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file CommonData.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * Shared algorithms and data types.
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "Common.h"

namespace dev
{
// String conversion functions, mainly to/from hex/nibble/byte representations.

enum class WhenError
{
    DontThrow = 0,
    Throw = 1,
};

enum class HexPrefix
{
    DontAdd = 0,
    Add = 1,
};

enum class ScaleSuffix
{
    DontAdd = 0,
    Add = 1
};

/// Convert a series of bytes to the corresponding string of hex duplets.
/// @param _w specifies the width of the first of the elements. Defaults to two - enough to
/// represent a byte.
/// @example toHex("A\x69") == "4169"
template <class T>
std::string toHex(T const& _data, int _w = 2, HexPrefix _prefix = HexPrefix::DontAdd)
{
    std::ostringstream ret;
    unsigned ii = 0;
    for (auto i : _data)
        ret << std::hex << std::setfill('0') << std::setw(ii++ ? 2 : _w)
            << (int)(typename std::make_unsigned<decltype(i)>::type)i;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

/// Converts a (printable) ASCII hex character into the correspnding integer value.
/// @example fromHex('A') == 10 && fromHex('f') == 15 && fromHex('5') == 5
int fromHex(char _i, WhenError _throw);

/// Converts a (printable) ASCII hex string into the corresponding byte stream.
/// @example fromHex("41626261") == asBytes("Abba")
/// If _throw = ThrowType::DontThrow, it replaces bad hex characters with 0's, otherwise it will
/// throw an exception.
bytes fromHex(std::string const& _s, WhenError _throw = WhenError::DontThrow);

/// Converts byte array to a string containing the same (binary) data. Unless
/// the byte array happens to contain ASCII data, this won't be printable.
inline std::string asString(bytes const& _b)
{
    return std::string((char const*)_b.data(), (char const*)(_b.data() + _b.size()));
}

/// Converts a string to a byte array containing the string's (byte) data.
inline bytes asBytes(std::string const& _b)
{
    return bytes((byte const*)_b.data(), (byte const*)(_b.data() + _b.size()));
}


// Big-endian to/from host endian conversion functions.

/// Converts a templated integer value to the big-endian byte-stream represented on a templated
/// collection. The size of the collection object will be unchanged. If it is too small, it will not
/// represent the value properly, if too big then the additional elements will be zeroed out.
/// @a Out will typically be either std::string or bytes.
/// @a T will typically by unsigned, u160, u256 or bigint.
template <class T, class Out>
inline void toBigEndian(T _val, Out& o_out)
{
    static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed,
        "only unsigned types or bigint supported");  // bigint does not carry sign bit on shift
    for (auto i = o_out.size(); i != 0; _val >>= 8, i--)
    {
        T v = _val & (T)0xff;
        o_out[i - 1] = (typename Out::value_type)(uint8_t)v;
    }
}

/// Converts a big-endian byte-stream represented on a templated collection to a templated integer
/// value.
/// @a _In will typically be either std::string or bytes.
/// @a T will typically by unsigned, u160, u256 or bigint.
template <class T, class _In>
inline T fromBigEndian(_In const& _bytes)
{
    T ret = (T)0;
    for (auto i : _bytes)
        ret =
            (T)((ret << 8) | (byte)(typename std::make_unsigned<typename _In::value_type>::type)i);
    return ret;
}

/// Convenience functions for toBigEndian
inline bytes toBigEndian(u256 _val)
{
    bytes ret(32);
    toBigEndian(std::move(_val), ret);
    return ret;
}
inline bytes toBigEndian(u160 _val)
{
    bytes ret(20);
    toBigEndian(_val, ret);
    return ret;
}

/// Convenience function for toBigEndian.
/// @returns a byte array just big enough to represent @a _val.
template <class T>
inline bytes toCompactBigEndian(T _val, unsigned _min = 0)
{
    static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed,
        "only unsigned types or bigint supported");  // bigint does not carry sign bit on shift
    int i = 0;
    for (T v = _val; v; ++i, v >>= 8)
    {
    }
    bytes ret(std::max<unsigned>(_min, i), 0);
    toBigEndian(_val, ret);
    return ret;
}

/// Convenience function for conversion of a u256 to hex
inline std::string toHex(u256 val, HexPrefix prefix = HexPrefix::DontAdd)
{
    std::string str = toHex(toBigEndian(val));
    return (prefix == HexPrefix::Add) ? "0x" + str : str;
}

inline std::string toHex(uint64_t _n, HexPrefix _prefix = HexPrefix::DontAdd, int _bytes = 16)
{
    // sizeof returns the number of bytes (not the number of bits)
    // thus if CHAR_BIT != 8 sizeof(uint64_t) will return != 8
    // Use fixed constant multiplier of 16
    std::ostringstream ret;
    ret << std::hex << std::setfill('0') << std::setw(_bytes) << _n;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

inline std::string toHex(uint32_t _n, HexPrefix _prefix = HexPrefix::DontAdd, int _bytes = 8)
{
    // sizeof returns the number of bytes (not the number of bits)
    // thus if CHAR_BIT != 8 sizeof(uint64_t) will return != 4
    // Use fixed constant multiplier of 8
    std::ostringstream ret;
    ret << std::hex << std::setfill('0') << std::setw(_bytes) << _n;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

inline std::string toCompactHex(uint64_t _n, HexPrefix _prefix = HexPrefix::DontAdd)
{
    std::ostringstream ret;
    ret << std::hex << _n;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

inline std::string toCompactHex(uint32_t _n, HexPrefix _prefix = HexPrefix::DontAdd)
{
    std::ostringstream ret;
    ret << std::hex << _n;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}



// Algorithms for string and string-like collections.

/// Escapes a string into the C-string representation.
/// @p _all if true will escape all characters, not just the unprintable ones.
std::string escaped(std::string const& _s, bool _all = true);

// General datatype convenience functions.

/// Determine bytes required to encode the given integer value. @returns 0 if @a _i is zero.
template <class T>
inline unsigned bytesRequired(T _i)
{
    static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed,
        "only unsigned types or bigint supported");  // bigint does not carry sign bit on shift
    unsigned i = 0;
    for (; _i != 0; ++i, _i >>= 8)
    {
    }
    return i;
}

/// Sets environment variable.
///
/// Portable wrapper for setenv / _putenv C library functions.
bool setenv(const char name[], const char value[], bool override = false);

/// Gets a target hash from given difficulty
std::string getTargetFromDiff(double diff, HexPrefix _prefix = HexPrefix::Add);

/// Gets the difficulty expressed in hashes to target
double getHashesToTarget(std::string _target);

/// Generic function to scale a value
std::string getScaledSize(double _value, double _divisor, int _precision, std::string _sizes[],
    size_t _numsizes, ScaleSuffix _suffix = ScaleSuffix::Add);

/// Formats hashrate
std::string getFormattedHashes(double _hr, ScaleSuffix _suffix = ScaleSuffix::Add, int _precision = 2);

/// Formats hashrate
std::string getFormattedMemory(
    double _mem, ScaleSuffix _suffix = ScaleSuffix::Add, int _precision = 2);

/// Adjust string to a fixed length filling chars to the Left
std::string padLeft(std::string _value, size_t _length, char _fillChar);

/// Adjust string to a fixed length filling chars to the Right
std::string padRight(std::string _value, size_t _length, char _fillChar);

}  // namespace dev
