# Mini-Torrent-File-Sharing-System

Functionalities provided

1. Creating .mtorrent file for the files a peer wants to share
	Format of .mtorrent file is as follows
		tracker1 ip:port
		tracker2 ip:port
		filename
		filesize
		hash string of entire file

2. Torrent Tracker:
	2.1 Creates and updates seederlist
	2.2 Communicates with other tracker for providing fault tolerance
	2.3 Add/remove a peer to seederlist based on peer requests

3. Torrent Peers:
	3.1 Creates .mtorrent file
	3.2 Share functionality to seed a file to other peers
	3.3 Get functionality to download a file from different peers
	3.4 Can upload multiple files simultaneously
	3.5 Can download multiple files simultaneously

Implementation Specification

1. To Compile and run 
	1.1 Compile peer.cpp : g++ peer.cpp -o peer -lcrypto -pthread
	1.2 Run peer.cpp : ./peer peerIP:peerPort tracker1ip:tracker1port tracker2ip:tracker2port logfile.txt

	1.1 Compile peer.cpp : g++ trackerServer.cpp -o tracker -lcrypto -pthread
	1.2 Run peer.cpp : ./tracker tracker1ip:tracker1port tracker2ip:tracker2port seederlist.txt logfile.txt

2. Share functionality at peer
	command: share filename

3. Get functionality at peer
	command: get filename

4. Remove File fuctionalilty
	command: remove filename.mtorrent

5. Close peer functionality
	command: close

Note : For get functionality to work you must have the .mtorrent file for that particular file which can be shared over other communication protocols

