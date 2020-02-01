#include "DataPoint.h"

DataPoint::DataPoint(int songId, int time) : time(time), songId(songId) { }

const int DataPoint::getTime() const { return time; }

const int DataPoint::getSongId() const { return songId; }
