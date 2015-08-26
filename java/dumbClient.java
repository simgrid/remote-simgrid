import org.simgrid.rsg.Actor;
import org.simgrid.rsg.Mailbox;

public class dumbClient {
	public static void main(String[] args) {

		// Start a bunch of threads (stress test)
		for (int i = 0; i<100; i++) {
			Thread t = new Thread() {
				@Override
				public void start() {
					Actor self = Actor.self();

					self.sleep(42);
					self.execute(8095000000.);
				}
			};
			t.run();
		}

		// A regular client doing some communications
		Actor self = Actor.self();
		Mailbox mbox = Mailbox.byName("toto");
		
		self.sleep(42);
		self.execute(8095000000.); // That's the power of my host on the used platform

		self.send(mbox,"message from client");
		String msg = self.recv(mbox);
		System.err.println("jClient: Received message: "+msg);
		
		self.quit();
	}
}
