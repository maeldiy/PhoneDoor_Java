import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRootPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

public class PhoneDoor_GUI{
  boolean debug = true;	
  JFrame frame;
  JPanel panel;
  JButton button1,button2,button3,button4,button5;
  JTextArea area;
  JScrollPane pane;
  Thread thread;
  DatagramSocket socketout;
  
  List<String> lignes = null; // init of List<String> for file parameter reading
  int OS = 0;
  byte[] buffer = new byte[1024];
  int port = 5650;
  String audiofilepath = null;

  public static void GUI() {
//	  PhoneDoor_GUI u = new PhoneDoor_GUI();
	  
  }
 
  public  PhoneDoor_GUI(List<String> config_data){

  lignes = config_data;
  final String SERVERIP = "192.168.0.177"; // IP adress of arduino ethernet
  final int SERVERPORT = 8888;

  frame = new JFrame("COMMANDES");
  frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
  frame.setUndecorated(true);
  frame.getRootPane()
  .setWindowDecorationStyle(JRootPane.PLAIN_DIALOG);
  panel = new JPanel();
  panel.setLayout(null);
  area = new JTextArea();
  area.setEditable(false);
  //taking settings from config file
  int width = Integer.parseInt(lignes.get(5));
  int height = Integer.parseInt(lignes.get(6));
  
  pane = new JScrollPane(area);
  pane.setBounds(10, 230, 320, 200);
  panel.add(pane);
  frame.add(panel);
  frame.setSize(width, height);
  frame.setVisible(true);
  frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);   // DISPOSE_ON_CLOSE);
  
  // closing GUI does not close camera app, the following code should do it; but doesn't. To be corrected
  
  frame.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(WindowEvent winEvt) {
	     // this override method close the application opened by the app except the player, but it is supposed to play a few seconds only
	      
	       if (System.getProperty("os.name").startsWith("Windows")){
						     try {						    
								Runtime.getRuntime().exec("taskkill /F /IM xeoma.exe");
								frame.dispose();
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} 
						   	  }
							  else { 
							  try {
								Runtime.getRuntime().exec("kill -9 " + "vlc");
								frame.dispose();
							} catch (IOException e) {
								JOptionPane.showMessageDialog(null,"error trying to cloase all windows" + 
							             ":\n" + e.getLocalizedMessage());
								e.printStackTrace();
							} 
							   // runtime.exec(new String[] { "motion -c motion.conf &"} ); // could use zoneminder alternatively ?
					//		    runtime.exec(new String[] { "xfcse-screensaver &" }); // see http://ubuntuforums.org/showthread.php?t=1810262 for screensaver enable/disable
							
	    }
	}
  });
   
  
  // button1 settings and usage
  button1 = new JButton("OUVRIR PORTAIL");
  button1.setBounds(10, 10, 320, 40);
  button1.addActionListener(new ActionListener(){
  public void actionPerformed(ActionEvent ae){
  try {
	InetAddress serverAddr = InetAddress.getByName(SERVERIP);
	socketout = new DatagramSocket();
	socketout.connect(serverAddr,SERVERPORT);
	SendDataToNetwork( "button ouvrir portail pressed",socketout,serverAddr, SERVERPORT);
	area.append("\n COMMANDE OUVRIR PORTAIL ENVOYEE");
	} catch (UnknownHostException e1) {
		JOptionPane.showMessageDialog(null,
	             ":\n" + e1.getLocalizedMessage());
	e1.printStackTrace();
	} catch (SocketException e) {
		 JOptionPane.showMessageDialog(null,"socket problem" + 
	             ":\n" + e.getLocalizedMessage());
	e.printStackTrace();
	} catch(Exception error){
    System.out.print("could't to connect to server because : " + error.getMessage());
  }	
  }
  });
  panel.add(button1);
 
// button2 settings and usage
  button2 = new JButton("OUVRIR UN BATTANT");
  button2.setBounds(10, 90, 320, 40);
  button2.addActionListener(new ActionListener(){
  public void actionPerformed (ActionEvent ae){

 
  try {
	InetAddress serverAddr = InetAddress.getByName(SERVERIP);
	socketout = new DatagramSocket();
	socketout.connect(serverAddr,SERVERPORT);
	SendDataToNetwork( "button ouvrir BATTANT pressed",socketout,serverAddr, SERVERPORT);
	 area.append("\n COMMANDE OUVRIR BATTANT ENVOYEE");
	} catch (UnknownHostException e1) {
	// TODO Auto-generated catch block
		JOptionPane.showMessageDialog(null,
	             ":\n" + e1.getLocalizedMessage());
	e1.printStackTrace();
	} catch (SocketException e) {
		 JOptionPane.showMessageDialog(null,"socket problem" + 
	             ":\n" + e.getLocalizedMessage());
	} catch(Exception error){
    System.out.print("could't to connect to server because : " + error.getMessage());
  }	
  }
  });
  button2.setEnabled(true);
  panel.add(button2);
//button3 settings and usage  
  button3 = new JButton("PARLER");
  button3.setBounds(10, 170, 320, 40);
  button3.addActionListener(new ActionListener(){
  public void actionPerformed (ActionEvent ae){
	  try {
	//		No real need, the aim is to activate the mic and send it to the micro...
		  	InetAddress serverAddr = InetAddress.getByName(SERVERIP);
			socketout = new DatagramSocket();
			socketout.connect(serverAddr,SERVERPORT);
			SendDataToNetwork( "button ouvrir PARLER pressed",socketout,serverAddr, SERVERPORT);
			area.append("\n PARLER");
			} catch (UnknownHostException e1) {
				 JOptionPane.showMessageDialog(null,
			             ":\n" + e1.getLocalizedMessage());
			e1.printStackTrace();
			} catch (SocketException e) {
				 JOptionPane.showMessageDialog(null,"socket problem" + 
			             ":\n" + e.getLocalizedMessage());
			e.printStackTrace();
			} catch(Exception error){
		    System.out.print("could't to connect to server because : " + error.getMessage());
		  }	
		  }
  });
  button3.setEnabled(true);
  panel.add(button3);
//button4 settings and usage  
  button4 = new JButton("SPARE");
  button4.setBounds(10, 450, 320, 40);
  button4.addActionListener(new ActionListener(){
  public void actionPerformed (ActionEvent ae){
	  try {
			InetAddress serverAddr = InetAddress.getByName(SERVERIP);
			socketout = new DatagramSocket();
			socketout.connect(serverAddr,SERVERPORT);
			SendDataToNetwork( "button SPARE pressed",socketout,serverAddr, SERVERPORT);
			  area.append("\n SPARE"); 
			} catch (UnknownHostException e1) {
				 JOptionPane.showMessageDialog(null,
			             ":\n" + e1.getLocalizedMessage());
			e1.printStackTrace();
			} catch (SocketException e) {
			// TODO Auto-generated catch block
				 JOptionPane.showMessageDialog(null,"socket problem" + 
			             ":\n" + e.getLocalizedMessage());
			e.printStackTrace();
			} catch(Exception error){
		    System.out.print("could't to connect to server because : " + error.getMessage());
		  }	
		  }
  });
  button4.setEnabled(true);
  panel.add(button4);
//button5 settings and usage  
  button5 = new JButton("audio file");
  button5.setBounds(10, 530, 320, 40);
  button5.addActionListener(new ActionListener(){
  public void actionPerformed (ActionEvent ae){
	  JFileChooser chooser = new JFileChooser();
	    File f;
		try {
			f = new File(new File("audio.mp3").getCanonicalPath()); 
			chooser.setSelectedFile(f);
			chooser.showOpenDialog(null);
			File Newaudiofilepath = chooser.getSelectedFile();
			JOptionPane.showMessageDialog(null,"fichier ouvert" + 
		             ":\n" + Newaudiofilepath);
				  try{
					// Determining OS
					  OS = PlatformDetector.detect();
						  if (OS == PlatformDetector.WINDOWS){
			                  	lignes.set(1, String.valueOf(Newaudiofilepath));
			                  	 Files.write(FileSystems.getDefault().getPath(lignes.get(8)), lignes, StandardCharsets.ISO_8859_1);
								}
						  else {
							    lignes.set(3, String.valueOf(Newaudiofilepath));
							    Files.write(FileSystems.getDefault().getPath(lignes.get(10)), lignes, StandardCharsets.UTF_8);
						  }
			 	  }
				  catch (Exception e) {
								JOptionPane.showMessageDialog(null,"error, couldnt write to file" + 
						             ":\n" + e.getLocalizedMessage());
							System.err.println(e);
						}
    
		  } catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
  }
  });
  button5.setEnabled(true);
  panel.add(button5);
  

  
  
  }
  
	 public void SendDataToNetwork(String cmd,DatagramSocket chaussette,InetAddress Addrserver,int SERVERPORT) { //You run this from the main thread. chaussette = socket in french
	       try {
		 			/* Create new UDP-Socket */ /* Create UDP-packet with data & destination(url+port) */
		 			byte[] c = cmd.getBytes();
		 			DatagramPacket packetcmd = new DatagramPacket(c, c.length, Addrserver,SERVERPORT);
		 			chaussette.send(packetcmd);
		       } catch (Exception e) {
		    	   area.append("COMMANDE OUVRIR UN BATTANT ENVOYEE" + e.getMessage());
		       }
		   }
  }
