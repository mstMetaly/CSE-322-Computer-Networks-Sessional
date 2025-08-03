# CSE 322 - Computer Networks Laboratory

This repository contains implementations for three offline assignments covering fundamental concepts in computer networks, including HTTP file servers, MANET routing protocols, and TCP congestion control algorithms.

## 📁 Repository Structure

```
CSE 322/
├── OFFLINE1/          # HTTP File Server Implementation
├── OFFLINE2/          # MANET Routing Protocol (AODV) Implementation  
├── OFFLINE3/          # TCP Congestion Control Implementation
└── README.md
```

---

## 🚀 OFFLINE 1: HTTP File Server

### Overview
Implementation of a multi-threaded HTTP file server with file upload capabilities using Java sockets.

### Features
- **Multi-threaded Server**: Handles multiple client connections simultaneously
- **HTTP Protocol Support**: Implements HTTP/1.1 protocol for file serving
- **File Upload**: Supports file upload functionality with size validation
- **Directory Browsing**: Serves directory listings in HTML format
- **Content Type Detection**: Automatic MIME type detection for various file formats
- **Error Handling**: Comprehensive HTTP error responses (404, 500, etc.)

### Supported File Types
- Text files (.txt)
- Images (.jpg, .jpeg, .png, .gif)
- Videos (.mp4)
- Other files (application/octet-stream)

### Project Structure
```
OFFLINE1/2005110/
├── Server/
│   ├── Server.java           # Main server implementation
│   ├── HandleClient.java     # HTTP request handling
│   ├── Worker.java          # Thread worker for client connections
│   └── uploaded/            # Directory for uploaded files
├── Client/
│   ├── Client2.java         # Client implementation
│   ├── ClientThread.java    # Client upload thread
│   └── upload/              # Directory containing files to upload
└── File/                    # Server's file directory
```

### Usage

#### Starting the Server
```bash
cd OFFLINE1/2005110/Server
javac *.java
java Server
```

#### Running the Client
```bash
cd OFFLINE1/2005110/Client
javac *.java
java Client2
```

#### Client Commands
- `UPLOAD filename` - Upload a file to the server
- Supported formats: txt, jpg, png, mp4

#### Web Browser Access
- Open browser and navigate to `http://localhost:3000`
- Browse files and directories
- Download files directly

---

## 🌐 OFFLINE 2: MANET Routing Protocol (AODV)

### Overview
Implementation and analysis of the Ad-hoc On-Demand Distance Vector (AODV) routing protocol using NS-3 network simulator.

### Features
- **AODV Protocol Implementation**: Complete AODV routing protocol in NS-3
- **Congestion Control Enhancement**: Custom congestion control mechanism for AODV
- **Performance Analysis**: Comparison between standard AODV and enhanced CCAODV
- **Simulation Framework**: Comprehensive MANET simulation environment
- **Data Collection**: Automated metrics collection and analysis

### Key Components

#### AODV Implementation
- **Route Discovery**: RREQ/RREP message handling
- **Route Maintenance**: Route timeout and error handling
- **Congestion Control**: Custom congestion detection and response
- **Routing Table Management**: Dynamic route table updates

#### Simulation Features
- **Mobility Models**: Random waypoint mobility
- **Network Topologies**: Configurable node density and movement patterns
- **Traffic Patterns**: UDP data flows with configurable rates
- **Performance Metrics**: Throughput, delay, delivery ratio, drop ratio

### Project Structure
```
OFFLINE2/2005110/
├── code/
│   ├── aodv-routing-protocol.cc    # Main AODV implementation
│   ├── aodv-routing-protocol.h     # AODV header file
│   ├── aodv-packet.cc             # AODV packet handling
│   ├── aodv-packet.h              # Packet definitions
│   ├── aodv-rtable.cc             # Routing table implementation
│   ├── aodv-rtable.h              # Routing table header
│   └── manet-routing-compare.cc   # Simulation framework
├── graph/
│   ├── graph.py                   # Data visualization script
│   ├── manet.sh                   # Simulation execution script
│   ├── 2005110_AODV.csv          # AODV simulation results
│   ├── 2005110_CCAODV.csv        # CCAODV simulation results
│   └── 2005110.pdf               # Generated analysis graphs
└── 2005110.pdf                   # Detailed implementation report
```

### Usage

#### Building the Simulation
```bash
cd OFFLINE2/2005110/code
# Follow NS-3 build instructions
./waf configure
./waf build
```

#### Running Simulations
```bash
cd OFFLINE2/2005110/graph
./manet.sh
```

#### Generating Analysis Graphs
```bash
cd OFFLINE2/2005110/graph
python graph.py
```

### Performance Metrics Analyzed
- **Throughput**: Data delivery rate
- **End-to-End Delay**: Packet transmission latency
- **Delivery Ratio**: Successful packet delivery percentage
- **Drop Ratio**: Packet loss rate
- **Network Scalability**: Performance with varying node counts

---

## 📊 OFFLINE 3: TCP Congestion Control

### Overview
Implementation and analysis of custom TCP congestion control algorithms using NS-3, focusing on HTCP (High-Speed TCP) with dynamic window sizing.

### Features
- **Custom TCP Implementation**: HTCP-DSS (High-Speed TCP with Dynamic Slow Start)
- **Dynamic Window Sizing**: Adaptive congestion window based on RTT and throughput
- **Performance Comparison**: Analysis against standard TCP variants
- **Comprehensive Metrics**: Detailed performance analysis and visualization

### Key Components

#### HTCP-DSS Implementation
- **Dynamic Slow Start**: Adaptive initial window sizing
- **RTT-Based Adaptation**: Window growth based on round-trip time
- **Throughput Monitoring**: Real-time throughput measurement
- **Congestion Response**: Intelligent backoff mechanisms

#### Analysis Framework
- **Multiple TCP Variants**: Comparison with NewReno, CUBIC, etc.
- **Performance Metrics**: Congestion window, RTT, throughput analysis
- **Visualization Tools**: Automated graph generation
- **Statistical Analysis**: Comprehensive performance evaluation

### Project Structure
```
OFFLINE3/2005110/
├── code/
│   ├── tcp-htcpdss.cc             # HTCP-DSS implementation
│   ├── tcp-htcpdss.h              # HTCP-DSS header
│   ├── tcp-variants-comparison.cc # TCP comparison framework
│   ├── CMakeLists.txt             # Build configuration
│   └── 2005110.txt               # Implementation notes
└── 2005110_Report.pdf            # Detailed analysis report
```

### Usage

#### Building the Implementation
```bash
cd OFFLINE3/2005110/code
# Follow NS-3 build instructions with custom TCP module
./waf configure
./waf build
```

#### Running TCP Comparisons
```bash
# Run TCP variant comparison simulations
./waf --run "tcp-variants-comparison --tcp-variant=TcpHtcpDSS"
```

### TCP Variants Analyzed
- **TcpHtcpDSS**: Custom High-Speed TCP implementation
- **TcpNewReno**: Standard TCP NewReno
- **TcpCubic**: CUBIC congestion control
- **TcpHighSpeed**: High-speed TCP variant

### Performance Metrics
- **Congestion Window**: Window size evolution over time
- **Round-Trip Time**: RTT measurement and analysis
- **Throughput**: Data transfer rate analysis
- **Fairness**: Fairness among competing flows

---

## 🛠️ Technical Requirements

### OFFLINE 1 (HTTP Server)
- **Java 8+**: For server and client implementation
- **Network Access**: For socket programming
- **File System**: For file upload/download operations

### OFFLINE 2 & 3 (NS-3 Simulations)
- **NS-3 Network Simulator**: Version 3.30 or later
- **C++ Compiler**: GCC 7+ or Clang
- **Python 3**: For data analysis and visualization
- **Required Libraries**:
  - matplotlib (for graph generation)
  - pandas (for data processing)
  - numpy (for numerical computations)

### Build Dependencies
```bash
# For NS-3 simulations
sudo apt-get install build-essential libsqlite3-dev libboost-all-dev libssl-dev
sudo apt-get install python3-dev python3-setuptools python3-pip
pip3 install matplotlib pandas numpy
```

---

## 📈 Results and Analysis

### OFFLINE 1 Results
- Successfully implemented multi-threaded HTTP server
- File upload/download functionality working correctly
- Proper HTTP protocol compliance
- Error handling for various scenarios

### OFFLINE 2 Results
- Enhanced AODV with congestion control shows improved performance
- Better throughput and delivery ratio compared to standard AODV
- Scalable performance across different network sizes
- Comprehensive analysis graphs generated

### OFFLINE 3 Results
- HTCP-DSS shows improved performance in high-speed networks
- Better fairness and efficiency compared to standard TCP variants
- Dynamic window sizing provides adaptive performance
- Detailed performance analysis completed

---

## 👨‍💻 Author

**Student ID**: 2005110  
**Course**: CSE 322 - Computer Networks Laboratory  
**Institution**: Bangladesh University Of Engineering and Technology

---


## 📚 References

- **HTTP Protocol**: RFC 7230-7237
- **AODV Protocol**: RFC 3561
- **TCP Congestion Control**: RFC 5681
- **NS-3 Documentation**: https://www.nsnam.org/documentation/
- **High-Speed TCP**: RFC 3649 
