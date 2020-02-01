#pragma once

class DataPoint {
private:
	int time;
	int songId;

public:
	DataPoint(int songId, int time);

	const int getTime() const;
	const int getSongId() const;
};