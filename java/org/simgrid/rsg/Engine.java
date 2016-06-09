package org.simgrid.rsg;

import RsgService.*;
import java.net.*;
import java.io.*;
import java.util.HashMap;
import java.util.Map;
import java.nio.charset.StandardCharsets;
import org.apache.thrift.TException;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TFramedTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.protocol.TMultiplexedProtocol;


public class Engine {
	final static int port = 9090;
	private static Engine pInstance;
	
	public TMultiplexedProtocol  getMultiplexedProtocol(String serviceName)  {
			return new TMultiplexedProtocol(pProtocol, serviceName);
	}
	
	private TProtocol getProtocol() {
		return pProtocol;
	}
	
	private TTransport getTransport() {
		return pTransport;
	}
	
	private TProtocol pProtocol;
	private TTransport pTransport;
	
	private Engine() {
		Socket socket;
		
		try {
			InetAddress serveur = InetAddress.getByName("localhost");
			socket = new Socket(serveur, port);
			
			DataInputStream inFromServer = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
			
			int iSize = inFromServer.readInt();
			iSize = Integer.reverseBytes(iSize); //read as little-endian
			
			System.out.println("Connected to Rpc server at port : " + iSize);
			Thread.sleep(1);
			pTransport = new TSocket("localhost", iSize);  
			pTransport.open();  
			
			pProtocol = new TBinaryProtocol(pTransport);

			// pTransport.close();
		}	catch (Exception  x) {
			x.printStackTrace();
		}
	}
	
	static public Engine getInstance() {
		if(pInstance == null) {
			pInstance = new Engine();
		} 
		return pInstance;
	}
	
	public static byte[] reverse(byte[] array) {
	      if (array == null) {
	          return null;
	      }
	      int i = 0;
	      int j = array.length - 1;
	      byte tmp;
	      while (j > i) {
	          tmp = array[j];
	          array[j] = array[i];
	          array[i] = tmp;
	          j--;
	          i++;
	      }
				return array;
	  }
	
}
