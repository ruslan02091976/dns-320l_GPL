/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "CurlAgent.hh"

#include "Error.hh"
#include "Header.hh"

#include "util/log/Log.hh"
#include "util/DataStream.hh"
#include "util/File.hh"

#include <boost/throw_exception.hpp>

// dependent libraries
#include <curl/curl.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <sstream>
#include <streambuf>
#include <iostream>

#include <signal.h>

namespace {

using namespace gr::http ;
using namespace gr ;
//#define _DEBUG
#ifdef _DEBUG
static http::StringResponse m_srsp;
#endif
std::size_t ReadStringCallback( void *ptr, std::size_t size, std::size_t nmemb, std::string *data )
{
	assert( ptr != 0 ) ;
	assert( data != 0 ) ;

	std::size_t count = std::min( size * nmemb, data->size() ) ;
	if ( count > 0 )
	{
		std::memcpy( ptr, &(*data)[0], count ) ;
		data->erase( 0, count ) ;
	}
	
	return count ;
}

std::size_t ReadFileCallback( void *ptr, std::size_t size, std::size_t nmemb, File *file )
{
	assert( ptr != 0 ) ;
	assert( file != 0 ) ;

	std::size_t count = std::min(
		static_cast<std::size_t>(size * nmemb),
		static_cast<std::size_t>(file->Size() - file->Tell()) ) ;
	assert( count <= std::numeric_limits<std::size_t>::max() ) ;
	
	if ( count > 0 )
		file->Read( static_cast<char*>(ptr), static_cast<std::size_t>(count) ) ;
	
	return count ;
}

} // end of local namespace

namespace gr { namespace http {

struct CurlAgent::Impl
{
	CURL			*curl ;
	std::string		location ;
	std::string		range;
} ;

static struct curl_slist* SetHeader( CURL* handle, const Header& hdr );

CurlAgent::CurlAgent() :
	m_pimpl( new Impl )
{
	m_pimpl->curl = ::curl_easy_init();
}

void CurlAgent::Init()
{
	::curl_easy_reset( m_pimpl->curl ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYPEER,	0L ) ; 
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYHOST,	0L ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADERFUNCTION,	&CurlAgent::HeaderCallback ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_WRITEHEADER ,	this ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADER, 			0L ) ;
}

CurlAgent::~CurlAgent()
{
	::curl_easy_cleanup( m_pimpl->curl );
}

std::size_t CurlAgent::HeaderCallback( void *ptr, size_t size, size_t nmemb, CurlAgent *pthis )
{
	char *str = reinterpret_cast<char*>(ptr) ;
	std::string line( str, str + size*nmemb ) ;
	
	static const std::string loc = "Location: " ;
	std::size_t pos = line.find( loc ) ;
	if ( pos != line.npos )
	{
		std::size_t end_pos = line.find( "\r\n", pos ) ;
		pthis->m_pimpl->location = line.substr( loc.size(), end_pos - loc.size() ) ;
	}

	static const std::string range = "Range: " ;
	pos = line.find( range ) ;
	if ( pos != line.npos )
	{
		std::size_t end_pos = line.find( "\r\n", pos ) ;
		pthis->m_pimpl->range = line.substr( range.size(), end_pos - range.size() ) ;
	}
	
	return size*nmemb ;
}

void CurlAgent::DumpDataStream( )
{
#ifdef _DEBUG
	if(m_srsp.Response().length() <= 8096)
		Log( "string response = %1%", m_srsp.Response(), log::info ) ;
#endif
}

std::size_t CurlAgent::Receive( void* ptr, size_t size, size_t nmemb, DataStream *recv )
{
	assert( recv != 0 ) ;
#ifdef _DEBUG
	m_srsp.Write( static_cast<char*>(ptr), size * nmemb );
#endif
	return recv->Write( static_cast<char*>(ptr), size * nmemb ) ;
}

long CurlAgent::ExecCurl(
	const std::string&	url,
	DataStream			*dest,
	const http::Header&	hdr)
{
	return ExecCurl(url, dest, hdr, false);
}

long CurlAgent::ExecCurl(
	const std::string&	url,
	DataStream			*dest,
	const http::Header&	hdr,
	bool putData )
{
	CURL *curl = m_pimpl->curl ;
	assert( curl != 0 ) ;
#ifdef _DEBUG
	m_srsp.Clear();
#endif
	char error[CURL_ERROR_SIZE] = {} ;
#ifdef _DEBUG
	::curl_easy_setopt(curl, CURLOPT_VERBOSE,		1 );
	//::curl_easy_setopt(curl, CURLOPT_TIMEOUT, 	60 ) ;//to test large file resume upload.
#endif
	::curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT,       1 ) ;
#ifdef _DEBUG
	if(putData == true)
		::curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME,        30 ) ;
	else
#endif
		::curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME,        120 ) ;
	::curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, 	error ) ;
	::curl_easy_setopt(curl, CURLOPT_URL, 			url.c_str());
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,	&CurlAgent::Receive ) ;
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA,		dest ) ;

	struct curl_slist *slist = SetHeader( m_pimpl->curl, hdr ) ;
//	dest->Clear() ;
	CURLcode curl_code = ::curl_easy_perform(curl);

	// get the HTTP response code
	long http_code = 0;
	::curl_easy_getinfo(curl,	CURLINFO_RESPONSE_CODE, &http_code);
	//Trace( "HTTP response %1%", http_code ) ;
	Log( "HTTP response = %1%", http_code, log::info ) ;

	// reset the curl buffer to prevent it from touch our "error" buffer
	::curl_easy_setopt(curl,	CURLOPT_ERRORBUFFER, 	0 ) ;
	
	// only throw for libcurl errors
	if ( curl_code != CURLE_OK )
	{
/*		BOOST_THROW_EXCEPTION(
			Error()
				<< CurlCode( curl_code )
				<< Url( url )
				<< CurlErrMsg( error )
				<< HttpHeader( hdr )
		) ;
*/
		Log( "ExecCurl error: curl_code = %1%, url = %2%,  error = %3%",  curl_code , url, error , log::info ) ;
	}
#ifdef _DEBUG	
	DumpDataStream();
#endif
	return http_code ;
}

long CurlAgent::Put(
	const std::string&		url,
	const std::string&		data,
	DataStream				*dest,
	const Header&			hdr )
{
	return Put(url, data, dest, hdr, false);
}

long CurlAgent::Put(
	const std::string&		url,
	const std::string&		data,
	DataStream				*dest,
	const Header&			hdr,
	bool largeFile)
{
	Trace("HTTP PUT \"%1%\"", url ) ;
	Log( "HTTP PUT data \"%1%\"", url,log::info ) ;
	Init() ;
	CURL *curl = m_pimpl->curl ;

	std::string put_data = data ;

	// set common options
	::curl_easy_setopt(curl, CURLOPT_UPLOAD,		1L ) ;
	::curl_easy_setopt(curl, CURLOPT_READFUNCTION,	&ReadStringCallback ) ;
	::curl_easy_setopt(curl, CURLOPT_READDATA ,		&put_data ) ;
	::curl_easy_setopt(curl, CURLOPT_INFILESIZE, 	put_data.size() ) ;

	return ExecCurl( url, dest, hdr ) ;
}

long CurlAgent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr )
{
	return Put(url, file, dest, hdr, false);
}

long CurlAgent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr,
	bool largeFile)
{
	assert( file != 0 ) ;  

	Trace("HTTP PUT \"%1%\"", url ) ;
	Log( "HTTP PUT file \"%1%\", largeFile = %2%", url,largeFile,log::info ) ;

	Init() ;
	CURL *curl = m_pimpl->curl ;

	// set common options
	::curl_easy_setopt(curl, CURLOPT_UPLOAD,			1L ) ;
	::curl_easy_setopt(curl, CURLOPT_READFUNCTION,		&ReadFileCallback ) ;
	::curl_easy_setopt(curl, CURLOPT_READDATA ,			file ) ;
	if(largeFile == true)
		::curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 	static_cast<curl_off_t>(file->Size(largeFile)) ) ;
	else
		::curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 	static_cast<curl_off_t>(file->Size()) ) ;

	return ExecCurl( url, dest, hdr, true ) ;
}

long CurlAgent::Get(
	const std::string& 		url,
	DataStream				*dest,
	const Header&			hdr )
{
	return Get(url, dest, hdr, false);
}

long CurlAgent::Get(
	const std::string& 		url,
	DataStream				*dest,
	const Header&			hdr,
	bool largeFile)
{
	Trace("HTTP GET \"%1%\"", url ) ;
	Log( "HTTP GET \"%1%\", largeFile = %2%", url,largeFile,log::info ) ;
	Init() ;

	// set get specific options
	::curl_easy_setopt(m_pimpl->curl, CURLOPT_HTTPGET, 		1L);

	return ExecCurl( url, dest, hdr ) ;
}

long CurlAgent::Post(
	const std::string& 		url,
	const std::string&		post_data,
	DataStream				*dest,
	const Header&			hdr )
{
	Trace("HTTP POST \"%1%\" with \"%2%\"", url, post_data ) ;
	Log( "HTTP POST \"%1%\" with \"%2%\"",url, post_data, log::info ) ;

	Init() ;
	CURL *curl = m_pimpl->curl ;

	// set post specific options
	::curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 			1L);
	::curl_easy_setopt(curl, CURLOPT_POST, 			1L);
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDS,	&post_data[0] ) ;
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.size() ) ;

	return ExecCurl( url, dest, hdr ) ;
}

long CurlAgent::Custom(
	const std::string&		method,
	const std::string&		url,
	DataStream				*dest,
	const Header&			hdr )
{
	Trace("HTTP %2% \"%1%\"", url, method ) ;

	CURL *curl = m_pimpl->curl ;

	::curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str() );

	return ExecCurl( url, dest, hdr ) ;
}

static struct curl_slist* SetHeader( CURL *handle, const Header& hdr )
{
	// set headers
	struct curl_slist *curl_hdr = 0 ;
    for ( Header::iterator i = hdr.begin() ; i != hdr.end() ; ++i )
		curl_hdr = curl_slist_append( curl_hdr, i->c_str() ) ;
	
	::curl_easy_setopt( handle, CURLOPT_HTTPHEADER, curl_hdr ) ;
	return curl_hdr;
}

std::string CurlAgent::RedirLocation() const
{
	return m_pimpl->location ;
}

std::string CurlAgent::GetRange() const
{
	return m_pimpl->range ;
}

std::string CurlAgent::Escape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	char *tmp = curl_easy_escape( curl, str.c_str(), str.size() ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

std::string CurlAgent::Unescape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	int r ;
	char *tmp = curl_easy_unescape( curl, str.c_str(), str.size(), &r ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

} } // end of namespace