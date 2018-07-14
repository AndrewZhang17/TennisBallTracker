#pragma once
#include <vector>

class Ball {
private:
	//conversion factor from in/s to mph
	const double IN_PER_SEC_TO_MPH = 3600.0 / (5280 * 12);
	//stores x and y coordinate, radius in pixels, and time of position
	struct positionInfo {
		int x;
		int y;
		int rad;
		int time;
	};
	//stores position information to calculate speed
	std::vector<positionInfo> positions;
public:
	//add new position information, if already 10 positions, remove earliest
	void addPosition(int x, int y, int radius, int time);
	//calculate speed by averaging first half and last half positions, then v=d/t
	void speed();
	//clear positions if no objects found, delay too long, or noisy threshold 
	void clear(int time = 0);
};
