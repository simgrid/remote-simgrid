package org.simgrid.rsg;

import java.util.HashMap;
import java.util.Map;

public class Mailbox {
	
	static private Map<String,Mailbox>mailboxes = new HashMap<String,Mailbox>();
	protected long remoteAddr = 0;
	private Mailbox(String name, long remoteAddr) {
		this.remoteAddr = remoteAddr;
		mailboxes.put(name, this);
	}
	public static Mailbox byName(String name) {
		Mailbox ret = mailboxes.get(name);
		if (ret == null) {
			Rsg.Request req = Rsg.Request.newBuilder()
					.setType(Rsg.Type.CMD_MB_CREATE)
					.setMbCreate(Rsg.Request.MBCreateRequest.newBuilder().setName(name))
					.build();
			Rsg.Answer ans = Engine.getInstance().sendRequest(req);
			ret = new Mailbox(name, ans.getMbCreate().getRemoteAddr());
		}
		return ret;
	}
	
	protected long getRemoteAddr() {
		return remoteAddr;
	}
}
