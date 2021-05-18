#if !defined( _CENETWORKCHANNEL_H____INCLUDED_ )
#define _CENETWORKCHANNEL_H____INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif	  // _MSC_VER > 1000

#define CENETAPI __stdcall

extern "C" {

/// <summary>
/// Init the network channel
/// </summary>
/// <param name="szRemoteHostIP">remote host IP</param>
/// <param name="nRemoteHostPort">remote port name</param>
/// <param name="bSecure">use SSL/TLS or not</param>
/// <param name="nProtocols">SSL/TLS communication protocols</param>
/// <returns>Channel handle, handle >= 0</returns>
INT CENETAPI InitNetworkChannel( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, INT nProtocols );

/// <summary>
/// Deinit the channel and release the resource
/// </summary>
/// <param name="nChHandle">channel handle</param>
void CENETAPI DeinitNetworkChannel( INT nChHandle );

/// <summary>
/// Receive data from the network channel with a specified timeout
/// </summary>
/// <param name="nChHandle">Channel handle</param>
/// <param name="buffer">Data buffer for receiving data</param>
/// <param name="Length">Data buffer length</param>
/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
/// <returns>Number of bytes received</returns>
INT CENETAPI Receive( INT nChHandle, BYTE buffer[], INT Length, INT timeout = 0 );

/// <summary>
/// Send data on the network channel to the broker
/// </summary>
/// <param name="nChHandle">Channel handle</param>
/// <param name="buffer">Data buffer to send</param>
/// <param name="Length">Data buffer length</param>
/// <returns>Number of byte sent</returns>
INT CENETAPI Send( INT nChHandle, BYTE buffer[], INT Length );

/// <summary>
/// Close the network socket
/// </summary>
/// <param name="nChHandle">Channel handle</param>
void CENETAPI Close( INT nChHandle );

/// <summary>
/// Connect to remote server
/// </summary>
/// <param name="nChHandle">Channel handle</param>
/// <returns>connect success will return TRUE</returns>
BOOL CENETAPI Connect( INT nChHandle );

/// <summary>
/// Set current thread processor number
/// </summary>
/// <param name="nProcessorNumber">processer number, if bigger then actual process count, then set 1</param>
void CENETAPI SetThreadProcessor( UINT nProcessorNumber );

}	 // extern "C"

#endif	  // !defined(_CENETWORKCHANNEL_H____INCLUDED_)
