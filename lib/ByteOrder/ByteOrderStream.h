#pragma once
#ifndef BYTEORDERSTREAM_H
#define BYTEORDERSTREAM_H

//LitteEndian: The least significant byte (LSB) value is at the lowest address. The other bytes follow in increasing order of significance.
//BigEndian:   The most  significant byte (MSB) value is stored at the memory location with the lowest address, the next byte value in significance is stored at the following memory location and so on

#ifndef HOST_BYTE_ORDER_LITTLE
    // секция эвристического определения настроек машины.
    #if defined(__sparc) || defined(__sparc__) \
       || defined(_POWER) || defined(__powerpc__) \
       || defined(__ppc__) || defined(__hpux) \
       || defined(_MIPSEB) || defined(_POWER) \
       || defined(__s390__)
    #define HOST_BYTE_ORDER_LITTLE 0
    #else
    #define HOST_BYTE_ORDER_LITTLE 1
    #endif
#endif
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

/**
 * \brief Класс, позволяющий переставлять порядок байт, в соответствии с переданным параметром OrderLittleEndian.
 *
 * Если порядок байт, переданный параметром, и текущий порядок аппаратной платформы совпадают, не делается ничего.
 *
 * Используется в ByteOrderStream* классах.
 */
template <bool OrderLittleEndian=true, bool OrderSwap=OrderLittleEndian!=bool(HOST_BYTE_ORDER_LITTLE)>
class ByteSwapper
{
public:

    static inline uint64_t swapBytes(uint64_t source)
    {
        return 0
            | ((source & 0x00000000000000ffULL) << 56)
            | ((source & 0x000000000000ff00ULL) << 40)
            | ((source & 0x0000000000ff0000ULL) << 24)
            | ((source & 0x00000000ff000000ULL) << 8)
            | ((source & 0x000000ff00000000ULL) >> 8)
            | ((source & 0x0000ff0000000000ULL) >> 24)
            | ((source & 0x00ff000000000000ULL) >> 40)
            | ((source & 0xff00000000000000ULL) >> 56);
    }
    static inline int64_t swapBytes(int64_t source)
    {
        return swapBytes(*reinterpret_cast<uint64_t*>(&source));
    }

    static inline uint32_t swapBytes(uint32_t source)
    {
        return 0
            | ((source & 0x000000ff) << 24)
            | ((source & 0x0000ff00) << 8)
            | ((source & 0x00ff0000) >> 8)
            | ((source & 0xff000000) >> 24);
    }
    static inline int32_t swapBytes(int32_t source)
    {
        return swapBytes(*reinterpret_cast<uint32_t*>(&source));
    }

    static inline uint16_t swapBytes(uint16_t source)
    {
        return 0
            | ((source & 0x00ff) << 8)
            | ((source & 0xff00) >> 8);
    }
    static inline int16_t swapBytes(int16_t source)
    {
        return swapBytes(*reinterpret_cast<uint16_t*>(&source));
    }
    static inline int8_t swapBytes(int8_t source)
    {
        return source;
    }
    static inline uint8_t swapBytes(uint8_t source)
    {
        return source;
    }
    static inline bool swapBytes(bool source)
    {
        return source;
    }
    static inline float swapBytes(float source)
    {
        union {
            float f;
            uint32_t u;
        } s;
        s.f = source;
        s.u = swapBytes(s.u);
        return s.f;
    }
    static inline double swapBytes(double source)
    {
        union {
            double f;
            uint64_t u;
        } s;
        s.f = source;
        s.u = swapBytes(s.u);
        return s.f;
    }
    template <typename T>
    static inline void swapBytesChecked(T& source) {
        if (OrderSwap)
            source = swapBytes(source);
    }
};


/**
 * \brief Обертка над стандартным потоком для изменения порядка байт.
 *
 * Класс оборачивает работу с переданным ostream потоком.
 * Потоковые операторы << будут осуществлять бинарную запись в соответствии с параметром OrderLittleEndian.
 */
template <bool OrderLittleEndian=true>
class ByteOrderStreamOut
{
    ByteOrderStreamOut& operator = (const ByteOrderStreamOut&);
    ByteOrderStreamOut(const ByteOrderStreamOut&);
    ByteOrderStreamOut(ByteOrderStreamOut&&);
    std::ostream& m_stream;
public:

    inline ByteOrderStreamOut(std::ostream& stream)
        :m_stream(stream)
    {

    }

    template <typename T>
    inline ByteOrderStreamOut& operator << (T data){
        ByteSwapper<OrderLittleEndian>::swapBytesChecked(data);
        m_stream.write((const char*)&data, sizeof(T));
        return *this;
    }

    inline ByteOrderStreamOut& operator << (const std::string& data){
        size_t size = data.size();
        *this << (uint32_t(size));
        for (size_t i=0; i < size; ++i){
            *this << data[i];
        }
        return *this;
    }

    template <typename T>
    inline ByteOrderStreamOut& operator << (const std::vector<T>& data){
        size_t size = data.size();
        *this << (uint32_t(size));
        for (size_t i=0; i < size; ++i){
            *this << data[i];
        }
        return *this;
    }
    template <typename T>
    inline ByteOrderStreamOut& Write(const T* data, size_t size){
        for (size_t i =0; i< size; ++i)
            *this << data[i];
        return *this;
    }
};

/**
 * \brief Обертка над стандартным потоком ввода для изменения порядка байт.
 *
 * Класс оборачивает работу с переданным istream потоком.
 * Потоковые операторы >> будут осуществлять бинарное чтение в соответствии с параметром OrderLittleEndian.
 */
template <bool OrderLittleEndian=true>
class ByteOrderStreamIn : public ByteSwapper<OrderLittleEndian>
{

    ByteOrderStreamIn& operator = (const ByteOrderStreamIn&);
    ByteOrderStreamIn(const ByteOrderStreamIn&);
    ByteOrderStreamIn(ByteOrderStreamIn&&);
    std::istream& m_stream;
public:


    inline ByteOrderStreamIn(std::istream& stream)
        :m_stream(stream)
    {
    }

    template <typename T>
    inline ByteOrderStreamIn& operator >> (T& data){
        m_stream.read((char*)&data, sizeof(T));;
        ByteSwapper<OrderLittleEndian>::swapBytesChecked(data);
        return *this;
    }

    inline ByteOrderStreamIn& operator >> (std::string& data){
        uint32_t size=0;
        *this >> size;
        data.resize(size);
        for (size_t i=0; i < size; ++i){
            *this >> data[i];
        }
        return *this;
    }

    template <typename T>
    inline ByteOrderStreamIn& operator >> (std::vector<T>& data){
        uint32_t size=0;
        *this >> size;
        data.resize(size);
        for (size_t i=0; i < size; ++i){
            *this >> data[i];
        }
        return *this;
    }
    template <typename T>
    inline ByteOrderStreamIn& Read(T* data, size_t size){
        for (size_t i =0; i< size; ++i)
            *this >> data[i];
        return *this;
    }
};

/**
 * \brief Вспомогательный класс для записи байтового потока в строку.
 *
 * Внутри просто создается std::ostringstream.
 */
class ByteOrderStreamOutString : public ByteOrderStreamOut<true>
{
    std::string m_str;
    std::ostringstream m_strStream;
public:
    ByteOrderStreamOutString()
        :ByteOrderStreamOut<true>(m_strStream) // passing unitialized.
        ,m_strStream(m_str)
    {
    }
    const std::string& string() {
        return m_str;
    }
};
/**
 * \brief Вспомогательный класс для чтения байтового потока из строки.
 *
 * Внутри просто создается std::istringstream.
 */
class ByteOrderStreamInString : public ByteOrderStreamIn<true>
{
    std::string m_str;
    std::istringstream m_strStream;
public:
    ByteOrderStreamInString()
        :ByteOrderStreamIn<true>(m_strStream) // passing unitialized.
        ,m_strStream(m_str)
    {
    }
    void setString(const std::string& str) {
        m_str = str;
    }
};


#endif
