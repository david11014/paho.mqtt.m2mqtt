using System;
using System.Text;
using System.Net;
using System.Runtime.InteropServices;

namespace uPLibrary.Networking.M2Mqtt
{
	partial class CENetworkChannel : IMqttNetworkChannel
	{
		#region Import DLL

		/// <summary>
		/// Init the network channel
		/// </summary>
		/// <param name="szRemoteHostIP">remote host IP</param>
		/// <param name="nRemoteHostPort">remote port name</param>
		/// <param name="bSecure">use SSL/TLS or not</param>
		/// <param name="nProtocols">SSL/TLS communication protocols</param>
		/// <returns>Channel handle, handle >= 0</returns>
		[DllImport( "CENetworkChannel.dll" )]
		public static extern int InitNetworkChannel( byte[] szRemoteHostIP, int nRemoteHostPort, bool bSecure, int nProtocols );

		/// <summary>
		/// Deinit the channel and release the resource
		/// </summary>
		/// <param name="nChHandle">channel handle</param>
		public static extern void DeinitNetworkChannel( int nChHandle );

		/// <summary>
		/// Receive data from the network channel with a specified timeout
		/// </summary>
		/// <param name="nChHandle">Channel handle</param>
		/// <param name="buffer">Data buffer for receiving data</param>
		/// <param name="Length">Data buffer length</param>
		/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
		/// <returns>Number of bytes received</returns>
		[DllImport( "CENetworkChannel.dll" )]
		public static extern int Receive( int nChHandle, byte[] buffer, int Length, int timeout );

		/// <summary>
		/// Send data on the network channel to the broker
		/// </summary>
		/// <param name="nChHandle">Channel handle</param>
		/// <param name="buffer">Data buffer to send</param>
		/// <param name="Length">Data buffer length</param>
		/// <returns>Number of byte sent</returns>
		[DllImport( "CENetworkChannel.dll" )]
		public static extern int Send( int nChHandle, byte[] buffer, int Length );

		/// <summary>
		/// Close the network channel
		/// </summary>
		/// <param name="nChHandle">Channel handle</param>
		[DllImport( "CENetworkChannel.dll" )]
		public static extern void Close( int nChHandle );

		/// <summary>
		/// Connect to remote server
		/// </summary>
		/// <param name="nChHandle">Channel handle</param>
		[DllImport( "CENetworkChannel.dll" )]
		public static extern bool Connect( int nChHandle );

		#endregion // Import DLL

		#region private member data

		// remote host information
		private string m_remoteHostName;
		private int m_remotePort;
		private IPAddress m_remoteIpAddress;

		// socket handle
		private int m_socketHandle;

		// using SSL
		private bool m_secure;

		// SSL/TLS protocol version
		private MqttSslProtocols m_sslProtocol;

		#endregion // private member data

		#region public method

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="remoteHostName">Remote Host name</param>
		/// <param name="remotePort">Remote port</param>
		/// <param name="secure">Using SSL</param>
		/// <param name="caCert">CA certificate</param>
		/// <param name="clientCert">Client certificate</param>
		/// <param name="sslProtocol">SSL/TLS protocol version</param>
		public CENetworkChannel( string remoteHostName, int remotePort, bool secure, MqttSslProtocols sslProtocol )
		{
			this.m_socketHandle = -1; // invald socket handle
			this.m_remoteHostName = remoteHostName;
			this.m_remotePort = remotePort;
			this.m_sslProtocol = sslProtocol;
			this.m_secure = secure;
			this.m_remoteIpAddress = null;

			// try use IP address format to parse ip address
			try {
				this.m_remoteIpAddress = IPAddress.Parse( remoteHostName );
			}
			catch {
				this.m_remoteIpAddress = null;
			}

			// hsot name is not ip format, ask DNS to get ip address
			if( m_remoteIpAddress == null ) {
				IPHostEntry hostEntry = Dns.GetHostEntry( remoteHostName );
				if( ( hostEntry != null ) && ( hostEntry.AddressList.Length > 0 ) ) {
					// check for the first address not null
					// it seems that with .Net Micro Framework, the IPV6 addresses aren't supported and return "null"
					int i = 0;
					while( hostEntry.AddressList[ i ] == null ) {
						i++;
					}
					this.m_remoteIpAddress = hostEntry.AddressList[ i ];
				}
				else {
					throw new Exception( "No address found for the remote host name" );
				}
			}


			byte[] IPData = Encoding.ASCII.GetBytes( m_remoteIpAddress.ToString() );
			this.m_socketHandle = InitNetworkChannel( IPData, this.m_remotePort, this.m_secure, (int)this.m_sslProtocol );

			if( m_socketHandle < 0 ) {
				throw new Exception( "Init network channel fail" );
			}
		}

		/// <summary>
		/// Distructor
		/// </summary>
		~CENetworkChannel()
		{
			DeinitNetworkChannel( this.m_socketHandle );

			this.m_socketHandle = INVALID_SOCKET;
		}

		
		/// <summary>
		/// Data available on channel
		/// </summary>
		public bool DataAvailable
		{
			get
			{
				return true;
			}
		}

		/// <summary>
		/// Receive data from the network channel
		/// </summary>
		/// <param name="buffer">Data buffer for receiving data</param>
		/// <returns>Number of bytes received</returns>
		public int Receive( byte[] buffer )
		{
			return Receive( m_socketHandle, buffer, buffer.Length, 0 );
		}

		/// <summary>
		/// Receive data from the network channel with a specified timeout
		/// </summary>
		/// <param name="buffer">Data buffer for receiving data</param>
		/// <param name="timeout">Timeout on receiving (in milliseconds)</param>
		/// <returns>Number of bytes received</returns>
		public int Receive( byte[] buffer, int timeout )
		{
			return Receive( m_socketHandle, buffer, buffer.Length, timeout );
		}

		/// <summary>
		/// Send data on the network channel to the broker
		/// </summary>
		/// <param name="buffer">Data buffer to send</param>
		/// <returns>Number of byte sent</returns>
		public int Send( byte[] buffer )
		{
			return Send( m_socketHandle, buffer, buffer.Length );
		}

		/// <summary>
		/// Close the network channel
		/// </summary>
		public void Close()
		{
			Close( m_socketHandle );
		}

		/// <summary>
		/// Connect to remote server
		/// </summary>
		public void Connect()
		{
			if( Connect( m_socketHandle ) == false ) {
				throw new Exception( "Connect Fail" );
			}
		}

		/// <summary>
		/// Accept client connection
		/// </summary>
		public void Accept()
		{
			// TODO : not support server setting
			return;
		}

		#endregion // public method
	}
}
