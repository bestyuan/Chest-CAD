#include "Chest_PA_AP.h"
#include "LungMaskFunctions.h"
#include "Iem/IemIO.h"

#ifdef WIN32
#include "minmax.h"       
#endif

using namespace CADX_SEG; 

Chest_PA_AP::Chest_PA_AP() :_type(IemShort) {
    _min_side = 100;
    _thH = -1;
    _thL = -1;
    _fraction1 = 0.25;
    _fraction2 = 0.30;
    _fraction3 = 0.30;
    _fraction4 = 0.03;
    _std_Factor = 2.5;
    _verbose = false;
    _subsample = true;
	_reorient = true;
    _ReorientationOp = iemNOP;
    _to_edge = 0.1;
    _AP = false;
	_spine_top_V =0;
	_spine_bottom_V =0;
	_boost_factor = 0.0;
}

IemPlane Chest_PA_AP::getMarkedImage() {
    if(_src.type()!=_type) _src = iemConvert(_src,_type);
    return _src;
}

void Chest_PA_AP::processing(IemPlane src) {
    FILE * pFile;
    int Orr=src.rows(), Occ=src.cols();
      // resize the image
    int sz = src.rows()/_min_side;
    if( src.rows()>src.cols() ) sz = src.cols()/_min_side;
    if(sz>1) { 
        if ( _subsample ) _src = iemSubsample(src,sz,sz);
        else              _src = iemAggregate(src,sz,sz);
    } else _src = src.copy();

    _type = _src.type();
    if(_type!=IemShort) _src = iemConvert(_src,IemShort);

    int rr=_src.rows(), cc=_src.cols();

    int th1 = _thL;
    int th2 = _thH;
    if(th1<0) th1 = (int) iemMinimum(_src);
    if(th2<0) th2 = (int) iemMaximum(_src);
/////
    IemLinearArray hist1(rr), hist2(cc);
    Fill_Hist(_src,hist1,hist2,th1,th2);

    int shift1,shift2;

	pFile = fopen ("myfile.txt","a");
    fprintf (pFile, "%s\n","is here");//////////
	fclose (pFile);
	//////////////////////////////////////////
    hist1 = Hist_Remove_Invalid(hist1,shift1);
/////////////////////////////////////////////////
	pFile = fopen ("myfile.txt","a");
    fprintf (pFile, "%s\n","is here");//////////
	fclose (pFile);

    hist2 = Hist_Remove_Invalid(hist2,shift2);

    if(_verbose) {
        iemWriteLinearArray("his1.txt",hist1);
        iemWriteLinearArray("his2.txt",hist2);
        printf("Shifts:  %d  %d\n",shift1,shift2);
    }

    _src = iemCrop(_src,hist1.length(),hist2.length(),shift1,shift2);
    int shift3 = rr-shift1-hist1.length();
    int shift4 = cc-shift2-hist2.length();

      // Get location of maximum value.
    int Lmax1,Lmax2;
    int err1 = Locate_Maximum(hist1,_fraction1,Lmax1);
    int err2 = Locate_Maximum(hist2,_fraction1,Lmax2);
    if (_verbose) printf("Step1: %5d %5d   %5d %5d\n",err1,err2,Lmax1,Lmax2);

    if( err1>0 && err2>0 ) {
        printf("BOTH Orientaions fail step 1:  %5d %5d   %5d %5d\n",err1,err2,Lmax1,Lmax2);
        _src = 0;
        return;
    }

    IemLinearArray hist;
    int st,np,flag,ii,jj;
    double pars[7], pars1[7], pars2[7];
    if (err1==0 && err2==0) {
        np = int(hist1.length()*_fraction2);
        st = Lmax1-np/2;
        int flag1 = Max_Peak(hist1, np, st, &pars1[0]);
        np = int(hist2.length()*_fraction2);
        st = Lmax2-np/2;
        int flag2 = Max_Peak(hist2, np, st, &pars2[0]);
        if (_verbose) {
            printf("Step2: %5d %9.1f  %5.1f %9.3f  %7.1f %7.1f  %5.0f %5.0f  %5d  ",
                   flag1,pars1[0],pars1[1],pars1[2],pars1[3],pars1[4],pars1[5],pars1[6],hist1.length());
            printf("%5d %9.1f  %5.1f %9.3f  %7.1f %7.1f  %5.0f %5.0f  %5d\n",
                   flag2,pars2[0],pars2[1],pars2[2],pars2[3],pars2[4],pars2[5],pars2[6],hist2.length());
        }
        if( flag1>0 && flag2>0 ) {
            printf("BOTH Orientaions fail step 2: %5d %5d %9.3f %9.3f %6.3f%6.3f\n",
                   flag1,flag2,pars1[2],pars2[2],pars1[4]/hist1.length(),pars2[4]/hist2.length());
            _src = 0;
            return;
        } else if( flag1==0 && flag2==0 ) {
            double s1 = log(pars1[2]/pars2[2]);
            double s2 = fabs(0.5-pars1[4]/hist1.length())-fabs(0.5-pars2[4]/hist2.length());
            double s = s1 - 8.0*s2;
            if (_verbose) printf("Step3:  %f  %f  %f\n",s,s1,s2);
            if (s<0) flag1 = 1;
            else     flag2 = 1;
        } 
        if(flag1==0	&& _reorient) {
            hist = hist1;
            flag = flag1;
            for (ii=0; ii<7; ii++) pars[ii] = pars1[ii];
            _src = iemReorient(_src,iemRCCW);
            _ReorientationOp = iemRCCW;
        } else {
            hist = hist2;
            flag = flag2;
            for (ii=0; ii<7; ii++) pars[ii] = pars2[ii];
        }
        

    } else {
        int Lmax;
        if(err1==0	&& _reorient) {
            hist = hist1;
            Lmax = Lmax1;
            _src = iemReorient(_src,iemRCCW);
            _ReorientationOp = iemRCCW;
        } else {
            hist = hist2;
            Lmax = Lmax2;
        }
        np = int(hist.length()*_fraction2);
        st = Lmax-np/2;
        flag = Max_Peak(hist, np, st, &pars[0]);
    }
    if(_verbose) printf("Step4: %5d  %9.1f  %5.1f %9.3f  %7.1f %7.1f  %5.0f %5.0f  %5d\n",
                        flag,pars[0],pars[1],pars[2],pars[3],pars[4],pars[5],pars[6],hist.length());

    if(_ReorientationOp==iemRCCW) {
	    int oo =Orr;
		Orr = Occ;
		Occ = oo;
        int tshift = shift1;
        shift1 = shift4;
        shift4 = shift3;
        shift3 = shift2;
        shift2 = tshift;
    }

    if(flag!=0) {
        printf("The ONLY orientation fails:  %5d\n",flag);
        _src = 0;
        return;        
    }
    rr=_src.rows();
    cc=_src.cols();

    np = int(hist.length()*_fraction3);
    int spine = int(pars[4]);
    int flag1 = Bottom_Search(hist, np, spine-np, -1, &pars1[0]);
    int flag2 = Bottom_Search(hist, np, spine,     1, &pars2[0]);
    if(_verbose) printf("Step5: %5d %5d  %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f %7.2f %7d\n",
                        flag1,flag2,pars1[4],pars[4],pars2[4],
                        pars1[0]-pars1[1]*pars1[1]/4/pars1[2],
                        pars[0]-pars[1]*pars[1]/4/pars[2],
                        pars2[0]-pars2[1]*pars2[1]/4/pars2[2],
                        ((pars1[4]+pars2[4])/2-pars[4])/hist.length()*100,hist.length());

    int locs0 = int(pars[4]);
    int locs1 = int(pars1[4]);
    int locs2 = int(pars2[4]);
    
    int dw = locs0-locs1;
	if(locs0-locs1>locs2-locs0) dw = locs2-locs0;
    int dd = dw/3;
    
    IemLinearArray Shist = Regional_Hist(_src,locs0-dd,  locs0+dd,  th1,th2);
    IemLinearArray Lhist = Regional_Hist(_src,locs0-dd*3,locs0-dd,  th1,th2);
    IemLinearArray Rhist = Regional_Hist(_src,locs0+dd,  locs0+dd*3,th1,th2);
 
    double significant = Diff_Mean_Striped(Shist,_std_Factor);
    if(significant>0 && _reorient) {
        locs0 = _src.cols()-locs0;
        locs1 = _src.cols()-locs1;
        locs2 = _src.cols()-locs2;
        _src = iemReorient(_src,iemR180);
        if( _ReorientationOp == iemRCCW) {
            _ReorientationOp = iemRCW;
        } else {
            _ReorientationOp = iemR180;
        }
		int ts = shift1; shift1=shift3; shift3=ts;
		ts = shift2; shift2=shift4; shift4=ts;
    }
    if(_verbose) printf("Step6:  %5d %5d %5d %5d %9.2f\n",locs1,locs0,locs2,dd,significant);

    double Hval = iemMaximum(_src);
    IemPlane img;
    IemImage tmp(_type,3,rr,cc);
    IemLinearArray Hline(cc);
    IemLinearArray Spine_Line(rr), Left_Lung_Line(rr),Right_Lung_Line(rr);
    IemLinearArray Spine_Value(rr), Left_Lung_Value(rr),Right_Lung_Value(rr);

    double sigma = _min_side*_fraction4+1;
    img = hiGaussianBlur(_src, sigma);
    if(_verbose)  iemWrite(img,"gau.tif");
    int locs[3];
    for (ii=0;ii<rr; ii++) {
        for (jj=0;jj<cc; jj++) Hline[jj] = img.get(ii,jj);
        peak_vally_locations(Hline,locs0,&locs[0],_to_edge);
        Spine_Line[ii]      = locs[0];
        Left_Lung_Line[ii]  = locs[1];
        Right_Lung_Line[ii] = locs[2];
            if(locs[0]>0) Spine_Value[ii] = img.get(ii,locs[0]);
            else          Spine_Value[ii] = -1;
            if(locs[1]>0) Left_Lung_Value[ii] = img.get(ii,locs[1]);
            else          Left_Lung_Value[ii] = -1;
            if(locs[2]>0) Right_Lung_Value[ii] = img.get(ii,locs[2]);
            else          Right_Lung_Value[ii] = -1;
    }
	
    if(_verbose) {
        tmp = 0;
        for (ii=0;ii<rr; ii++) {
            for (jj=0;jj<cc; jj++) {
                tmp.put(0,ii,jj,_src.get(ii,jj));
                tmp.put(1,ii,jj,_src.get(ii,jj));
                tmp.put(2,ii,jj,_src.get(ii,jj));
            }
            if(Spine_Line[ii]>=0)      {
                tmp.put(0,ii,int(Spine_Line[ii])     ,Hval);
                tmp.put(1,ii,int(Spine_Line[ii])     ,0);
                tmp.put(2,ii,int(Spine_Line[ii])     ,0);
            }
            if(Left_Lung_Line[ii]>=0)  {
                tmp.put(0,ii,int(Left_Lung_Line[ii]) ,0);
                tmp.put(1,ii,int(Left_Lung_Line[ii]) ,Hval);
                tmp.put(2,ii,int(Left_Lung_Line[ii]) ,0);
            }
            if(Right_Lung_Line[ii]>=0) {
                tmp.put(0,ii,int(Right_Lung_Line[ii]),0);
                tmp.put(1,ii,int(Right_Lung_Line[ii]),Hval);
                tmp.put(2,ii,int(Right_Lung_Line[ii]),0);
           }
        }
        iemWrite(iemConvert(tmp*(255/iemMaximum(tmp)),IemByte),"chest2.tif");
    }

	IemLinearArray Left_Lung_Copy = Left_Lung_Line.copy();
	IemLinearArray Right_Lung_Copy = Right_Lung_Line.copy();
    validate_line(img,Spine_Line,Left_Lung_Line);
    validate_line(img,Spine_Line,Right_Lung_Line);
	int L_size=0, R_size=0;
	for (ii=0;ii<rr; ii++) {
		if(Left_Lung_Line[ii]>0) L_size++;
		if(Right_Lung_Line[ii]>0) R_size++;
	}
	if(_verbose)printf("Step7: %5d  %5d\n",L_size,R_size);

//    for (ii=0; ii<rr; ii++) {
//        if(Left_Lung_Line[ii]>0 || Right_Lung_Line[ii]>0) break;
//        Spine_Line[ii] = -1;
//    }
//    for (ii=rr-1; ii>0; ii--) {
//        if(Left_Lung_Line[ii]>0 || Right_Lung_Line[ii]>0) break;
//        Spine_Line[ii] = -1;
//    }
    
    if(_verbose) {
//	if(fabs(L_size-R_size)/(L_size+R_size)>0.08) {
        tmp = 0;
        for (ii=0;ii<rr; ii++) {
            for (int jj=0;jj<cc; jj++) {
                tmp.put(0,ii,jj,_src.get(ii,jj));
                tmp.put(1,ii,jj,_src.get(ii,jj));
                tmp.put(2,ii,jj,_src.get(ii,jj));
            }
            if(Spine_Line[ii]>=0)      {
                tmp.put(0,ii,int(Spine_Line[ii])     ,Hval);
                tmp.put(1,ii,int(Spine_Line[ii])     ,0);
                tmp.put(2,ii,int(Spine_Line[ii])     ,0);
            }
            if(Left_Lung_Line[ii]>=0)  {
                tmp.put(0,ii,int(Left_Lung_Line[ii]) ,0);
                tmp.put(1,ii,int(Left_Lung_Line[ii]) ,Hval);
                tmp.put(2,ii,int(Left_Lung_Line[ii]) ,0);
            }
            if(Right_Lung_Line[ii]>=0) {
                tmp.put(0,ii,int(Right_Lung_Line[ii]),0);
                tmp.put(1,ii,int(Right_Lung_Line[ii]),Hval);
                tmp.put(2,ii,int(Right_Lung_Line[ii]),0);
           }
        }
        iemWrite(iemConvert(tmp*(255/iemMaximum(tmp)),IemByte),"chest3.tif");
    }

    IemLinearArray locs1_dif(rr),locs2_dif(rr);
    for (ii=0;ii<rr; ii++) {
        if(Spine_Line[ii]>=0) {
            if(Left_Lung_Line[ii]>=0)  locs1_dif[ii] = Spine_Line[ii]-Left_Lung_Line[ii];
            if(Right_Lung_Line[ii]>=0) locs2_dif[ii] = Right_Lung_Line[ii]-Spine_Line[ii];
        }
    }

    int win_l = int(L_size*0.15);
    int win_r = int(R_size*0.15);
    bool start_l=true, start_r=true;
    double tail_cutN = -0.04, tail_cutP=0.5;
    for (ii=rr-1;ii>max(win_l,win_r); ii--) {
        if (locs1_dif[ii]>0 && locs1_dif[ii-win_l]>0) {
             locs1_dif[ii] = (locs1_dif[ii]-locs1_dif[ii-win_l])/(locs1_dif[ii]+locs1_dif[ii-win_l]);
             if(locs1_dif[ii]<tail_cutN && start_l) Left_Lung_Line[ii] = -1;
             if(locs1_dif[ii]>tail_cutP && start_l) Left_Lung_Line[ii] = -1;
             if(locs1_dif[ii]>=tail_cutN && locs1_dif[ii]<=tail_cutP)start_l=false;
        } else locs1_dif[ii] = 0;
        if (locs2_dif[ii]>0 && locs2_dif[ii-win_r]>0) {
             locs2_dif[ii] = (locs2_dif[ii]-locs2_dif[ii-win_r])/(locs2_dif[ii]+locs2_dif[ii-win_r]);
             if(locs2_dif[ii]<tail_cutN && start_r) Right_Lung_Line[ii] = -1;
             if(locs2_dif[ii]>tail_cutP && start_r) Right_Lung_Line[ii] = -1;
             if(locs2_dif[ii]>=tail_cutN && locs2_dif[ii]<=tail_cutP)start_r=false;
        } else locs2_dif[ii] = 0;
    }

    IemLinearArray left_line  = get_lung_value(img,Spine_Line,Left_Lung_Line);
    IemLinearArray right_line = get_lung_value(img,Spine_Line,Right_Lung_Line);

    if(_verbose) {
        iemWriteLinearArray("locs1.txt",locs1_dif);
        iemWriteLinearArray("locs2.txt",locs2_dif);
        iemWriteLinearArray("Sline.txt",Spine_Line);
        iemWriteLinearArray("Lline.txt",Left_Lung_Line);
        iemWriteLinearArray("Rline.txt",Right_Lung_Line);
        iemWriteLinearArray("Shist.txt",Spine_Value);
        iemWriteLinearArray("Lhist.txt",Left_Lung_Value);
        iemWriteLinearArray("Rhist.txt",Right_Lung_Value);
        iemWriteLinearArray("left.txt",left_line);
        iemWriteLinearArray("right.txt",right_line);
    }

    int tot_point=0;
    IemLinearArray line_dif(rr),locs_dif(rr);
    double sum1=0, sum2=0;
	int s_top=0,s_bottom=0;
    for (ii=0; ii<rr; ii++) 
        if(left_line[ii]>0 && right_line[ii]>0) {
			if (tot_point==0) s_top=ii;
			s_bottom =ii;
            locs_dif[tot_point] = (Right_Lung_Line[ii]+Left_Lung_Line[ii]-Spine_Line[ii]-Spine_Line[ii])
                                 /(Right_Lung_Line[ii]-Left_Lung_Line[ii]);
            line_dif[tot_point++] = left_line[ii]-right_line[ii];
            sum1 += left_line[ii];
            sum2 += right_line[ii];
        }

	_spine_top_V = Spine_Value[s_top];
	_spine_bottom_V = Spine_Value[s_bottom];
	double spineU=0,spineD=0;
	int s_half = (s_top+s_bottom)/2;
    for (ii=s_top; ii<s_half; ii++)spineU += Spine_Line[ii];
	spineU /= s_half-s_top;
    for (ii=s_half; ii<s_bottom; ii++)spineD += Spine_Line[ii];
	spineD /= s_bottom-s_half;

    if(_verbose)printf("Area:  %f   %f    %f   %f\n",sum1,sum2,spineU,spineD);

    if(tot_point<4) {
        printf("Not enough points for two lungs:  %5d\n",tot_point);
        _src = 0;
        return;        
    }

    double slope1,offset1,average1,chi2;
//    linear_fit_P( tot_point/2,  tot_point, locs_dif, slope1,offset1,average1,chi2);
//    if(_verbose)printf("FitL:  %d  %d  %f   %f   %f   %f\n",tot_point/2,tot_point,slope1,offset1,average1,chi2);

      // remove bad points at the end
    double Slast=6;
    tot_point++;
    while (fabs(Slast)>2.5 && tot_point>3) {
        tot_point--;
        double mean=0, std=0;
        for (ii=0;ii<tot_point;ii++) mean += line_dif[ii];
        mean /= tot_point;
        for ( ii=0;ii<tot_point;ii++) std += (line_dif[ii]-mean)*(line_dif[ii]-mean);
        std = sqrt(std/(tot_point-1));
        Slast = (line_dif[tot_point-1]-mean)/std;
    }

    linear_fit_P( tot_point/2,  tot_point, line_dif, slope1,offset1,average1,chi2);
    if(_verbose)printf("Fit1:  %d  %d  %f   %f   %f   %f\n",tot_point/2,tot_point,slope1,offset1,average1,chi2);
    if(offset1>0 && _reorient) _AP = true;

      //
    double sumy=0,sumy2=0,sumx=0,sumx2=0;
    int st_fit = tot_point/2;
    for (ii=st_fit;ii<tot_point;ii++) sumy += line_dif[ii];
    sumy /= tot_point-st_fit;
    for (ii=st_fit;ii<tot_point;ii++) {
        int idx = (tot_point-1-ii);
        sumx  += idx;
        sumx2 += idx*idx;
        sumy2 += idx*(line_dif[ii]-sumy);
    }
    sumy2 /= tot_point-st_fit;
    sumx  /= tot_point-st_fit;
    sumx2 /= tot_point-st_fit;
    sumy2 /= sumx2-sumx*sumx;
    sumy += -sumy2*sumx;
    if(_verbose)printf("Fit2:  %f   %f   %f\n",sumy2,sumy,sumy2*sumx);

    np = tot_point/4;
    double sum_np=0;
    int start = tot_point/2-np/2;
    for (ii=start;ii<start+np;ii++) sum_np += line_dif[ii];
    int Max_st=start+np-1;
    double Max_sum=sum_np;
    int Min_st=start+np-1;
    double Min_sum=sum_np;
    for (ii=start+np; ii<tot_point; ii++) {
         sum_np += line_dif[ii]-line_dif[ii-np];
         if(sum_np > Max_sum) {
             Max_st=ii;
             Max_sum = sum_np;
         } else if(sum_np < Min_sum) {
             Min_st=ii;
             Min_sum = sum_np;
         }
    }
    Max_sum /= np;
    Min_sum /= np;

    double locs_max = (Max_st-double(np)/2)/tot_point;
    double locs_min = (Min_st-double(np)/2)/tot_point;
    if(_verbose)printf("Step8:   %7.3f   %7.3f   %7.3f  %9.2f  %9.2f  %5d\n",
                       locs_max,locs_min,Slast,Max_sum,Min_sum,tot_point);
    
//    if (abs(locs_max-locs_min)>0.2) {
//        if (locs_max>locs_min) _AP = true;
//    } else {
//        if (Max_sum+Min_sum>0) _AP = true;
//    }

	if (_boost_factor>0.0) {
		if(_AP) {
			boost_line(Right_Lung_Line,Right_Lung_Copy,_boost_factor);
		    for (ii=rr-1;ii>0; ii--) 
				if(Right_Lung_Line[ii]>0) break;
			boost_line(Left_Lung_Line,Left_Lung_Copy,_boost_factor,ii);
		} else {
			boost_line(Left_Lung_Line,Left_Lung_Copy,_boost_factor);
		    for (ii=rr-1;ii>0; ii--) 
				if(Left_Lung_Line[ii]>0) break;
			boost_line(Right_Lung_Line,Right_Lung_Copy,_boost_factor,ii);
		}
	}

   if(_verbose) {
        tmp = 0;
        for (ii=0;ii<rr; ii++) {
            for (int jj=0;jj<cc; jj++) {
                tmp.put(0,ii,jj,_src.get(ii,jj));
                tmp.put(1,ii,jj,_src.get(ii,jj));
                tmp.put(2,ii,jj,_src.get(ii,jj));
            }
            if(Spine_Line[ii]>=0)      {
                tmp.put(0,ii,int(Spine_Line[ii])     ,Hval);
                tmp.put(1,ii,int(Spine_Line[ii])     ,0);
                tmp.put(2,ii,int(Spine_Line[ii])     ,0);
            }
            if(Left_Lung_Line[ii]>=0)  {
                tmp.put(0,ii,int(Left_Lung_Line[ii]) ,0);
                tmp.put(1,ii,int(Left_Lung_Line[ii]) ,Hval);
                tmp.put(2,ii,int(Left_Lung_Line[ii]) ,0);
            }
            if(Right_Lung_Line[ii]>=0) {
                tmp.put(0,ii,int(Right_Lung_Line[ii]),0);
                tmp.put(1,ii,int(Right_Lung_Line[ii]),Hval);
                tmp.put(2,ii,int(Right_Lung_Line[ii]),0);
           }
        }
        iemWrite(iemConvert(tmp*(255/iemMaximum(tmp)),IemByte),"chest4.tif");
    }


	if (sz>1) {
        _spine_line = Line_Rescale(Spine_Line,sz,Orr,shift1,shift2);
        _left_line  = Line_Rescale(Left_Lung_Line,sz,Orr,shift1,shift2);
        _right_line = Line_Rescale(Right_Lung_Line,sz,Orr,shift1,shift2);

    } else {
        _spine_line = Spine_Line;
        _left_line = Left_Lung_Line;
        _right_line = Right_Lung_Line;
    }
    if(_verbose) printf("Rescale: %d  %d  %d  %d  %d  %d\n",Orr,Occ,shift1,shift2,shift3,shift4);

    if(_AP) {
        _src = iemReorient(_src,iemFVER);
        for (ii=0; ii<Orr; ii++) {
            if(_spine_line[ii]>0) _spine_line[ii] = Occ-1-_spine_line[ii];
            if(_left_line[ii]>0)  _left_line[ii]  = Occ-1-_left_line[ii];
            if(_right_line[ii]>0) _right_line[ii] = Occ-1-_right_line[ii];
        }
		IemLinearArray tmp = _left_line;
		_left_line = _right_line;
		_right_line = tmp;
    }

}
