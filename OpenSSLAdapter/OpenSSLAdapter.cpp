#include <openssl\ssl.h>
#include <openssl\err.h>
#include "OpenSSLAdapter.h"

int CSSLAdapter::OPENSSL_init_ssl( unsigned __int64 opts, const OPENSSL_INIT_SETTINGS *settings )
{
	return ::OPENSSL_init_ssl( opts, settings );
}

int CSSLAdapter::SSL_read( SSL *ssl, void *buf, int num )
{
	return ::SSL_read( ssl, buf, num );
}

int CSSLAdapter::SSL_write( SSL *ssl, const void *buf, int num )
{
	return ::SSL_write( ssl, buf, num );
}

const SSL_METHOD *CSSLAdapter::TLS_client_method( void )
{
	return ::TLS_client_method();
}

SSL_CTX *CSSLAdapter::SSL_CTX_new( const SSL_METHOD *meth )
{
	return ::SSL_CTX_new( meth );
}

SSL *CSSLAdapter::SSL_new( SSL_CTX *ctx )
{
	return ::SSL_new( ctx );
}

int CSSLAdapter::SSL_set_fd( SSL *s, int fd )
{
	return ::SSL_set_fd( s, fd );
}

int CSSLAdapter::SSL_connect( SSL *ssl )
{
	return ::SSL_connect( ssl );
}

const char *CSSLAdapter::SSL_get_version( const SSL *s )
{
	return ::SSL_get_version( s );
}

unsigned long CSSLAdapter::ERR_get_error( void )
{
	return ::ERR_get_error();
}

void CSSLAdapter::ERR_error_string_n( unsigned long e, char *buf, size_t len )
{
	::ERR_error_string_n( e, buf, len );
}

int CSSLAdapter::SSL_shutdown( SSL *s )
{
	return ::SSL_shutdown( s );
}

void CSSLAdapter::SSL_free( SSL *ssl )
{
	::SSL_free( ssl );
}

void CSSLAdapter::SSL_CTX_free( SSL_CTX *ctx )
{
	::SSL_CTX_free( ctx );
}
