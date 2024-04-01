#include "Vehicle.h"

Vehicle::Vehicle(int depotNumber,double maxRouteTime,double vehicleCapacity):  //特定的仓库号、最大行驶时间和车辆容量。
depotNumber(depotNumber), maxRouteTime(maxRouteTime), vehicleCapacity(vehicleCapacity)
{}
Vehicle::~Vehicle(void){}