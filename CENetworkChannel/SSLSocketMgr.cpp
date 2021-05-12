#include "StdAfx.h"
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "OpenSSLAdapter.h"
#include "SSLSocket.h"
#include "SSLSocketMgr.h"

// init static member data
BOOL CSSLSocketMgr::m_bInitWinSocket = FALSE;

// =============== //
// public function //
// =============== //

CSSLSocketMgr::CSSLSocketMgr( void ) : m_SSLSocketList( 0 )
// constructor
{
	if( m_bInitWinSocket == FALSE ) {
		WSADATA wsaData;
		::WSAStartup( WINSOCK_VERSION, &wsaData );
		m_bInitWinSocket = TRUE;
	}
}

CSSLSocketMgr::~CSSLSocketMgr( void )
// distructor
{
	// find non empty slot and close the socket
	for( UINT i = 0; i < m_SSLSocketList.size(); i++ ) {
		if( m_SSLSocketList[ i ] != NULL ) {
			Close( i );
		}
	}
}

INT CSSLSocketMgr::InitNetworkChannel( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, INT nProtocols )
/// <summary>
/// Init the network channel
/// </summary>
/// <param name="szRemoteHostIP">remote host name</param>
/// <param name="nRemoteHostPort">remote port name</param>
/// <param name="bSecure">use SSL/TLS or not</param>
/// <param name="nProtocols">SSL/TLX communication protocols</param>
/// <returns>Channel handle, handle >= 0</returns>
{
	CSSLSocket *pSSLSocket = new CSSLSocket( szRemoteHostIP, nRemoteHostPort, bSecure, ( SSLProtocols )nProtocols );

	return RegistrySocket( pSSLSocket );;
}

BOOL CSSLSocketMgr::DataAvailable( INT nChHandle )
/// <summary>
/// Data available on channel
/// </summary>
/// <param name="nChHandle">Socket handle</param>
{
	// check handle
	if( nChHandle < 0 || ( UINT )nChHandle >= m_SSLSocketList.size() || m_SSLSocketList[ nChHandle ] == NULL ) {
		return FALSE;
	}

	return m_SSLSocketList[ nChHandle ]->DataAvailable();
}

INT CSSLSocketMgr::Receive( INT nChHandle, BYTE buffer[], INT Length, INT timeout )
/// <summary>
/// Receive data from the network channel with a specified timeout
/// </summary>
/// <param name="nChHandle">Socket handle</param>
/// <param name="buffer">Data buffer for receiving data</param>
/// <param name="Length">Data buffer length</param>
/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
/// <returns>Number of bytes received</returns>
{
	// check handle
	if( nChHandle < 0 || ( UINT )nChHandle >= m_SSLSocketList.size() || m_SSLSocketList[ nChHandle ] == NULL ) {
		return 0;
	}
	
	return m_SSLSocketList[ nChHandle ]->Receive( buffer, Length, timeout );
}

INT CSSLSocketMgr::Send( INT nChHandle, BYTE buffer[], INT Length )
/// <summary>
/// Send data on the network channel to the broker
/// </summary>
/// <param name="nChHandle">Socket handle</param>
/// <param name="buffer">Data buffer to send</param>
/// <returns>Number of byte sent</returns>
{
	// check handle
	if( nChHandle < 0 || ( UINT )nChHandle >= m_SSLSocketList.size() || m_SSLSocketList[ nChHandle ] == NULL ) {
		return 0;
	}

	return m_SSLSocketList[ nChHandle ]->Send( buffer, Length );
}

void CSSLSocketMgr::Close( INT nChHandle )
/// <summary>
/// Close the network channel
/// </summary>
/// <param name="nChHandle">Socket handle</param>
{
	// check handle
	if( nChHandle < 0 || ( UINT )nChHandle >= m_SSLSocketList.size() || m_SSLSocketList[ nChHandle ] == NULL ) {
		return;
	}

	// close socket
	m_SSLSocketList[ nChHandle ]->Close();

	// free the socket
	delete m_SSLSocketList[ nChHandle ];
	m_SSLSocketList[ nChHandle ] = NULL;
}

void CSSLSocketMgr::Connect( INT nChHandle )
/// <summary>
/// Connect to remote server
/// </summary>
/// <param name="nChHandle">Socket handle</param>
{
	// check handle
	if( nChHandle < 0 || ( UINT )nChHandle >= m_SSLSocketList.size() || m_SSLSocketList[ nChHandle ] == NULL ) {
		return;
	}

	m_SSLSocketList[ nChHandle ]->Connect();
}

// ================ //
// private function //
// ================ //

INT CSSLSocketMgr::RegistrySocket( CSSLSocket *pSSLSocket )
/// <summary>
/// registry socket
/// </summary>
/// <param name="pSSLSocket">SSL socket want to registry</param>
/// <returns>socket handle</returns>
{
	// find empty slot first
	for( UINT i = 0; i < m_SSLSocketList.size(); i++ ) {
		if( m_SSLSocketList[ i ] == NULL ) {
			m_SSLSocketList[ i ] = pSSLSocket;
			return i;
		}
	}

	// no empty slot, add new item to vector
	m_SSLSocketList.push_back( pSSLSocket );

	// return last index
	return m_SSLSocketList.size() - 1;
}