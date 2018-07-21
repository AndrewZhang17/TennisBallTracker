#include <iostream>
#include <vector>
#include "Ball.h"

using namespace std;

void Ball::addPosition(int x, int y, int radius, int time) {
	positionInfo p = { x, y, radius, time };
	if (positions.size() < 10) {
		positions.push_back(p);
	}
	else {
		positions.erase(positions.begin());
		positions.push_back(p);
	}
}

int Ball::speed() {
	if (positions.size() > 1) {
		int firstXTotal = 0, lastXTotal = 0, firstYTotal = 0, lastYTotal = 0,
			firstTimeTotal = 0, lastTimeTotal = 0, radiusTotal = 0,
			size = positions.size();
		double firstXAvg, lastXAvg, firstYAvg, lastYAvg, firstTimeAvg, lastTimeAvg, radiusAvg;
		auto p = positions.begin();
		for (p; p != positions.begin() + (size / 2); p++) {
			firstXTotal += p->x;
			firstYTotal += p->y;
			firstTimeTotal += p->time;
			radiusTotal += p->rad;
		}
		for (p; p != positions.end(); p++) {
			lastXTotal += p->x;
			lastYTotal += p->y;
			lastTimeTotal += p->time;
			radiusTotal += p->rad;
		}
		firstXAvg = ((double)firstXTotal) / (size / 2);
		firstYAvg = ((double)firstYTotal) / (size / 2);
		firstTimeAvg = ((double)firstTimeTotal) / (size / 2);
		lastXAvg = ((double)lastXTotal) / ((size + 1) / 2);
		lastYAvg = ((double)lastYTotal) / ((size + 1) / 2);
		lastTimeAvg = ((double)lastTimeTotal) / ((size + 1) / 2);
		radiusAvg = ((double)radiusTotal) / size;

		double distance = sqrt(pow(lastXAvg - firstXAvg, 2) + pow(lastYAvg - firstYAvg, 2));
		//convert distance to inches using radius
		distance = (distance*2.7) / radiusAvg;
		if (distance != 0) {
			double time = (lastTimeAvg - firstTimeAvg) / 1000;
			double speed = (distance / time)*IN_PER_SEC_TO_MPH;
			cout << "speed: " << round(speed) << " mph" << endl;
			return speed;
		}
	}
	return 0;
}

void Ball::clear(int time) {
	if (time == 0) {
		positions.clear();
	}
	else if (positions.size() > 0) {
		auto rp = positions.rbegin();
		if (time - rp->time > 1000) {
			positions.clear();
		}
	}
}