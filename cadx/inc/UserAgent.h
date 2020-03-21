//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef USERAGENT_H
#define USERAGENT_H


#include "Iem/Iem.h"
#include "Util.h"
#include "string"


namespace CADX_SEG {

  
class UserAgent {

	protected:
	
	short debug;
	short log;
	char tag[1024];
	char outputDir[1024];
	ostream* outStream;

	public:
	
	UserAgent();
	UserAgent(const UserAgent& agent);
	UserAgent(const UserAgent& agent, char* tagSuffix);

	void setDebug(short _debug) {debug = _debug;}
	void setLog(short _log) {log = _log;}
	void setTag(char* _tag) {strcpy(tag, _tag);}
	void setOutputDir(char* _outputDir) {strcpy(outputDir, _outputDir);}
	void setOutStream(ostream& s) {outStream = &s;}

	void addTag(char* _tag) {strcat(tag, _tag);}

	char* getTag() {return tag;}
	short getDebug() {return debug;}
	short getLog() {return log;}
	ostream& getLogFile() {if(outStream != NULL) return *outStream; else return cerr;}
	char* getOutputDir() {return outputDir;}

	void writeImage(IemTImage<short>& img, char* name, char* type = "tif");
	void writeImage(IemTImage<unsigned char>& img, char* name, char* type = "tif");
	
	void writeDebugImage(IemTImage<short>& img, char* name) {if(debug) writeImage(img, name);}
	void writeDebugImage(IemTImage<unsigned char>& img, char* name) {if(debug) writeImage(img, name);}

	UserAgent& operator=(const UserAgent& agent);

	protected:
	
	void initialize();



};


} // Namespace CADX_SEG

#endif
