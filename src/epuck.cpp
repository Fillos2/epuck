/*
 * epuck.cpp
 *
 *  Created on: 15/apr/2015
 *      Author: filippo
 */

#include "epuck_control/epuck.h"
#include "ros/ros.h"
//#include "roslib.h"
#include "std_msgs/Duration.h"

//#include "serial/serial.h"
#include "epuck_control/serialcomm.h"
#include <string>

e_puck::e_puck() {

	last_vel_update_=left_freq_=right_freq_=0;
	ser_=new SerialComm();
	std::string namesp = ros::this_node::getNamespace();
	std::string buffer;
	std::string num= namesp.substr(7,4);
	char numero_epuck[6];
	flag =0;
	ros::Subscriber sub = nh_.subscribe("cmd_vel", 1000, &e_puck::cmd_vel_Callback, this);
	ros::Publisher chatter_pub = nh_.advertise<std_msgs::Duration>("chatter", 1000);
	connect(num);
	ROS_INFO_STREAM("Numero "<< num);
	ros::Time t;
	ros::Duration d;
	std_msgs::Duration msg;
	ros::Rate loop_rate(20);

	while (ros::ok()) {

		if((ros::Time::now().toSec()-last_vel_update_)> 2){
			right_freq_=left_freq_=0;
		}
		
		ROS_INFO_STREAM(right_freq_);
		ROS_INFO_STREAM(left_freq_);
		char buffer[30];
		int n = sprintf(buffer, "D,%d,%d\r",left_freq_,right_freq_);

		ROS_INFO_STREAM("Serial Port"<<num);
		t = ros::Time::now();
		int scritti = ser_->writeData(buffer,n,1000000);
		d=ros::Time::now()-t;
		msg.data = d;
		chatter_pub.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
}

e_puck::~e_puck() {
	
}

void e_puck::cmd_vel_Callback(const geometry_msgs::Twist::ConstPtr& msg) {
	double v = msg->linear.x;
	double w = msg->angular.z;
	double wr,wl;
	wr=((2*v)+(0.053*w))/(0.041);
	wl=((2*v)-(0.053*w))/(0.041);
	right_freq_=(1000/6.28)*wr;
	left_freq_=(1000/6.28)*wl;
	last_vel_update_=ros::Time::now().toSec();
}

int e_puck::connect(std::string num) {

	ROS_INFO_STREAM("Numero "<< num);
	std::string rfnum;

	if(num == "2037"){
		rfnum="37";
	}
	else if(num == "1702"){
			rfnum="102";
	}
	else if(num == "2034"){
			rfnum="34";
	}
	else if(num == "2053"){
			rfnum="53";
	}
	else if(num == "1701"){
			rfnum="101";
	}
	else if(num == "2041"){
			rfnum="41";
	}
	else if(num == "2042"){
			rfnum="42";
	}
	else if(num == "2033"){
			rfnum="33";
	}
	else ROS_INFO_STREAM("errore"<<'\n');
	std::string str = "/dev/rfcomm"+rfnum;
	char * nome = new char [str.length()+1];
	std::strcpy (nome, str.c_str());
	ser_->connect(nome);
	
	ros::Duration(10).sleep();
	 
	ROS_INFO_STREAM("Serial Port initialized");
	ser_->writeData(std::string("L,1,1\r").c_str(),std::string("L,1,1\r").length(),1000000);
	ROS_INFO_STREAM("L,1,1\r");
	return 0;
}

