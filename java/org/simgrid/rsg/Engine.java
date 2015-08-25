package org.simgrid.rsg;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import org.simgrid.rsg.Rsg.Answer;
import org.simgrid.rsg.Rsg.Request;

import com.google.protobuf.CodedInputStream;
import com.google.protobuf.CodedOutputStream;
import com.google.protobuf.TextFormat;

public class Engine {
	private Socket mysock;
	private OutputStream out;
	private InputStream in;

	private Engine() {
		Integer port = 0;
		try {
			port = Integer.parseInt(System.getenv().get("RSG_PORT"));
		} catch (java.lang.NumberFormatException nfe) {
			System.err.println("RSG_PORT is not set. Did you launch that binary from the RSG server?");
			System.exit(1);
		}
		try {
			mysock = new Socket("127.0.0.1", port);
			out = mysock.getOutputStream();
			in = mysock.getInputStream();
		} catch (IOException e) {
			throw new RuntimeException("Cannot connect back to the server on port "+port, e);
		}
	}

	public void finalize() {
		try {
			mysock.close();
		} catch (IOException e) {
			System.err.println("Error while closing my socket to the server:");
			e.printStackTrace();
		}
	}
	static private Engine instance;
	static public Engine getInstance() {
		if (instance == null) {
			instance = new Engine();
		}
		return instance;
	}

	public Answer sendRequest(Request req) {
		Answer ret;
		try {
		    int serialized = req.getSerializedSize();
		    final CodedOutputStream codedOutput =
		        CodedOutputStream.newInstance(out, serialized+4);
		    codedOutput.writeFixed32NoTag(serialized);
		    req.writeTo(codedOutput);
		    codedOutput.flush();
		    
		    byte[] data = new byte[4];
		    in.read(data);
		    CodedInputStream codedInput = CodedInputStream.newInstance(data);
		    serialized = codedInput.readRawLittleEndian32();
		    data = new byte[serialized];
		    in.read(data);
		    codedInput = CodedInputStream.newInstance(data);

			ret = Answer.parseFrom(data);
			//System.err.println("Java: Read "+ret.getClass().getName()+": "+TextFormat.shortDebugString(ret));
		} catch (IOException e) {
			throw new RuntimeException("Error while reading the answer",e);
		}
		clock = ret.getClock();
		return ret;
	}
	
	private double clock = 0;
	public double getClock() {
		return clock;
	}
}