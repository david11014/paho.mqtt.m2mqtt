#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "OpenSSLAdapter.h"
#include "SSLSocket.h"

// #define SSLSOCKET_DEBUG_SSL
// #define SSLSOCKET_DEBUG_RECEIVE
// #define SSLSOCKET_DEBUG_SEND

// =============== //
// public function //
// =============== //

CSSLSocket::CSSLSocket( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, SSLProtocols Protocols )
// constructor
{
	// IP address
	memset( m_szRemoteHostIP, '\0', MAX_IP_STR_LEN );
	strcpy_s( m_szRemoteHostIP, MAX_IP_STR_LEN, szRemoteHostIP );

	// init setting
	m_bConnected	  = FALSE;
	m_nRemoteHostPort = nRemoteHostPort;
	m_Protocols		  = Protocols;
	m_bSecure		  = bSecure;
	m_Socket		  = INVALID_SOCKET;
	m_pSSL			  = NULL;
	m_pSSLContext	  = NULL;

	memset( m_ReceiveBuffer, 0, sizeof( BYTE ) * RECV_SIZE );
	memset( m_QueueData, 0, sizeof( BYTE ) * RECV_SIZE );
	m_nQueueLen = 0;

	// stop running receive and send request
	m_bStopRequest = FALSE;

	// receive state
	m_RecvState = ERS_Idle;

	// send state
	m_SendState = ESS_Idle;

	m_RecvEndEvent = CreateEvent( NULL,		// attribute
								  FALSE,	// is manual reset
								  FALSE,	// initial state
								  NULL		// event name
	);

	m_SendEndEvent = CreateEvent( NULL,		// attribute
								  FALSE,	// is manual reset
								  FALSE,	// initial state
								  NULL		// event name
	);
}

CSSLSocket::~CSSLSocket( void )
// distructor
{
	DeinitSocket();
}

INT CSSLSocket::Receive( BYTE buffer[], INT nLength, INT timeout )
// Receive data from the network channel with a specified timeout
// Note: NOT support timeout now
{
	// clear buffer
	memset( buffer, 0, nLength * sizeof( BYTE ) );

	if( m_bConnected == FALSE || m_bStopRequest == TRUE ) {
		return 0;
	}

	// read data from queue first
	int nRemainDataCount = nLength - ReadDataFromQueue( buffer, nLength );

	// has enouth data
	if( nRemainDataCount == 0 ) {
		return nLength;
	}

	// receive all data
	int nTotalRead = 0, nReadCount = 0;
	while( TRUE )
	{
		m_RecvState = ERS_Receiving;
		// receive data
		if( m_bSecure == TRUE ) {
			nReadCount = CSSLAdapter::SSL_read( m_pSSL, ( m_ReceiveBuffer + nTotalRead ), RECV_SIZE - nTotalRead + 1 );
		}
		else {
			nReadCount = ::recv( m_Socket, ( CHAR * )( void * )( m_ReceiveBuffer + nTotalRead ), RECV_SIZE - nTotalRead + 1, 0 );
		}
		m_RecvState = ERS_Idle;

		// notice receive function sending is end, and force terminate receive process
		if( m_bStopRequest == TRUE ) {
			SetEvent( m_RecvEndEvent );
			return 0;
		}

		// add receive data count
		nTotalRead += nReadCount;

		// check is data receive finish or not firs
		if( nTotalRead >= nLength ) {
			break;
		}
		// no data to read
		else if( nReadCount == 0 ) {
			break;
		}
		// receive fail
		else if( nReadCount < 0 ) {
#ifdef SSLSOCKET_DEBUG_RECEIVE // for receive debug
			printf( "SSL read fail\n" );
			printf( "Socket Error code: %d\n", WSAGetLastError() );
			printf( "Socket %d\n", m_Socket );

			CHAR   buffer[ 256 ] = {};
			u_long err			 = CSSLAdapter::ERR_get_error();
			while( err != 0 ) {
				CSSLAdapter::ERR_error_string_n( err, buffer, sizeof( buffer ) );
				printf( "*** %s\n", buffer );

				err = CSSLAdapter::ERR_get_error();
			}
#endif
			return -1;
		}
	}
	// record receive data
	PutDataToQueue( m_ReceiveBuffer, nTotalRead );

	// copy data to output buffer
	ReadDataFromQueue( buffer + ( nLength - nRemainDataCount ), nRemainDataCount );

#ifdef SSLSOCKET_DEBUG_RECEIVE // for receive debug
	printf( "Receive data %d, queue remain %d data\n", nLength, m_nQueueLen );
	printf( "Receive buffer: " );
	for( int i = 0; i < nLength ; i++ ) {
		printf( "%d, ", buffer[ i ] );
	}
	printf( "\nQueue: " );
	for( int i = 0; i < m_nQueueLen ; i++ ) {
		printf( "%d, ", m_QueueData[ i ] );
	}
	printf( "\n" );
#endif

	return nLength;
}

INT CSSLSocket::Send( BYTE buffer[], INT nLength )
// Send data on the network channel to the broker
{
	INT nSendCount = 0;

	if( m_bConnected == FALSE || m_bStopRequest == TRUE ) {
		return 0;
	}

	m_SendState = ESS_Sending;
	if( m_bSecure == TRUE ) {
		nSendCount = CSSLAdapter::SSL_write( m_pSSL, buffer, nLength );
	}
	else {
		nSendCount = ::send( m_Socket, ( CHAR * )( void * )buffer, nLength, 0 );
	}

	m_SendState = ESS_Idle;

	// notice disconnect function sending is end
	if( m_bStopRequest == TRUE ) {
		SetEvent( m_SendEndEvent );
	}

#ifdef SSLSOCKET_DEBUG_SEND // for send debug
	printf( "------\n" );
	printf( "Send %d data\n", nLength );
	for( int i = 0; i < nLength ; i++ ) {
		printf( "%d, ", buffer[ i ] );
	}
	printf( "\n" );
	printf( "Result Send %d data\n", nSendCount );

	if( nSendCount < 0 ) {
		printf( "SSL send fail\n" );
		printf( "Socket Error code: %d\n", WSAGetLastError() );
		printf( "Socket %d\n", m_Socket );

		CHAR   buffer[ 256 ] = {};
		u_long err			 = CSSLAdapter::ERR_get_error();
		while( err != 0 ) {
			CSSLAdapter::ERR_error_string_n( err, buffer, sizeof( buffer ) );
			printf( "*** %s\n", buffer );

			err = CSSLAdapter::ERR_get_error();
		}
	}
#endif
	return nSendCount;
}

void CSSLSocket::Close( void )
// Close the network channel
{
	DeinitSocket();
}

BOOL CSSLSocket::Connect( void )
// Connect to remote server
{
	if( m_bConnected == TRUE ) {
		return TRUE;
	}

	// create socket
	m_Socket = CreateSocket( m_szRemoteHostIP, m_nRemoteHostPort );

	// check socket result
	if( m_Socket == INVALID_SOCKET ) {
		return FALSE;
	}

	// no SSL request, it will not init SSL
	if( m_bSecure == TRUE ) {
		m_bConnected = InitSSL();
		return m_bConnected;
	}
	else {
		m_bConnected = TRUE;
	}

	return m_bConnected;
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
	m_pSSLContext = CreateSSLContext();
	if( m_pSSLContext == NULL ) {
		return FALSE;
	}

	// Start to build ssl connection
	m_pSSL = CSSLAdapter::SSL_new( m_pSSLContext );
	if( m_pSSL == NULL ) {
		return FALSE;
	}

	CSSLAdapter::SSL_set_fd( m_pSSL, m_Socket );

	INT nConnectResult = CSSLAdapter::SSL_connect( m_pSSL );
	if( nConnectResult <= 0 ) {

#ifdef SSLSOCKET_DEBUG_SSL // for SSL connect debug
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
#endif
		return FALSE;
	}

#ifdef SSLSOCKET_DEBUG_SSL // for SSL connect debug
	printf( "SSL connect success, using %s\n", CSSLAdapter::SSL_get_version( m_pSSL ) );
#endif


	return TRUE;
}

void CSSLSocket::DeinitSocket( void )
// free the socket resouse
{
	if( m_bConnected == FALSE ) {
		return;
	}

	// request stop all operation
	m_bStopRequest = TRUE;

	// deinit socket first, let receive and send function auto stop and return
	if( m_Socket != INVALID_SOCKET ) {
		// close socket
		closesocket( m_Socket );

		m_Socket = INVALID_SOCKET;
	}

	// wait receive is end process
	if( m_RecvState == ERS_Receiving ) {
		WaitForSingleObject( m_RecvEndEvent, INFINITE );
	}

	// wait send is end process
	if( m_SendState == ESS_Sending ) {
		WaitForSingleObject( m_SendEndEvent, INFINITE );
	}

	// clean SSL setting
	if( m_bSecure == TRUE ) {
		if( m_pSSL != NULL ) {
			// SSL close
			CSSLAdapter::SSL_shutdown( m_pSSL );

			// Free resource
			CSSLAdapter::SSL_free( m_pSSL );

			m_pSSL = NULL;
		}

		if( m_pSSLContext != NULL ) {
			// free context
			CSSLAdapter::SSL_CTX_free( m_pSSLContext );

			m_pSSLContext = NULL;
		}
	}

	// clear buffer
	memset( m_ReceiveBuffer, 0, sizeof( BYTE ) * RECV_SIZE );
	memset( m_QueueData, 0, sizeof( BYTE ) * RECV_SIZE );
	m_nQueueLen = 0;

	m_bStopRequest = FALSE;
	m_bConnected = FALSE;
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
	// use TLS method, it will auto support SSLv3, TLSv1, TLSv1.1 and TLSv1.2
	method = CSSLAdapter::TLS_client_method();
	// Create context
	ctx = CSSLAdapter::SSL_CTX_new( method );

	return ctx;
}

int CSSLSocket::ReadDataFromQueue( BYTE buffer[], INT nLength )
// read data from queue, return read data count
{
	if( m_nQueueLen <= 0 || nLength <= 0 || buffer == NULL ) {
		return 0;
	}

	// make sure not copy too many data
	int nCopyLen = min( nLength, m_nQueueLen );

	// copy dat from queue
	memcpy( buffer, m_QueueData, nCopyLen );

	// move queue data
	if( m_nQueueLen - nCopyLen > 0 ) {
		memmove( m_QueueData, m_QueueData + nCopyLen, ( m_nQueueLen - nCopyLen ) );
	}

	// update length
	m_nQueueLen -= nCopyLen;

	return nCopyLen;
}

void CSSLSocket::PutDataToQueue( BYTE buffer[], INT nLength )
// put data to queue
{
	// check length
	if( nLength <= 0 || buffer == NULL || m_nQueueLen == RECV_SIZE ) {
		return;
	}

	// make sure not copy too many data
	int nCopyLen = min( nLength, ( RECV_SIZE - m_nQueueLen ) );

	// copy dat from queue
	memcpy( m_QueueData + m_nQueueLen, buffer, nCopyLen );

	// update length
	m_nQueueLen += nCopyLen;
}
