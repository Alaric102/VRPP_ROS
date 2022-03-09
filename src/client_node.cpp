#include <iostream>
#include <string>

#include <ros/ros.h>
#include <geometry_msgs/Transform.h>

#include "TcpClient.h"

#define DEFAULT_PORT "12345"
#define DEFAULT_ADDR "localhost"

enum UnityCommands {
    setStartPoint = 1,
    setGoalPoint,
    startPlanning
};

// Client
// 1. Initialize Winsock.
// 2. Create a socket.
// 3. Connect to the server.
// 4. Send and receive data.
// 5. Disconnect.

int main(int argc, char **argv){
    TcpClient tcpClient(DEFAULT_ADDR, DEFAULT_PORT);

    if (!tcpClient.init()){
        std::cout << "Failed to init TCP Client." << std::endl;
        return 1;
    }

    if (!tcpClient.createSocket()){
        std::cout << "Failed to Create socket." << std::endl;
        return 1;
    }

    if (!tcpClient.connectScoket()){
        std::cout << "Failed to connect socket." << std::endl;
    }

    ros::init(argc, argv, "client_node");
    ros::NodeHandle nh;
    ros::Rate loopRate(10);

    ros::Publisher startStatePub = nh.advertise<geometry_msgs::Transform>("startState", 10);
    ros::Publisher goalStatePub = nh.advertise<geometry_msgs::Transform>("goalState", 10);
    static geometry_msgs::Transform stateMsg;

    while(nh.ok()){
        if (tcpClient.isConnected()){
            std::cout << ".";
        } else {
            tcpClient.Reconnect();
            std::cout << "Reconnecting" << std::endl;
        }
        
        int cmd = tcpClient.recvCommand();
        switch (cmd) {
            case setStartPoint: {
                stateMsg.translation = tcpClient.getVector3();
                stateMsg.rotation = tcpClient.getQuaternion();
                startStatePub.publish(stateMsg);
                break;
            } case setGoalPoint: {
                stateMsg.translation = tcpClient.getVector3();
                stateMsg.rotation = tcpClient.getQuaternion();
                goalStatePub.publish(stateMsg);
                break;
            } case startPlanning: {

                break;
            }       
            default:{
                std::cout << "Unrecognized command: " << cmd << std::endl;
                break;
            }
        }
        loopRate.sleep();

    }
    return 0;
}