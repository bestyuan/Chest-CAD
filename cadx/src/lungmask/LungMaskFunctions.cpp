#include "LungMaskFunctions.h"



void boost_line(IemLinearArray &lung, IemLinearArray &lungC, double boost_factor, int limit) {
	int len = lung.length();
	int ii,ii0,ii1;
	for (ii0=0; ii0<len; ii0++) 
		if(lung[ii0]>0) break;
	for (ii1=len-1; ii1>0; ii1--) 
		if(lung[ii1]>0) break;

		// upword
	int nt = int((ii1-ii0+1)*boost_factor), cnt=0;
    int Pii = ii0;
    int gap = -1;
	for ( ii=ii0-1; ii>=0; ii--) {
		cnt++;
		if(cnt>nt) break;
//		if(lungC[ii]>0) {
//			if (gap>max_gap) break;
//            if (fabs(lungC[ii]-lungC[Pii])>max_shift) break;
//			gap = 0;
//			Pii = ii;
//			lung[ii]=lungC[ii];
//		} else {
//			gap++;
//		}
		lung[ii] = lung[ii0];
	}

		// downword (factor*1.5)
	if(limit>0) {
		cnt=0;
		Pii = ii1;
		gap = -1;
		for ( ii=ii1+1; ii<limit; ii++) {
			cnt++;
			if(cnt>nt*1.5) break;
//			if(lungC[ii]>0) {
//				if (gap>max_gap) break;
//				printf("%d  %d  %f  %f\n",Pii,ii,lungC[Pii],lungC[ii]);
//				if (fabs(lungC[ii]-lungC[Pii])>max_shift) break;
//				gap = 0;
//				Pii = ii;
//				lung[ii]=lungC[ii];
//			} else {
//				gap++;
//			}
			lung[ii] = lung[ii1];
		}
	}
}

void Group_BY_Shift(IemPlane &img, IemLinearArray &spine, IemLinearArray &lung, int max_shift) {

    int len = spine.length(), ii=0, jj=0;
    IemLinearArray Pgroup(len);
    int group=1, Pii=-1;
    
    while (ii<len) {
        if(spine[ii]>0 && lung[ii]>0) {
            if(Pii>-1) 
                if(ii-Pii>1 || abs(lung[ii]-lung[Pii])>max_shift) group++;
            Pgroup[ii]=group;
            Pii=ii;
        } 
        ii++;
    }
//    iemWriteLinearArray("group.txt",Pgroup);
//    printf("Group: %d\n",group);

      // Lasgest group
    ii=1;
    int Max_group=0,Max_size=0;
    while (ii<=group) {
        while (Pgroup[jj]!=ii)jj++;
        int cnt=0;
        while (Pgroup[jj]==ii) {
            jj++;
            cnt++;
        }
//        printf("G: %3d  %3d  %3d\n",ii,jj-cnt,cnt);
        if(cnt>Max_size) {
            Max_group = ii;
            Max_size = cnt;
        }
        ii++;
    }

      // Second Largest group
    int Max_group2=0,Max_size2=0;
    jj=0;
    if(group>1) {
        ii=1;
        while (ii<=group) {
            while (Pgroup[jj]!=ii)jj++;
            int cnt=0;
            while (Pgroup[jj]==ii) {
                jj++;
                cnt++;
            }
            if(cnt>Max_size2 && ii!=Max_group) {
                Max_group2 = ii;
                Max_size2 = cnt;
            }
            ii++;
        }
//        printf("G2: %3d  %3d  %f\n",Max_size, Max_size2,float(Max_size2)/len );
    }

    int Region_from=len, Region_to=0;
    for(ii=0; ii<len; ii++) {
        if (Pgroup[ii]==Max_group) {
            if(ii<Region_from) Region_from=ii;
            if(ii>Region_to)   Region_to  =ii;
        } 
    }

    if ( float(Max_size2)/Max_size>0.35) { // Combination may needed.
        double Pmean1=0, Pcol1=0;
        int Cnt1=0, Cnt2=0;
        double Pmean2=0, Pcol2=0;
        for (ii=0; ii<len; ii++) {
            if (Pgroup[ii]==Max_group) {
                Pcol1 += lung[ii];
                Pmean1 += img.get(ii,int(lung[ii]));
                Cnt1 ++;
            }
            else if (Pgroup[ii]==Max_group2) {
                Pcol2 += lung[ii];
                Pmean2 += img.get(ii,int(lung[ii]));
                Cnt2 ++;
            }
        }
        Pcol1 /= Cnt1;
        Pmean1 /= Cnt1;
        Pcol2 /= Cnt2;
        Pmean2 /= Cnt2;
//        printf("G3: %3d  %3d  %7.1f %7.1f %7.1f %7.1f %7.3f %7.3f\n",
//			Cnt1, Cnt2, Pcol1, Pcol2, Pmean1, Pmean2, fabs(Pcol2/img.cols()-0.5), fabs(Pmean1-Pmean2)/(Pmean1+Pmean2));
        if ( fabs(Pmean1-Pmean2)/(Pmean1+Pmean2)<.4 ) { // pixel values are close
            if ( fabs(Pcol2/img.cols()-0.5)*3<1 ) { // not too close to boundarys
//                printf("G3: %3d  %3d  %7.1f %7.1f %7.1f %7.1f %7.3f\n",
//                       Cnt1, Cnt2, Pcol1, Pcol2, Pmean1, Pmean2, fabs(Pcol2/img.cols()-0.5));
                for(ii=0; ii<len; ii++) {
                    if (Pgroup[ii]==Max_group2) {
                        if(ii<Region_from) Region_from=ii;
                        if(ii>Region_to)   Region_to  =ii;
                    } 
                }
            }
        }
    }

//    printf("G4: %d   %d\n",Region_from,Region_to);
    for(ii=0; ii<Region_from; ii++)   lung[ii] = -1;
    for(ii=Region_to+1; ii<len; ii++) lung[ii] = -1;

}

void validate_line(IemPlane &img, IemLinearArray &spine, IemLinearArray &lung) {
    int max_shift = 3;
	Group_BY_Shift(img,spine,lung,max_shift);

    int len = spine.length(), ii=0, jj=0;

      // The point that goes up is the shoulder
    int Pii=-1;
	int Pnext=0,Pshoulder;
	double Dprev=0, Dp;	
	while (Pnext<len/3) {
		while (ii<len) {
			if(spine[ii]>0 && lung[ii]>0) {
				Dp = img.get(ii,int(spine[ii]))-img.get(ii,int(lung[ii]));
				if(Pii>0 && Dp-Dprev>0 ) break;  // Stop when it goes up.
				Pii=ii;
				Dprev = Dp;
			}
			ii++;
		}
		Pshoulder=Pii;
		while(ii<len) {
			if(spine[ii]>0 && lung[ii]>0) {
				Dp = img.get(ii,int(spine[ii]))-img.get(ii,int(lung[ii]));
				if(Dp<Dprev) break;
			}
			ii++;
		}
		Pnext=ii;
    }
//    printf("G5: %d  %d   %7.0f\n",Pii,ii,Dprev);
    for(ii=0; ii<Pshoulder; ii++)   lung[ii] = -1;

    ii=len-1;
    while (ii>0) {
        if(spine[ii]>0 && lung[ii]>0) {
            Dp = img.get(ii,int(spine[ii]))-img.get(ii,int(lung[ii]));
//			printf("G6: %5d  %f\n",ii,Dp);
            if( Dp>Dprev ) break;
        }
        ii--;
    }

//	ii++;
    for(; ii<len; ii++)   lung[ii] = -1;

}

void validate_lineO(IemPlane &img, IemLinearArray &spine,IemLinearArray &lung) {

    int MAX_cnt=10, cnt=0;
    IemLinearArray Pstart(MAX_cnt), Pend(MAX_cnt), Porder(MAX_cnt);

    double ratio = 0.1;
    int max_gap = 4;
    int max_shift = 4, max_shift1=15;
    int width = img.cols();
      // find the first dip as the shoulder
    int len = spine.length();
    int ii=0;
    int ii_st=0;
    int Max_ii=0;
    int Max_ii_st=0;
    while (ii<len) {
        int Pii = -2;
        double Vprevious=100000;
        for (; ii<len; ii++) {
            if(spine[ii]>0 && lung[ii]>0) {
                double dv = img.get(ii,spine[ii])-img.get(ii,lung[ii]);
                if (dv>Vprevious || abs(ii-Pii)<2) break;
                Vprevious = dv;
                Pii = ii;
            }
        }
        if(ii==len) break;
        if(abs(ii-Pii)<2) ii=Pii;
        printf("st %d\n",ii);
        ii_st = ii;
        double Vmax = Vprevious;
        Pii = -1;
        int gap = -1;
        for (; ii<len; ii++) {
            if(spine[ii]>0 && lung[ii]>0) {
                double dv = img.get(ii,spine[ii])-img.get(ii,lung[ii]);
                if (dv>Vmax) Vmax=dv;
                else if(dv-Vprevious<(Vmax-Vprevious)*ratio) break;
                if (gap>max_gap) break;
                if (gap>=0 && fabs(lung[ii]-lung[Pii])>max_shift && fabs(lung[ii]/width-0.5)>0.3 ) break;
                if (gap>=0 && fabs(lung[ii]-lung[Pii])>max_shift && fabs(spine[ii]-spine[Pii])>max_shift ) break;
                if (gap>=0 && fabs(lung[ii]-lung[Pii])>max_shift1 ) break;
                Pii = ii;
                gap = 0;
            } else if (gap>=0) gap++;
        }

        if (ii_st!=Pii) {
            Pstart[cnt] = ii_st;
            Pend[cnt] = Pii;
            cnt++;
        }
        if(cnt==MAX_cnt) break;
    }

//    printf("Number of pieces = %d\n",cnt);
    for (ii=0; ii<cnt-1; ii++) 
        for (int jj=ii+1; jj<cnt; jj++) {
            if (Pend[ii]-Pstart[ii]<Pend[jj]-Pstart[jj]) Porder[ii] ++;
            else  Porder[jj] ++;
        }
    
    for (ii=0; ii<cnt; ii++) {
        printf("Section: %d  %7.0f  %7.0f  %9.4f  %5.0f\n",ii,Pstart[ii],Pend[ii],(Pend[ii]-Pstart[ii])/len,Porder[ii]);
        if (Pend[ii]-Pstart[ii]>Max_ii-Max_ii_st) {
            Max_ii = int(Pend[ii]);
            Max_ii_st = int(Pstart[ii]);
        }
    }
    
    for (ii=0; ii<Max_ii_st; ii++) lung[ii] = -1;
    for (ii=Max_ii+1; ii<len; ii++) lung[ii] = -1;


}

void peak_vally_locations( IemLinearArray &a, int locs0, int *locs, double to_edge){
    int len = a.length();
    IemLinearArray na(len), idx0(len), idx1(len);
    int cnt=0;
    na[cnt] = a[0];
    idx0[cnt] = 0;
	int ii;
    for (ii=1;ii<len; ii++) {
        if(a[ii]!=a[ii-1]) {
            idx1[cnt] = ii-1;
            cnt++;
            na[cnt] = a[ii];
            idx0[cnt] = ii;
        }
    }
    idx1[cnt] = len-1;

    int ll0=0;
    for (ii=0;ii<cnt; ii++) {
        if (locs0>=idx0[ii] && locs0<=idx1[ii]) {
            ll0=ii;
            break;
        }
    }
    
    int step=0;
    if ( na[ll0-1]>na[ll0] || na[ll0+1]>na[ll0] ) {
        if (na[ll0-1]>na[ll0+1]) step = -1;
        else                     step = 1;
        while (ll0>0 && ll0<cnt-1 && na[ll0+step]>na[ll0] ) ll0+=step;
        
    }
    locs[0] = int(idx0[ll0]+idx1[ll0])/2; // Maximum location
//    printf("%d  %f  %f  %d\n",ll0,idx0[ll0],idx1[ll0],locs[0]);

    int tt=ll0;
    while (tt>0 && na[tt-1]<na[tt] ) tt-=1;
    if(tt==0) locs[1] = -1;
    else      locs[1] = int(idx0[tt]+idx1[tt])/2; // Minimum location on left
    tt=ll0;
    while (tt<cnt-1 && na[tt+1]<na[tt] ) tt+=1;
    if(tt==cnt-1) locs[2] = -1;
    else          locs[2] = int(idx0[tt]+idx1[tt])/2; // Minimum location on right

    if(    locs[1]<int(len*to_edge))locs[1]=-1;
    if(len-locs[2]<int(len*to_edge))locs[2]=-1;

}

IemPlane KESPR2IEM(const char *filename) {
    
    FILE *fp = fopen(filename,"rb");
    if (!fp) IEM_THROW(IemErrorType::error,"Error opening KESPR file\n");
    
    char *pb = (char *) malloc(1);
    char *str = (char *) malloc(100);
    char *ps = str;
    int npos=0;
    
    bool wtr = false;
    int numRows=0, numCols=0;

    for (int i=0;i<2048;i++){
        if (fread(pb,1,1,fp)) {
            if ( (*pb >= 0x20) && (*pb <= 0x7e) ) {
                strncpy(ps++,pb,1);
                if(!wtr) {
                    npos=i;
                    wtr = true;
                } 
            } else {
                if(wtr) {
                    *ps=0x0;
//                     printf("%5d     ##%s##\n",npos,str);
                    if(npos==608) numRows = atoi(str);
                    if(npos==624) numCols = atoi(str);
                    
                    ps=str;
                    wtr = false;
                }
            }
        } else  IEM_THROW(IemErrorType::error,"Error reading KESPR file herder\n");
    } 

    free(pb);
    free(str);

    int len = numRows*numCols*sizeof(short);
    void *p_image = malloc(len);
    
    if (fread(p_image,len,1,fp)) {
        IemPlane tt(IemShort,numRows,numCols,p_image);
        return tt;
    }
    IEM_THROW(IemErrorType::error,"Error reading KESPR image\n");
}

void Fill_Hist(IemPlane &img, IemLinearArray &hist1, IemLinearArray &hist2, int th1, int th2){

    double R_th = 0.4;
    IemTPlane<short> imgT(img);
    short *pd=imgT[0];
    int rr=img.rows(), cc=img.cols(), ii;
    IemLinearArray n1(rr), n2(cc);

    for (ii=0; ii<rr; ii++)
        for (int jj=0;jj<cc;jj++) { 
            int data = *pd++;
            if ( data>th1 && data<th2 ) {
                hist1[ii] += data;
                hist2[jj] += data;
                n1[ii] ++;
                n2[jj] ++;
            }
        }
    for (ii=0; ii<rr; ii++) {
        if(n1[ii]>int(R_th*cc)) hist1[ii] /= n1[ii];
        else                    hist1[ii] = -1;
    }
    for (ii=0; ii<cc; ii++) {
        if(n2[ii]>int(R_th*rr)) hist2[ii] /= n2[ii];
        else                    hist2[ii] = -1;
    }

}

IemLinearArray Hist_Remove_Invalid(IemLinearArray val, int &shift) {
    int zero_front=0, zero_back=0;
    int rr = val.length(), ii;
    for (ii=0; ii<rr; ii++) {
        if (val[ii]>0) break;
        zero_front++;
    }
    for (ii=0; ii<rr; ii++) {
        if (val[rr-1-ii]>0) break;
        zero_back++;
    }

    int length = rr-zero_front-zero_back;
    IemLinearArray hist(length);
    for (ii=0; ii<length; ii++) hist[ii] = val[ii+zero_front];
    shift = zero_front;
    return hist;
}

int Locate_Maximum(IemLinearArray &hist, double f_edge, int &locs) {

    int length = hist.length();
    int W_edge = int(length*f_edge);

    locs = W_edge;
    double M_value = hist[locs];
    for (int ii=W_edge+1; ii<length-W_edge; ii++) 
        if (hist[ii]>M_value) {
            locs = ii;
            M_value = hist[ii];
        }
    
    if ( locs==W_edge ) return 1;
    if ( locs==length-1-W_edge ) return 1;
    return 0;
}

int Max_Peak(IemLinearArray &data, int win, int st, double *pars) {
    if (win<4) return 1;
    int max_nt=10,mt=0;
    double dst = 4;
    st -= int(dst/2);
    int flag = 0;
    while (fabs(dst/2)>1 && mt<max_nt && flag==0) {
        st += int(dst/2);
        int ft = p2fit(win, data, st, &pars[0]);
        if ( ft>0 ) flag = 10+ft;
        else if ( pars[2]>=0 ) flag = 2;
        else {
            dst = -pars[1]/pars[2]/2;
            mt++;
            if (fabs(dst)>win/2)  flag = 3;
        }
    }
    if(mt==max_nt) flag = 4;
    pars[3] = sqrt(pars[3]/(win-3));
    pars[4] = st+float(win)/2+dst;
    pars[5] = st;
    pars[6] = st+win;
    
    return flag;
}

int p2fit(int np, IemLinearArray& p, int start, double *par) {
    if (start<0) return 1;
    if (start+np>int(p.length())) return 2;
    if (np<3) return 3; // at least 3 points
    int i;
    double sum = 0;
    for (i=0; i<np; i++) sum += p[i+start];
    sum /= np;
    float mid = float(np-1)/2;
    double s2=0,s4=0,t1=0,t2=0;
    double x;
    for (i=0; i<np; i++) {
        float f = i-mid;
        x  = f*f;                 s2 += x;
        x *= f*f;                 s4 += x;
        x  = f*(p[i+start]-sum);  t1 += x;
        x *= f;                   t2 += x;
    }
    s2 /= np;
    s4 /= np;
    t1 /= np;
    t2 /= np;
    par[2] = t2/(s4-s2*s2);
    par[1] = t1/s2;
    par[0] = -par[2]*s2+sum;
    par[3] = 0.0;
    for (i=0; i<np; i++) {
        float f = i-mid;
        double delta = (p[i+start]-par[0]-par[1]*f-par[2]*f*f);
        par[3] += delta*delta;
    }
    return 0;
    
}

int Bottom_Search(IemLinearArray &hist, int win, int start, int step, double *pars) {
    double Mini_dx=win;
    int Mini_st=-1;

    for (int ii=0;ii<6;ii++)pars[ii]=1;

    int st = start+step;
    int flag = 0;
    while ( flag==0 ) {
        flag = p2fit(win, hist, st, &pars[0]);
        if(flag>0)flag += 10;
        if( pars[2]>0 && flag==0) {
            double dx = -pars[1]/pars[2]/2;
//            printf("%5d %5d  %5d %9.1f  %5.1f %9.3f  %7.1f %7.1f\n",
//                   flag,st,st+win,pars[0],pars[1],pars[2],dx,Mini_dx);
            if(fabs(dx)<1) {
                pars[3] = sqrt(pars[3]/(win-3));
                pars[4] = st+float(win)/2+dx;
                pars[5] = st;
                pars[6] = st+win;
                return 0;
            }
            if(fabs(dx)<Mini_dx) {
                Mini_dx = fabs(dx);
                Mini_st = st;
            }
            if(fabs(dx)>1.5*Mini_dx && Mini_st!=-1) flag = 3;
        }
        st += step;
    }
    
    if (Mini_st==-1) {
        pars[4] = 0;
        if(step>0) pars[4] = hist.length()-1;;
        return 10;
    }
    st = Mini_st;
	p2fit(win, hist, st, &pars[0]);
    pars[3] = sqrt(pars[3]/(win-3));
    pars[4] = st+float(win)/2-pars[1]/pars[2]/2;
    pars[5] = st;
    pars[6] = st+win;
    if(flag==0)flag=2;
    return flag;
}

IemLinearArray Regional_Hist(IemPlane img, int x1, int x2, int th1, int th2){
    IemTPlane<short> imgT(img);
    int rr=img.rows();
    IemLinearArray dst(rr),cnt(rr);

    x2++;
    for (int ii=0; ii<rr; ii++) {
        short *pd=imgT[ii]+x1;
        for (int jj=x1;jj<x2;jj++) {
            int val = *pd++;
            if( val>th1 && val<th2) {
                dst[ii] += val;
                cnt[ii] ++;
            }
        }
    }

	int jj;
    for (jj=0;jj<rr;jj++) 
        if(cnt[jj]>1) dst[jj] /= cnt[jj];
        else          dst[jj] = -1;

    cnt = dst.copy();
    for (jj=0;jj<rr;jj++) {
        if(dst[jj]==-1) {
            int mini_d=rr;
            double mini_v=0;
            for (int ii=0;ii<rr;ii++) {
                if(cnt[ii]!=-1) 
                    if(abs(ii-jj)<mini_d) {
                        mini_d=abs(ii-jj);
                        mini_v=cnt[ii];
                    }
            }
            dst[jj]=mini_v;
        }
    }

    return dst;
}

double Diff_Mean_Striped(IemLinearArray hist,double Sfactor) {
    double fraction= 0.25;
    int len = hist.length()-1;
    IemLinearArray dif(len), flag(len);
    for(int ii=0; ii<len; ii++) dif[ii] = hist[ii]-hist[ii+1];

    double Vmax=Sfactor+1,mean=0,std=0;
    int cnt=0;

    while(Vmax>Sfactor) {
        double sum=0, sum2=0;
        cnt = 0;
		int ii;
        for(ii=0; ii<len; ii++) {
            if(flag[ii]==0) {
                sum  += dif[ii];
                sum2 += dif[ii]*dif[ii];
                cnt ++;
            } 
        }
    
        mean = sum/cnt;
        std = sqrt(sum2/cnt-mean*mean);
        int Lmax=-1;
        for(ii=0; ii<len; ii++) {
            if(flag[ii]==0) {
                double val = fabs(dif[ii]-mean)/std;
                if(Lmax==-1 || val>Vmax) {
                    Lmax=ii;
                    Vmax=val;
                }
            }
        }
//        printf("Strip: %9.2f  %5d  %5d  %9.3f  %9.3f  %9.3f\n",fabs(double(Lmax)/len-0.5),Lmax,cnt,mean,Vmax,std);
        if (Vmax>Sfactor) {
            if( fabs(double(Lmax)/len-0.5) > fraction ) {
                if(Lmax>len/2) for(int ii=Lmax; ii<len;  ii++) flag[ii] = 1;
                else           for(int ii=0;    ii<=Lmax; ii++) flag[ii] = 1;
            } else Vmax = -1;
        }
    }

    return mean/std*sqrt(cnt);
      
}

IemPlane find_ridegs(IemPlane &img){
    int rr=img.rows(), cc=img.cols();
    IemPlane ridge(img.type(),rr,cc);
    ridge = 0;

    for (int ii=0; ii<rr; ii++) 
        for(int jj=1; jj<cc-1; jj++) {
            double val1 = img.get(ii,jj);
            double val0 = img.get(ii,jj-1);
            double val2 = img.get(ii,jj+1);
            double xx = val2+val0-val1-val1;
            if      ( val0>val1 && val2>val1 ) ridge.put(ii,jj,xx);
            else if ( val0<val1 && val2<val1 ) ridge.put(ii,jj,xx);
        }

    return ridge;

}

IemLinearArray LAverage(IemLinearArray a, int win) {
    int len = a.length();
    IemLinearArray tmp(len);
    int start = win/2;
    for (int ii=0; ii<len; ii++) {
        int nt=0;
        for (int jj=ii-start; jj<ii+win-start; jj++) {
            if (jj>=0 && jj<len) {
                nt++;
                tmp[ii] += a[jj];
            }
        }
        tmp[ii] /= nt;
    }
    return tmp;
}

IemLinearArray Line_Rescale(IemLinearArray &a, int scale , int new_size, int shift1, int shift2){
    IemLinearArray out(new_size);
    out = -1;
    int len = a.length();
    double Vprevious,Vnext;
    int half_scale=scale/2;

    for(int ii=0; ii<len; ii++) 
        if (a[ii]>0) {
            if(ii>0 && a[ii-1]>0) Vprevious = a[ii-1];
            else                  Vprevious = a[ii];
            if(ii<len-1 && a[ii+1]>0) Vnext = a[ii+1];
            else                     Vnext = a[ii];
			int id,idx;
            for (id=0; id<half_scale; id++) {
                idx = (shift1+ii)*scale+half_scale-id-1;
                out[idx] = (shift2+a[ii])*scale+(Vprevious-a[ii])*id+half_scale;
            }
            for (id=half_scale; id<scale; id++) {
                idx = (shift1+ii)*scale+id;
                out[idx] = (shift2+a[ii])*scale+(Vnext-a[ii])*(id-half_scale)+half_scale;
            }
        }

    return out;

}

//using namespace std;

IemPlane 
hiGaussianBlur(const IemPlane &src, float sigma, float sigfactor)
{
  IemPlane smooth;
  PLANE_DISPATCH(src, srcT, smooth = hiGaussianBlur(srcT, sigma, sigfactor));
  return smooth;
}


IemImage 
hiGaussianBlur(const IemImage &src, float sigma, float sigfactor)
{
  IemImage smooth(src.type(), src.chans(), src.rows(), src.cols());
  for (int ii = 0; ii < src.chans(); ii++)
    smooth[ii] = hiGaussianBlur(src[ii], sigma, sigfactor);
  return smooth;
}

IemLinearArray get_lung_value(IemPlane &img,IemLinearArray &Spine_Line,IemLinearArray &Lung_Line){

    double ratio=0.4;
    int rr=img.rows();

    IemLinearArray line(rr);
    
    for(int ii=0; ii<rr;ii++) {
        if(Spine_Line[ii]>0 && Lung_Line[ii]>0) {
            int idx = int(Spine_Line[ii]*ratio+Lung_Line[ii]*(1-ratio));
            line[ii] = img.get(ii,idx);
        }
    }
    return line;
}

void linear_fit_P(int st_fit, int tot_point, IemLinearArray &line_dif, 
                  double &slope, double &offset, double &average, double &chi2) {
    double sumy=0,sumy2=0,sumx=0,sumx2=0;
	int ii;
    for ( ii=st_fit;ii<tot_point;ii++) sumy += line_dif[ii];
    sumy /= tot_point-st_fit;
    for ( ii=st_fit;ii<tot_point;ii++) {
        int idx = (tot_point-1-ii);
        sumx  += idx;
        sumx2 += idx*idx;
        sumy2 += idx*(line_dif[ii]-sumy);
    }
    sumy2 /= tot_point-st_fit;
    sumx  /= tot_point-st_fit;
    sumx2 /= tot_point-st_fit;
    sumy2 /= sumx2-sumx*sumx;
    average = sumy;
    sumy  -= sumy2*sumx;
    slope = sumy2;
    offset = sumy;

    chi2=0;
    for ( ii=st_fit;ii<tot_point;ii++) {
        int idx = (tot_point-1-ii);
        double dx = (line_dif[ii]-idx*slope-offset);
        chi2 += dx*dx;
    }
    chi2 = sqrt(chi2/(tot_point-st_fit));
        
}


