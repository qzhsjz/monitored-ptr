
// Copyright (c) 2010-2019 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__detail__io__io_exceptions_hpp
#define __yas__detail__io__io_exceptions_hpp

#include <yas/detail/config/config.hpp>
#include <yas/detail/io/exception_base.hpp>

namespace yas {

/***************************************************************************/

__YAS_DECLARE_EXCEPTION_TYPE(io_exception)

/***************************************************************************/

#define __YAS_THROW_BAD_ARCHIVE_INFORMATION() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "archive is corrupted or try to use \"yas::no_header\" flag");

#define __YAS_THROW_BAD_ARCHIVE_VERSION() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "incompatible archive version");

#define __YAS_THROW_BAD_ARCHIVE_TYPE() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "incompatible archive type");

#define __YAS_THROW_ARCHIVE_NO_HEADER() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "you cannot use information functions with \"yas::no_header\" flag");

#define __YAS_THROW_READ_STORAGE_SIZE_ERROR(...) \
	if ( __YAS_UNLIKELY(__VA_ARGS__) ) { \
		__YAS_THROW_EXCEPTION(::yas::io_exception, "stored value is too big to read"); \
	}

#define __YAS_THROW_READ_ERROR(...) \
	if ( __YAS_UNLIKELY(__VA_ARGS__) ) { \
		__YAS_THROW_EXCEPTION(::yas::io_exception, "can't read requested bytes"); \
	}

#define __YAS_THROW_WRITE_ERROR(...) \
	if ( __YAS_UNLIKELY(__VA_ARGS__) ) { \
		__YAS_THROW_EXCEPTION(::yas::io_exception, "can't write requested bytes"); \
	}

#define __YAS_THROW_FILE_ALREADY_EXISTS() \
    __YAS_THROW_EXCEPTION(::yas::io_exception, "file already exists");

#define __YAS_THROW_FILE_IS_NOT_EXISTS() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "file is not exists");

#define __YAS_THROW_ERROR_OPEN_FILE() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "open file error");

#define __YAS_THROW_BAD_FILE_MODE() \
	__YAS_THROW_EXCEPTION(::yas::io_exception, "bad file open mode");

#define __YAS_THROW_BAD_COMPACTED_MODE() \
    __YAS_THROW_EXCEPTION(::yas::io_exception, "incompatible compacted/non-compacted mode");

/***************************************************************************/

} // namespace yas

#endif // __yas__detail__io__io_exceptions_hpp
