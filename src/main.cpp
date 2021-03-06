/*
 * main.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: user
 */

#include <HamsterAPIClientCPP/Hamster.h>
#include <iostream>
#include "Robot.h"
#include "LocalizationManager.h"

using namespace std;
using namespace HamsterAPI;
HamsterAPI::Hamster * hamster;

int main() {
	try {
		hamster = new HamsterAPI::Hamster(1);
		sleep(1);
		const int mapHeight = 500;
		const int mapWidth = 500;
		const double mapResolution = 0.05;

		cv::namedWindow("Camera Image");
		sleep(1);
		OccupancyGrid ogrid = hamster->getSLAMMap();

		Robot robot(hamster);
		LocalizationManager locManager(ogrid, hamster);

		locManager.initParticles();


		while (hamster->isConnected()) {
			try {
				sleep(1);


					cv::Mat mat(mapWidth,mapHeight,CV_8UC3,cv::Scalar(0,0,0));
					cv::imshow("Camera Image",mat);
					cv::waitKey(2);



				HamsterAPI::LidarScan ld = hamster->getLidarScan();

				if (ld.getDistance(180) < 0.4) {
					hamster->sendSpeed(-0.5, 0);
					cout << "Front: " << ld.getDistance(180) << endl;
				} else if (ld.getDistance(180) < 0.8) {
					hamster->sendSpeed(0.5, 45);
					cout << "Front: " << ld.getDistance(180) << endl;
				}

				else
					hamster->sendSpeed(1.0, 0);


				robot.updatePose();
				locManager.updateParticles(robot.getDeltaX(), robot.getDeltaY(), robot.getDeltaYaw());
				locManager.printParticles();
				sleep(0.5);





			} catch (const HamsterAPI::HamsterError & message_error) {
				HamsterAPI::Log::i("Client", message_error.what());
			}

		}
	} catch (const HamsterAPI::HamsterError & connection_error) {
		HamsterAPI::Log::i("Client", connection_error.what());
	}
	return 0;
}

