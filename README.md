Video Transfer System Using Socket Programming (UDP)

Overview

This project implements a video transfer system using socket programming in C. It employs the UDP (User Datagram Protocol) for efficient client-server communication. The system is designed to transmit video files while minimizing data loss and optimizing network resource utilization. Additionally, error detection and recovery mechanisms are incorporated to handle packet loss during the transfer, ensuring robust and reliable communication between the client and server.

Features
	•	UDP Communication: The system uses UDP for client-server communication, offering a fast and lightweight protocol suited for video transfer with minimal latency.
	•	Reliable Video Transmission: Despite using UDP, the system incorporates techniques for error detection and recovery to ensure the video file is transmitted reliably, even in the event of packet loss.
	•	Efficient Network Utilization: Optimizations are applied to reduce unnecessary network overhead, improving the performance of video transfers over potentially congested networks.
	•	Error Detection and Recovery: The system includes mechanisms to detect packet loss and recover from errors during video transfer, ensuring smooth and uninterrupted transmission.

System Design
	•	Client-Server Architecture: The system consists of two main components: the client, which sends the video file, and the server, which receives and saves it. The client and server communicate using UDP sockets.
	•	Error Handling: To ensure reliable transmission, the system tracks and verifies packet delivery. If a packet is lost or corrupted, the system can detect the issue and request retransmission, ensuring that the video file is accurately transmitted.
	•	Video File Transfer: The system allows large video files to be transferred efficiently, handling potential packet loss without compromising the integrity of the transmitted video.

Installation

To run this project, you need to have a C compiler and basic knowledge of socket programming.
	1.	Clone the repository:

git clone https://github.com/yourusername/video-transfer-system.git


	2.	Navigate to the project directory:

cd video-transfer-system


	3.	Compile the code:

gcc -o video_transfer_client client.c -pthread
gcc -o video_transfer_server server.c -pthread


	4.	Run the server:

./video_transfer_server


	5.	Run the client:

./video_transfer_client <video_file_path>



Usage
	•	Client: The client reads the video file from disk and transmits it over UDP to the server.
	•	Server: The server listens for incoming packets, reassembles the video file, and saves it once the complete file has been received.

The system ensures that even if packets are lost during the transmission, the client will be able to retransmit them, making the transfer robust and reliable.

Error Handling

The system uses sequence numbers for each packet to ensure that lost or corrupted packets are detected. When a packet is not received, the server sends a request for retransmission, and the client resends the missing packet.

Optimization
	•	Network Resource Utilization: The system optimizes the transmission process by adjusting the packet size and transmission speed, ensuring that network resources are used efficiently during video transfer.
	•	Low Latency: By using UDP and implementing efficient error handling, the system minimizes the delay in the transmission process, ensuring smooth and continuous video transfer.

Contributing

Feel free to fork the repository, make changes, and submit pull requests. Any improvements to enhance the transfer speed, error handling, or overall performance are highly encouraged.

