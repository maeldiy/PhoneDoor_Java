this project implement a video door gate using java app and an Ethernet video camera.
See video door gate architecture.jpg and http://maeldiy.wordpress.com/ for more details
See license file for license

The repositery contains Arduino_Phone_door directory for Arduino part. 

By receiving an Ethernet message from the arduino device, the android application do the following :

1 Acknowledge the message by answer to the Ethernet device
<<<<<<< HEAD
2 Play a sound
3 Launch a ipcam viewing app depending the OS (windows or Linux)
4 wake up from sreen saver (not from sleep mode)
5 LAunch the GUI which is able to send a command to the arduino hardware, it is also able to set the sound
 What the application need ? :

_ as it is a JAVA app : the Java runtime 7 :-)
_ the file config.txt to be available
_ VLC  to be set in linux, xeoma for windows
=======
4 Play a sound
5 Launch a ipcam viewing app depending the OS (windows or Linux)
6 LAunch the GUI which is able to send a command to the arduino hardware, it is also able to set the sound
7 What the application need ? :

_ as it is a JAVA app : the Java runtime 7
_ the file config.txt to be available
_ zoneminder to be set in linux, xeoma for windows
>>>>>>> 17abafc7f96e90a7e109594a5d34ee6946d0805e
_ JMF mpeg plugin install : http://www.oracle.com/technetwork/java/javase/download-137625.html 

Beware ! :  The application is intended to run on a device which is permanently connected to a power supply. As the Wifi never stop, it will drain the battery.

What the application should do more (future deev ??) :

3 Allow the possibility to define the volume of the song
4 be able to switch to screensaver and to Define the time out value to return to main screen
<<<<<<< HEAD
5 zoneminder support in linux to enable multiple view
=======
5 if necessary wake up from secrren saver (not from sleep mode)
>>>>>>> 17abafc7f96e90a7e109594a5d34ee6946d0805e
