#if !defined( _OPEN_SSL_ADAPTER_H____INCLUDED_ )
#define _OPEN_SSL_ADAPTER_H____INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif	  // _MSC_VER > 1000

struct ossl_init_settings_st;
struct ssl_method_st;
struct ssl_st;
struct ssl_ctx_st;

typedef struct ossl_init_settings_st OPENSSL_INIT_SETTINGS;
typedef struct ssl_method_st		 SSL_METHOD;
typedef struct ssl_st				 SSL;
typedef struct ssl_ctx_st			 SSL_CTX;

class CSSLAdapter
{
public:
	static int OPENSSL_init_ssl( unsigned __int64 opts, const OPENSSL_INIT_SETTINGS *settings );

	static int SSL_read( SSL *ssl, void *buf, int num );

	static int SSL_write( SSL *ssl, const void *buf, int num );

	static const SSL_METHOD *TLS_client_method( void );

	static SSL_CTX *SSL_CTX_new( const SSL_METHOD *meth );

	static SSL *SSL_new( SSL_CTX *ctx );

	static int SSL_set_fd( SSL *s, int fd );

	static int SSL_connect( SSL *ssl );

	static const char *SSL_get_version( const SSL *s );

	static unsigned long ERR_get_error( void );

	static void ERR_error_string_n( unsigned long e, char *buf, size_t len );

	static int SSL_shutdown( SSL *s );

	static void SSL_free( SSL *ssl );

	static void SSL_CTX_free( SSL_CTX *ctx );
};
#endif
