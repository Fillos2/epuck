#include "ros/ros.h"
#include "epuck_control/epuck.h"

//git test


int main(int argc, char **argv) {

	ros::init(argc, argv, "listener");
	e_puck  epuck;
	epuck.~e_puck();
	return 0;
}

