
import java.awt.AWTException;
import java.awt.MouseInfo;
import java.awt.Point;
import java.awt.Robot;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.List;

import javax.media.CannotRealizeException;
import javax.media.Manager;
import javax.media.NoPlayerException;
import javax.media.Player;
import javax.swing.JOptionPane;



public class Phonedoor_daemon{
  Thread thread;
  DatagramSocket socket;
  List<String> lignes = null; // init of List<String> for file parameter reading
  int OS = 0;
  byte[] buffer = new byte[1024];
  int port = 5650;
  public String audiofilepath = null;

  public String Answer = new String("OKFROMTAB");
  public static void stop()
  {
    // implementation goes here
  } 

  public static void main(String[] args) {
	  Phonedoor_daemon u = new Phonedoor_daemon();
	  
  }
  
  
  public Phonedoor_daemon(){
	  new StartThread();
  }
  
  public class StartThread implements Runnable{
	 
  StartThread(){
  thread = new Thread(this);  
  thread.start();
  }
  public void run(){

  try{
	  try{
		  socket = new DatagramSocket(port);
// read all parameter once from file		  
		// Determining OS
		  OS = PlatformDetector.detect();
//		  try {
			  //for test : read parameters from text file 
			  if (System.getProperty("os.name").startsWith("Windows")){
                  lignes = Files.readAllLines(FileSystems.getDefault().getPath("C:\\Phone_door\\config.txt"), StandardCharsets.ISO_8859_1);  //iso 8859 for windows , utf-8 for linux
			      audiofilepath = lignes.get(1); 
			      System.out.println(audiofilepath);
					}
			  else {
				  lignes = Files.readAllLines(FileSystems.getDefault().getPath("/home/mael/config.txt"), StandardCharsets.UTF_8);  //iso 8859 for windows , utf-8 for linux
				  audiofilepath = lignes.get(3); 
			      System.out.println(audiofilepath);
			  }
 	  }

		  catch (Exception e) {
					JOptionPane.showMessageDialog(null,"error, perhaps  could not find file settings" + 
			             ":\n" + e.getLocalizedMessage());
				System.err.println(e);
			}
	  //PhoneDoor_GUI a = null;
		  //waiting for call request	  
		  while(true){
			  try{
  //Receive request from client
				  DatagramPacket packet =  
						  new DatagramPacket(buffer, buffer.length );
				  socket.receive(packet);
				  InetAddress client = packet.getAddress();
				  int client_port = packet.getPort();
				  String donnees_recues = new String(buffer);
				  // debug
				  //			  JOptionPane.showMessageDialog(null,"donnees_recues  " + 
		//		          ":\n" + donnees_recues);
				  // GUI launch alone for settings
				  if (donnees_recues.contains("GUI")) {
					  PhoneDoor_GUI a = new PhoneDoor_GUI(lignes);	
				  }
				  // Launch app
				  if (donnees_recues.contains("end_request_calls")) {
				 // for debug purpose only, inserted here to remove keep alive frames
	/*			  JOptionPane.showMessageDialog(null,"received packet with following data" + 
				              donnees_recues); 
		*/		  // Sending acknowledge
					   
					  byte answer[] = Answer.getBytes();
					  packet = 
							  new DatagramPacket(answer, answer.length,client,client_port);
					  socket.send(packet);
					  //  Launching GUI 
					  PhoneDoor_GUI a = new PhoneDoor_GUI(lignes);						    
					    // launch sound player					  	  
					  						  // open the sound file as a Java input stream
					  		InputStream in = new FileInputStream(audiofilepath); 
						    File f=new File(audiofilepath);
						    // create an audiostream from the inputstream
						    final Player p=Manager.createRealizedPlayer(f.toURI().toURL());
						    // Start the music
						    p.start();

					  // Launching viewer
						    if (System.getProperty("os.name").startsWith("Windows")){
				            Runtime runtime = Runtime.getRuntime();
						     runtime.exec(new String[] { "C:\\Users\\Loule\\AppData\\Local\\VirtualStore\\Program Files\\Xeoma\\xeoma.exe"} ); 
						    //entering sleep mode command, no need for windows device ; if the system runs it's because someone is on						    
				//		    Runtime.getRuntime().exec("rundll32.exe powrprof.dll,SetSuspendState 0,1,0"); 
						   	  }
							  else { Runtime runtime = Runtime.getRuntime();
							  	runtime.exec(new String[] { "vlc -vvv http://192.168.0.10/cgi/mjpg/mjpeg.cgi?.mjpeg &"});

							   // runtime.exec(new String[] { "motion -c motion.conf &"} ); // could use zoneminder alternatively ?
					//		    runtime.exec(new String[] { "xfcse-screensaver &" }); // see http://ubuntuforums.org/showthread.php?t=1810262 for screensaver enable/disable
							
							   }
						    // getting out  from screensaver by a small hack : moving the mouse
								try {
									Point point = MouseInfo.getPointerInfo().getLocation();
									Robot robot;
									robot = new Robot();
									robot.mouseMove(point.x + 1, point.y);  
									robot.mouseMove(point.x, point.y); 
								} catch (AWTException sc) {
									JOptionPane.showMessageDialog(null,"error awt exception (sreen saver exit)" + 
								             ":\n" + sc.getLocalizedMessage());
									sc.printStackTrace();
								}  				 
				  }
			  }
			  
			  catch (NoPlayerException e) {
				  JOptionPane.showMessageDialog(null,"error noplayer" + 
				             ":\n" + e.getLocalizedMessage());
				e.printStackTrace();
			} 
			  catch (CannotRealizeException e) {
				JOptionPane.showMessageDialog(null,"error cannot realize" + 
			             ":\n" + e.getLocalizedMessage());
				e.printStackTrace();
			} 
			  catch (IOException e) {
								  JOptionPane.showMessageDialog(null,"error aaaa" + 
								             ":\n" + e.getLocalizedMessage());
							  System.err.println(e);
			 } finally {}
				  }
		  
}finally {}
 
  }
  }
}
			 
