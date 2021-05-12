#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "OpenSSLAdapter.h"
#include "SSLSocket.h"

// =============== //
// public function //
// =============== //

CSSLSocket::CSSLSocket( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, SSLProtocols Protocols )
// constructor
{
	// IP address
	memset( m_szRemoteHostIP, '\0', MAX_IP_STR_LEN );
	strcpy_s( m_szRemoteHostIP, MAX_IP_STR_LEN, szRemoteHostIP );

	m_nRemoteHostPort = nRemoteHostPort;
	m_Protocols		  = Protocols;
	m_bSecure		  = bSecure;
	m_bConnectSuccess = FALSE;
	m_Socket		  = INVALID_SOCKET;
	m_pSSL			  = NULL;
	m_pSSLContext	  = NULL;
}

CSSLSocket::~CSSLSocket( void )
// distructor
{
	DeinitSocket();
}

BOOL CSSLSocket::DataAvailable( void )
// Data available on channel
{
	return m_bConnectSuccess;
}

INT CSSLSocket::Receive( BYTE buffer[], INT nLength, INT timeout )
// Receive data from the network channel with a specified timeout
// Note: NOT support timeout now
{
	if( m_bSecure == TRUE ) {
		return CSSLAdapter::SSL_read( m_pSSL, buffer, nLength );
	}
	else {
		return ::recv( m_Socket, ( CHAR * )( void * )buffer, nLength, 0 );
	}
}

INT CSSLSocket::Send( BYTE buffer[], INT nLength )
// Send data on the network channel to the broker
{
	if( m_bSecure == TRUE ) {
		return CSSLAdapter::SSL_write( m_pSSL, buffer, nLength );
	}
	else {
		return ::send( m_Socket, ( CHAR * )( void * )buffer, nLength, 0 );
	}
}

void CSSLSocket::Close( void )
// Close the network channel
{
	DeinitSocket();
}

void CSSLSocket::Connect( void )
// Connect to remote server
{
	printf( "Start create client\n" );
	printf( "Connect to %s:%d\n", m_szRemoteHostIP, m_nRemoteHostPort );

	// create socket
	m_Socket = CreateSocket( m_szRemoteHostIP, m_nRemoteHostPort );

	// check socket result
	if( m_Socket < 0 ) {
		printf( "Create socket fail\n" );
		return;
	}

	printf( "Create socket success\n" );

	// no SSL request, it will not init SSL
	if( m_bSecure == TRUE ) {
		m_bConnectSuccess = InitSSL();
	}
	else {
		m_bConnectSuccess = TRUE;
	}
}

// ================ //
// private function //
// ================ //

BOOL CSSLSocket::InitSSL( void )
// init SSL, if init fail return FALSE
{
	// SSL init
	CSSLAdapter::OPENSSL_init_ssl( 0, NULL );

	// Create SSL_CTX
	if( ( m_pSSLContext = CreateSSLContext() ) == NULL ) {
		printf( "Create ssl context fail\n" );
		return FALSE;
	}
	printf( "Create ssl context success\n" );

	// Start to build ssl connection
	m_pSSL = CSSLAdapter::SSL_new( m_pSSLContext );

	printf( "SSL version %s\n", CSSLAdapter::SSL_get_version( m_pSSL ) );

	CSSLAdapter::SSL_set_fd( m_pSSL, m_Socket );

	INT nConnectResult = CSSLAdapter::SSL_connect( m_pSSL );
	if( nConnectResult <= 0 ) {
		printf( "SSL connect fail\n" );
		printf( "Socket Error code: %d\n", WSAGetLastError() );
		printf( "Socket %d\n", m_Socket );

		CHAR   buffer[ 256 ] = {};
		u_long err			 = CSSLAdapter::ERR_get_error();
		while( err != 0 ) {
			CSSLAdapter::ERR_error_string_n( err, buffer, sizeof( buffer ) );
			printf( "*** %s\n", buffer );

			err = CSSLAdapter::ERR_get_error();
		}
		return FALSE;
	}
	printf( "SSL connect success\n" );

	return TRUE;
}

void CSSLSocket::DeinitSocket( void )
// free the socket resouse
{
	// clean SSL setting
	if( m_bSecure == TRUE ) {
		if( m_pSSL != NULL ) {
			// SSL close
			CSSLAdapter::SSL_shutdown( m_pSSL );

			// Free resource
			CSSLAdapter::SSL_free( m_pSSL );
		}

		if( m_pSSLContext != NULL ) {
			// free context
			CSSLAdapter::SSL_CTX_free( m_pSSLContext );
		}
	}

	if( m_Socket != INVALID_SOCKET ) {
		// close socket
		closesocket( m_Socket );
	}
}

SOCKET CSSLSocket::CreateSocket( CHAR szRemoteHostIP[], INT port )
// create normal socket
{
	SOCKET			   fd;
	struct sockaddr_in addr;

	// create socket
	fd = ::socket( AF_INET, SOCK_STREAM, 0 );
	if( fd < 0 ) {
		return INVALID_SOCKET;
	}
	// TCP connect to server
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family		 = AF_INET;							// IPv4
	addr.sin_port		 = ::htons( port );					// port
	addr.sin_addr.s_addr = ::inet_addr( szRemoteHostIP );	// IP address
	if( ::connect( fd, ( struct sockaddr * )&addr, sizeof( addr ) ) < 0 ) {
		// close socket when connect fail
		::closesocket( fd );
		return INVALID_SOCKET;
	}

	return fd;
}

SSL_CTX *CSSLSocket::CreateSSLContext()
// create ssl context
{
	const SSL_METHOD *method;
	SSL_CTX *		  ctx;
	// Support only TLSv1.2
	method = CSSLAdapter::TLS_client_method();
	// Create context
	ctx = CSSLAdapter::SSL_CTX_new( method );

	return ctx;
}