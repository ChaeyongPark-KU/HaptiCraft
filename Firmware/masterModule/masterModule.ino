/****
 *  Firmware for HaptiCraft (IEEE TVCG 2026)
 *  Author: Chaeyong Park
 *  Copyright (c): 2026. AXIS Laboratory, Korea University
 *  WEMOS D1 MINI (ESP8266)
 * 
 *  Master module
 *  - The master module is responsible for receiving the commands from the user and sending the commands to the corresponding haptic modules.
 *  - The master module also manages the connected haptic modules and their states.
 *  - The communication between the master module and the haptic modules is done through the WiFi mesh network using the painlessMesh library.
 *  - The master module receives the commands from the user through the serial communication and parses the commands to send the corresponding commands to the haptic modules.
 *  
 *  The command format is as follows:
 *  $N1C1,N2C2 ...
 *  NX: Name of a module to operate
 *  CX: The command of the module
 *
 */
#include "painlessMesh.h"
#include <Vector.h>

#define MESH_PREFIX "modularHaptics"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555
#define NODE_COUNT_MAX 20
#define CEHCK_THRESHOLD 2

/* A new data structure */
typedef struct {
  uint32_t nodeID;
  String nodeName;
  bool check;
} Node;

/* Serial Communication */
String inString;

/* Node Information */
String nodeName = "MM";
Node storage_array[NODE_COUNT_MAX];
Vector<Node> connectedNodes;
int isCheck = 0;
int nodeNum = 0;

/* General Information */
bool isTouched = false;

/* Task Functions */
Scheduler userScheduler;  // to control your personal task
void checkAndRemoveNode();
Task taskRemoveNode(TASK_SECOND * 3, TASK_FOREVER, &checkAndRemoveNode);
//void serialTransfer();
//Task taskSerialTransfer(TASK_SECOND, TASK_FOREVER, &serialTransfer);

/* General Functions */
String getNodeNameFromID(Vector<Node> *v, uint32_t _nodeID);
void setNodeCheck(uint32_t _nodeID);
void checkAndUpdateNode(uint32_t _nodeID, String _nodeName);
void removeNode(uint32_t _nodeID);
void removeAllNodes();
void showConnectedList(bool isMesh);
int numberOfConnectedMesh(bool isMesh);

/* Functions for WiFi mesh network */
painlessMesh mesh;
void receivedCallback(uint32_t _nodeID, String &msg);
void newConnectionCallback(uint32_t _nodeID);
void changedConnectionCallback();
void droppedConnectionCallback(uint32_t _nodeID);

/* Get the node name from the node ID */
String getNodeNameFromID(uint32_t _nodeID) {
  for (int i = 0; i < nodeNum; i++) {
    if (connectedNodes[i].nodeID == _nodeID) {
      return connectedNodes[i].nodeName;
    }
  }
}

/* Set the check value of the node to true when the node is connected */
void setNodeCheck(uint32_t _nodeID) {
  int sizeVector = connectedNodes.size();
  for (int i = 0; i < sizeVector; i++) {
    if (connectedNodes[i].nodeID == _nodeID) {
      connectedNodes[i].check = true;
    }
  }
}

/* Check the node connection and update the node information if the node is already connected. 
 * If the node is not connected, add the node to the connected node list */
void checkAndUpdateNode(uint32_t _nodeID, String _nodeName = "NN") {
  int sizeVector = connectedNodes.size();
  int sizeMesh = mesh.getNodeList().size();
  int flagExist = false;

  if (sizeVector <= sizeMesh) { // Need to add a new node
    for (int i = 0; i < sizeVector; i++) {
      if (connectedNodes[i].nodeID == _nodeID) {
        flagExist = true;
        if (connectedNodes[i].nodeName != _nodeName) {
          connectedNodes[i].nodeName = _nodeName;
          Serial.printf("UPDATE MODULE: ID-%u NAME-%s\n", _nodeID, _nodeName);
          break;
        }
      }
    }
    if (!flagExist) {
      connectedNodes.push_back({_nodeID, _nodeName, true});
      nodeNum++;
      Serial.printf("NEW MODULE: ID-%u NAME-%s\n", _nodeID, _nodeName);
      String _command = "#C" + _nodeName + "\n";
      Serial.printf(_command.c_str());
    }
  }
}

/* Check the node connection and remove the node if it is not connected */
void checkAndRemoveNode() {
  if (isCheck == CEHCK_THRESHOLD){
    int sizeVector = connectedNodes.size();
    int sizeMesh = mesh.getNodeList().size();

    for (int i = 0; i < sizeVector; i++) { // Remove
      if (connectedNodes[i].check) {
        connectedNodes[i].check = false;
      } else {
        Serial.printf("REMOVE NODE ID: %u Name: %s\n", connectedNodes[i].nodeID, connectedNodes[i].nodeName.c_str());
        String _command = "#D" + connectedNodes[i].nodeName + "\n";
        Serial.printf(_command.c_str());
        connectedNodes.remove(i);
        nodeNum--;
      }
    }
    if (sizeVector < sizeMesh) { // Check the difference between the number of mesh and vector
      SimpleList<uint32_t> nodes = mesh.getNodeList();
      SimpleList<uint32_t>::iterator _node = nodes.begin();
      while(_node != nodes.end()) {
        uint32_t _nodeID = *(_node);
        bool existInNode = false;
        for (int i = 0; i < sizeVector; i++) {
          if (connectedNodes[i].nodeID == _nodeID) {
            existInNode = true;
            break;
          }
        }
        if (!existInNode) {
          mesh.sendBroadcast("I");
          break;
        }
        _node++;
      }
    }
    
  } else {
    mesh.sendBroadcast("D");
  }

  isCheck++;
  if (isCheck > CEHCK_THRESHOLD) {
    isCheck = 0; 
  }
}

/* Just for test */
void serialTransfer() {
  Serial.printf("z\n");
}

/* Remove the node if it is not connected */
void removeNode(uint32_t _nodeID) {
  for (int i = 0; i < nodeNum; i++) {
    if (connectedNodes[i].nodeID == _nodeID) {
      Serial.printf("REMOVE NODE ID: %u Name: %s\n", connectedNodes[i].nodeID, connectedNodes[i].nodeName.c_str());
      String _command = "#D" + connectedNodes[i].nodeName + "\n";
      Serial.printf(_command.c_str());
      connectedNodes.remove(i);
      nodeNum--;
      break;
    }
  }
}

/* Remove all nodes */
void removeAllNodes() {
  connectedNodes.clear();
  nodeNum = 0;
}

/* Show the connected nodes */
void showConnectedMesh(bool isMesh = false) {
  if (isMesh) {
    SimpleList<uint32_t> nodes = mesh.getNodeList();
    SimpleList<uint32_t>::iterator _node = nodes.begin();
    int cnt = 1;
    while(_node != nodes.end()) {
      Serial.printf("%d: ID-%u\n", cnt++, *(_node));
      _node++;
    }
  } else {
    int _size = connectedNodes.size();
    for (int i = 0; i < _size; i++) {
      Serial.printf("%d: ID-%u NAME-%s\n", i+1, connectedNodes[i].nodeID, connectedNodes[i].nodeName);
    }
  }
}

/* Show the number of connected nodes */
int numberOfConnectedMesh(bool isMesh = false) {
  if (isMesh) {
    return mesh.getNodeList().size();
  } else {
    return connectedNodes.size();
  }
}

// Needed for painless library
void receivedCallback(uint32_t _nodeID, String &msg) {
  String command = msg.substring(0, 1);

  if (command == "N") {
    String _nodeName = msg.substring(1, 3);
    checkAndUpdateNode(_nodeID, _nodeName);
  } else if (command == "H") {
    setNodeCheck(_nodeID);
  } else if (command == "C") {
    if (!isTouched && msg.substring(1, 2) == "O") {
      isTouched = true;
      String _command = "#T" + getNodeNameFromID(_nodeID) + "\n";
      Serial.printf(_command.c_str());
    } else if (isTouched && msg.substring(1, 2) == "F") {
      isTouched = false;
      String _command = "#R" + getNodeNameFromID(_nodeID) + "\n";
      Serial.printf(_command.c_str());
    }
  } else {
    Serial.printf("JUST MSG FROM %u MSG: %s\n", _nodeID, msg.c_str());
  }
}

/* When a new node is connected, send the information of the master module to the new node and update the connected node list */
void newConnectionCallback(uint32_t _nodeID) {
  //connectedNodes.push_back({_nodeID, "NN"});
  Serial.printf("FIND A NEW NODE: ID-%u\n", _nodeID);
  mesh.sendBroadcast("I");
  //nodeNum++;
}

/* When the connection is changed, check the connection and update the connected node list */
void changedConnectionCallback() {
  Serial.printf("CHANGED CONNECTIONS\n");
  mesh.sendBroadcast("I");
}

/* When a node is dropped, remove the node from the connected node list */
void droppedConnectionCallback(uint32_t _nodeID) {
  Serial.printf("DROPPED CONNECTIONS %u\n", _nodeID);
}

/* Setup and loop functions */
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);

  connectedNodes.setStorage(storage_array);
  
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onDroppedConnection(&droppedConnectionCallback);

  userScheduler.addTask(taskRemoveNode);
  taskRemoveNode.enable();
}

/* In the loop function, it will run the user scheduler and check the serial communication */
void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if (Serial.available() > 0) {
    inString = Serial.readString();
    Serial.print(inString);
    String header = inString.substring(0, 1);

    if (header == "$") {
      mesh.sendBroadcast(inString);
    } else if (header == "A") { // Show all connected nodes
      Serial.printf("Number of nodes is %d\n", numberOfConnectedMesh());
      showConnectedMesh();
    } else if (header == "D") { // Show all connected meshes
      Serial.printf("Number of nodes is %d\n", numberOfConnectedMesh(true));
      showConnectedMesh(true);      
    } else if (header == "E" || header == "T" || header == "M" || header == "Q" || header == "P" || header == "W") { // Test command
      mesh.sendBroadcast(inString);
    } else if (header == "R") {
      removeAllNodes();
    }
  }
}
