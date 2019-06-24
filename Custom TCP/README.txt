In order to compile the server, it is sufficient to go to the Server folder and type make.
In order to compile the client, it is sufficient to go to the Client folder and type make.

In order to run the server, it is sufficient to type: ./server <FilenameToSend> <ServerPortNumber> <ServerIPAddress/Hostname>
In order to run the client, it is sufficient to type: ./client <ServerIPAddress/Hostname> <ServerPortNumber> <FilenameToSave> <ClientIPAddress/Hostname>

The program works as follow:
First off the client sends a request to the server to let him know that it wants to start to download the file.
The server then replies with a port number, that it will be used by the server to send the requested data to the client.
After that the client binds a socket to that port, in this way it can receive the data.
After a successful binding the client sends to the server a new server port number, that it will be used by the client to send
the acks data to the server.
In this way both server and client have an exclusive path for exchanging data, after this initial setup they can start
to exchange packets and acks.

Reliability:
Reliability has been implemented as described in Chapter3 slides 37-38 (rdt 3.0 in action).
Basically when the client receives a packet from the server it checks if it has actually received it or not, if not
the client will not send its ack to the server and the server will resend the packet, when the timeout runs out.
The client also will notice if the received packet is a duplicate, in this case it will not write its content in the file.
After being sure that the client has received a "good packet" it will send and ack to the server.
If the server doesn't receive the ack or the received ack is equal to the last one received, it will trivially resend the
packet until it gets the reply that it is expecting.

Multisessioning:
Reliability has been implemented trough multiprocessing, namely when the server receives a request from the client,
it creates a new instance of itself to handle the connection and the data transfer with the client, while the "father"
process awaits for a new request from a new client, when it receives it, it creates a new instance...

Debug mode:
I also implemented a DEBUG mode both for the client and the server, the flag to set this mode can be found in both the
headers for client and server, to activate it it is sufficient to set the flag to 1.
Other than printing useful information for debugging, the debug mode allows us to define the packet drop rate (server side)
and the ack drop rate (client side) simply by setting the macro DROP_RATE through a numeric value.
The program will drop one ack/packet every $(DROP_RATE).
