/*
MIT License

Copyright(c) 2020 Futurewei Cloud

    Permission is hereby granted,
    free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

    The above copyright notice and this permission notice shall be included in all copies
    or
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS",
    WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER
    LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include <streambuf>

#include <skvhttp/common/Binary.h>
#include <skvhttp/common/Common.h>
// #include <skvhttp/common/Serialization.h>
// #include <skvhttp/mpack/MPackSerialization.h>

namespace skv::http {

const size_t MAX_SERIALIZED_SIZE_CPP_DEC_FLOATS = 100;

template<typename WriterT, size_t bufSize>
class DFWriteStreamBuf : public std::streambuf {
public:

    DFWriteStreamBuf(WriterT& writer): _writer(writer), _sz(0) {}

    std::streamsize xsputn(const char_type* s, std::streamsize count) {
        std::memcpy(_data + _sz, s, count);
        _sz += count;
        return count;
    }

    std::streambuf::int_type sputc(char_type ch) {
        _data[_sz] = ch;
        ++_sz;
        return ch;
    }

    ~DFWriteStreamBuf() {
        typename WriterT::Binary bin(_data, _sz, [](){});
        _writer.write(bin);
    }

    WriterT& _writer;
    char _data[bufSize];
    size_t _sz;
};

template<typename ReaderT>
class DFReadStreamBuf : public std::streambuf {
public:
    DFReadStreamBuf(ReaderT& reader): _reader(reader), _index{0} {
        typename ReaderT::Binary bin;
        _reader.read(bin);
        _data = (char *) bin.data();
    }

    std::streamsize xsgetn(char_type* s, std::streamsize count) {
        std::memcpy(s, _data +_index, count);
        _index += count;
        return count;
    }

    std::streambuf::int_type sbumpc() {
        char_type ch = (char_type) _data[_index];
        ++_index;
        return (int_type) ch;
    }

    ReaderT& _reader;
    char *_data;
    size_t _index;
};

}