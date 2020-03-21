//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Watershed.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemTImageIter.h"


using namespace CADX_SEG;



Watershed::Watershed() {
	initialize();
}


Watershed::~Watershed() {
	if(zones != NULL) delete[] zones;
	if(hist != NULL) delete[] hist;
	if(boundary != NULL) delete[] boundary;
}


void Watershed::initialize() {
	nZones = 0;
	zones = NULL;
	hist = NULL;
	zMax = 0;
//	nHistBins = 100;
	nHistBins = 50;
	histScale = 1.0;
	boundary = NULL;
}


void Watershed::makeZones(IemTImage<short>& _img, IemTImage<unsigned char>& _imgSupportMap, list<Point>& peaks) {

	img = _img;
	imgSupportMap = _imgSupportMap;
	
	imgZoneMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgZoneMap = 0;  

	nZones = peaks.size();
	 
	zones = new Cluster[nZones];

	list<Point>::iterator iter;
	
	// Find the highest peak.
	zMax = SHRT_MIN;
	for(iter = peaks.begin(); iter != peaks.end(); iter++) {
		short z = (*iter).z;
		if(z > zMax) zMax = z;
	}

	// Don't make the histogram too large.	
	nHistBins = Util::min(nHistBins, zMax + 1);
	
	hist = new list<Point>[nHistBins];
	histScale = (double)(nHistBins - 1) / (double)zMax;
	
	boundary = new list<Point>[nZones];
	
	imgAltitude = IemTImage<short>(1, img.rows(), img.cols());
	imgAltitude = zMax;
	
	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
			if(imgSupportMap[0][y][x] != 0) { 
			     short v = zMax - img[0][y][x];
			     imgAltitude[0][y][x] = Util::min(v, zMax);
			}
	}}

	userAgent.writeDebugImage(imgAltitude, "Watershed_Altitude");

	long i = 0;
	for(iter = peaks.begin(); iter != peaks.end(); iter++, i++) {
		long x = (*iter).x;
		long y = (*iter).y;
		short z = img[0][y][x];

		zones[i].setXc(x);
		zones[i].setYc(y);
		zones[i].setZc(z);
		zones[i].setNumber(i+1);

		Point point(x, y, z);
		zones[i].addPoint(point);
		imgZoneMap[0][y][x] = zones[i].getNumber();
		
		boundary[i].push_back(point);
		
		userAgent.getLogFile() << "\nWatershed: zone= " << zones[i].getNumber()
		 << " x= " << x << " y= " << y << " z= " << z;
	}

	buildHistogram();
	
	for(long k = 0; k < nHistBins; k++) {
	     add(k);
	     char name[1024];
	}
	
//	sprintf(name, "Watershed_ZoneMap_%d", 30);
	userAgent.writeDebugImage(imgZoneMap, "Watershed_ZoneMap");
}


void Watershed::buildHistogram() {
 
	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {

			if(imgSupportMap[0][y][x] != 0) {
				short z = imgAltitude[0][y][x];

				if(z < 0 || z > zMax) continue;
				 
				Point point(x, y, img[0][y][x]);
				hist[(long)(histScale * z)].push_back(point);
			}
	}}
}

 
void Watershed::add(long k) {

	list<Point> addList;

	// Cycle thru pixels with code value k.
	for(list<Point>::iterator iter = hist[k].begin(); iter != hist[k].end(); iter++) {
		long x = (*iter).x;
		long y = (*iter).y;
		
		// Set the state field to the index of the zone the point
		// is to be added to. Do not want to change zone list
		// until all points are processed.
		(*iter).state = findClosestZone(x, y);
	}
	
	for(iter = hist[k].begin(); iter != hist[k].end(); iter++) {
		long x = (*iter).x;
		long y = (*iter).y;
		long index = (*iter).state;

		zones[index].addPoint(*iter);
		imgZoneMap[0][y][x] = zones[index].getNumber();
	}
	
	updateBoundarys();
}


long Watershed::findClosestZone(long x0, long y0) {

	long d2min = LONG_MAX;
	long indexMin = 0;

	for(long i = 0; i < nZones; i++) {
		
	     for(list<Point>::iterator iter = boundary[i].begin(); iter != boundary[i].end(); iter++) {
			short x = (*iter).x;
			short y = (*iter).y;
			
			long dx = x - x0;
			long dy = y - y0;

			long d2 = dx * dx + dy * dy;
			
			if(d2 < d2min) {d2min = d2; indexMin = i;}
		}

	}

	return indexMin;
}


/*
void Watershed::updateBoundarys() {

	for(long i = 0; i < nZones; i++) {boundary[i].clear();}
 
	for(long x = 1; x < img.cols() - 1; x++) {
		for(long y = 1; y < img.rows() - 1; y++) {

			if(imgSupportMap[0][y][x] != 0) {
				short z = imgZoneMap[0][y][x];

				if(z == 0) continue; 
				
				if(imgZoneMap[0][y][x+1] != z
				 || imgZoneMap[0][y][x-1] != z
				 || imgZoneMap[0][y+1][x] != z
				 || imgZoneMap[0][y-1][x] != z) {

					Point point(x, y, 0);
					boundary[z-1].push_back(point);
				} 
			}
	}}
}
*/

void Watershed::updateBoundarys() {

	for(long i = 0; i < nZones; i++) {boundary[i].clear();}

	IemTImageIter<unsigned char> iterSupportMap(imgSupportMap);
	IemTImageIter<unsigned char> iterZoneMap(imgZoneMap);
	IemTImageIter<unsigned char> iterZoneMapEast(imgZoneMap);
	IemTImageIter<unsigned char> iterZoneMapWest(imgZoneMap);
	IemTImageIter<unsigned char> iterZoneMapNorth(imgZoneMap);
	IemTImageIter<unsigned char> iterZoneMapSouth(imgZoneMap);

	long cols = img.cols() - 1;
	long rows = img.rows() - 1;

	for(long y = 1; y < rows; y++) {
	                               	
		iterSupportMap.setPos(y, 1);
		iterZoneMap.setPos(y, 1);
		iterZoneMapEast.setPos(y, 2);
		iterZoneMapWest.setPos(y, 0);
		iterZoneMapNorth.setPos(y+1, 1);
		iterZoneMapSouth.setPos(y-1, 1);

		for(long x = 1; x < cols; x++) { 
			if(iterSupportMap[0] != 0 && iterZoneMap[0] != 0) {
				short z = iterZoneMap[0];

				if(iterZoneMapEast[0] != z || iterZoneMapWest[0] != z || iterZoneMapNorth[0] != z || iterZoneMapSouth[0] != z) {
					Point point(x, y, 0);
					boundary[z-1].push_back(point);
				}
			}
			iterSupportMap.colInc(); iterZoneMap.colInc(); iterZoneMapEast.colInc();
			iterZoneMapWest.colInc(); iterZoneMapNorth.colInc(); iterZoneMapSouth.colInc();
		}
	}
}



