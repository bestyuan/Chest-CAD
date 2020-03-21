//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "UserAgent.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


UserAgent::UserAgent() {
	initialize();
}


UserAgent::UserAgent(const UserAgent& agent) {
	initialize();
	*this = agent; 
}


UserAgent::UserAgent(const UserAgent& agent, char* tagSuffix) {
	initialize();
	*this = agent;
	addTag(tagSuffix);
}


void UserAgent::initialize() {
	debug = 0;
	log = 0;
	outStream = NULL;
	strcpy(tag, "userAgent");
	strcpy(outputDir, "./");
}


void UserAgent::writeImage(IemTImage<short>& img, char* name, char* type) {
	char outFileName[1024];
	sprintf(outFileName, "%s/%s_%s.%s", outputDir, tag, name, type);
	iemWrite(img, outFileName);
}

void UserAgent::writeImage(IemTImage<unsigned char>& img, char* name, char* type) {
	char outFileName[1024];
	sprintf(outFileName, "%s/%s_%s.%s", outputDir, tag, name, type);
	iemWrite(img, outFileName);
}  
  
  
UserAgent& UserAgent::operator=(const UserAgent& agent) {
	debug = agent.debug;
	log = agent.log;
	strcpy(tag, agent.tag);
	strcpy(outputDir, agent.outputDir);
	outStream = agent.outStream;

	return *this;
}














