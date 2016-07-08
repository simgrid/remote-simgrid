package org.simgrid.rsg;

import org.apache.thrift.server.TServer;

import RsgService.*;
import org.simgrid.rsg.Engine;
import java.util.HashMap;
import java.util.Map;

public class Actor {
	
   public static void quit() {
		 try{
      RsgActor.Client client = new RsgActor.Client(Engine.getInstance().getMultiplexedProtocol("RsgActor"));
			client.close();	
		} catch(Exception e) {
			e.printStackTrace();
		}
   }
   
}
