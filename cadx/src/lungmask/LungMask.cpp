#include "Iem/Iem.h"
#include "Iem/IemIO.h"
#include "Chest_PA_AP.h"
#include "LungMaskFunctions.h"
#include "ASM.h"


//#include "minmax.h"

using namespace std;

vector<byte *> Around_the_Points(IemTPlane<byte> &img, vector<byte *> points) {
	vector<byte *> New_points;

	for (int ii=0; ii<points.size(); ii++) {
		byte *p=points[ii];
		if (*p==0) {
			*p = 255;
			if(*(p+1)==0) New_points.push_back(p+1);
			if(*(p-1)==0) New_points.push_back(p-1);
			if(*(p+img.cols())==0) New_points.push_back(p+img.cols());
			if(*(p-img.cols())==0) New_points.push_back(p-img.cols());
		}
	}
	return New_points;
}

void FillShape(IemPlane output, int x0, int y0) {

	IemTPlane<byte> img(output);
	vector<byte *> points;
	byte *p=img[x0]+y0;
	points.push_back(p);

	while(points.size()>0) points = Around_the_Points(img,points);
}

void DrawShape(IemPlane &output, vector<float> &ShapeV, int factor){

	int len = ShapeV.size(), ii, jj, step;
	int xx0=factor*ShapeV[len-2],yy0=factor*ShapeV[len-1],xx1,yy1;
	float slope;
	for (ii=2; ii<ShapeV.size(); ) {
		xx1 = int(factor*ShapeV[ii++]);
		yy1 = int(factor*ShapeV[ii++]);
		if(ii==42) ii=0;
		if(ii==2) ii=42;
		step =1;
//		printf("%5d  %5d  %5d  %5d  %5d  %5d  %5d\n",ii,output.rows(),output.cols(),xx0,xx1,yy0,yy1);
		if (abs(xx1-xx0)>abs(yy1-yy0)) {
			slope = float(yy1-yy0)/(xx1-xx0);
			if(xx0>xx1) step=-1;
			for(jj=xx0;jj!=xx1;jj+=step) {
				int xx = jj;
				int yy = yy0+slope*(jj-xx0);
				if(xx<0)xx=0;
				else if(xx>=output.rows())xx=output.rows()-1;
				if(yy<0)yy=0;
				else if(yy>=output.cols())yy=output.cols()-1;
				output.put(xx,yy,255);	
			}
		} else {
			slope = float(xx1-xx0)/(yy1-yy0);
			if(yy0>yy1) step=-1;
			for(jj=yy0;jj!=yy1;jj+=step) {
				int xx = xx0+slope*(jj-yy0);
				int yy = jj;
				if(xx<0)xx=0;
				else if(xx>=output.rows())xx=output.rows()-1;
				if(yy<0)yy=0;
				else if(yy>=output.cols())yy=output.cols()-1;
				output.put(xx,yy,255);	
			}
		}
		xx0 = xx1;
		yy0 = yy1;
	}
	
	int x0 = factor*(ShapeV[0] + ShapeV[len-2])/2;
	int y0 = factor*(ShapeV[1] + ShapeV[len-1])/2;
	FillShape(output,x0,y0);
}


// argv[1]: image file
// argv[2]: Parameter file
// argv[3]: Left Lung Model.
// argv[4]: Right Lung Model.

int main(int argc, char** argv) {
    
//	int time = clock();
//	printf("TIME2: %d\n",time);
    try {

        ASM LeftLung, RightLung;
        IemImage image = iemRead(argv[1]);
//		time = clock();
//		printf("TIME2: %d\n",time);

        Chest_PA_AP chest;
		if(argc>7)chest.setVerbose(true);
		chest.setBoostFactor(0.10);

		string out1 = "_mask.png";
		string out2 = "_overlay.tif";
		// remove directory
		string tmpnm = argv[1];
		int name_start = tmpnm.find_last_of("\\");
		if (name_start!=-1) tmpnm = tmpnm.substr(name_start+1,tmpnm.length()); 
		// remove extention
		name_start = tmpnm.find_last_of(".");
		if (name_start!=-1) tmpnm = tmpnm.substr(0,name_start); 
//		printf("Filename:  %s\n",tmpnm.c_str());

		if(strcmp(argv[5],"-")==0) out1 = tmpnm + out1;
		else                       out1 = argv[5];
		if(strcmp(argv[6],"-")==0) out2 = tmpnm + out2;
		else                       out2 = argv[6];

        bool rc;
        rc =  LeftLung.LoadModel(argv[2], argv[3]);
        if (!rc) {
            printf("Error loading the left model or paramater file.\n");
            exit(1);
        }
        rc = RightLung.LoadModel(argv[2], argv[4]);
        if (!rc) {
            printf("Error loading the right model or paramater file.\n");
            exit(1);
        }
        
        float left_top_x, left_bottom_x, right_top_x, right_bottom_x;
        float left_top_y, left_bottom_y, right_top_y, right_bottom_y;
        
        chest.processing(image[0]);
        image = iemReorient(image, chest.getReorientOp());
        if(chest.isAP()) {
			image = iemReorient(image,iemFVER);
			printf("Image is AP\n");
		}

		IemReorientOp rop = chest.getReorientOp();
		if(rop==iemRCW) rop = iemRCCW;
		else if(rop==iemRCCW) rop = iemRCW;

        
//    IemLinearArray spine      = chest.getSpineLine();
        IemLinearArray right_lung  = chest.getLeftLungLine();
        IemLinearArray left_lung = chest.getRightLungLine();
        
        int ii;
        int length = left_lung.length();
        for ( ii=0; ii<length; ii++ ) {
            if(left_lung[ii]>0) {
                left_top_y = ii;
                left_top_x = left_lung[ii];
                break;
            }
        }
        for ( ii=0; ii<length; ii++ ) {
            if(right_lung[ii]>0) {
                right_top_y = ii; 
                right_top_x = right_lung[ii];
                break;
            }
        }
        
        for ( ii=length-1; ii>=0; ii-- ) {
            if(left_lung[ii]>0) {
                left_bottom_y = ii;
                left_bottom_x = left_lung[ii];
                break;
            }
        }
        for ( ii=length-1; ii>=0; ii-- ) {
            if(right_lung[ii]>0) {
                right_bottom_y = ii; 
                right_bottom_x = right_lung[ii];
                break;
            }
        }
        if(argc>7)printf("Lung locations: %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f\n",
               left_top_x, left_bottom_x, right_top_x, right_bottom_x,
               left_top_y, left_bottom_y, right_top_y, right_bottom_y);
        


		IemImage img = image; // hiGaussianBlur(image, 0.1);

		int ww=100;
		img = iemPad(img,img.rows()+ww+ww,img.cols()+ww+ww,ww,ww);

        vector<float> ShapeVL,ShapeVR;
		IemTImage<byte> imageT(img);
	    LeftLung.InitShape( ww+left_top_y,  ww+left_top_x,  ww+left_bottom_y,  ww+left_bottom_x,  0, 46, ShapeVL);
		RightLung.InitShape(ww+right_top_y, ww+right_top_x, ww+right_bottom_y, ww+right_bottom_x, 0, 45, ShapeVR);

		LeftLung.Search(imageT, ShapeVL);
		RightLung.Search(imageT, ShapeVR);
		img = iemCrop(img,img.rows()-ww-ww,img.cols()-ww-ww,ww,ww);
		for(ii=0;ii<ShapeVL.size();ii++) {
			ShapeVL[ii] -= ww;
			ShapeVR[ii] -= ww;
		}

//		time = clock();
//		printf("TIME2: %d\n",time);

		int factor = 1;
        IemPlane output(IemByte,factor*img.rows(), factor*img.cols());
        output = 0;
		DrawShape(output,ShapeVL,factor);
		DrawShape(output,ShapeVR,factor);
        if(chest.isAP()) output = iemReorient(output,iemFVER);
        output = iemReorient(output, rop);
        iemWrite(output,out1.c_str());        

		IemTImage<byte> imageC(image);
        LeftLung.ShowShape(imageC,ShapeVL,-1);
        RightLung.ShowShape(imageC,ShapeVR,-1);
        if(chest.isAP()) image = iemReorient(image,iemFVER);
        image = iemReorient(image, rop);
        iemWrite(image,out2.c_str());        
        return 0;
    } catch(IemError const &err) {
        cerr << "Caught an Iem exception: " << err << endl;
    } catch(...) {
        cerr << "Caught an unknown uncaught exception" << endl;
    }
   
//	time = clock();
//	printf("TIME2: %d\n",time);

	return 1;
}
