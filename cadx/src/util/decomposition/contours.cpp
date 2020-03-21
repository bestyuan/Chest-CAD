// contours.cc
//
// Perform shape decomposition using contour analysis.
//
//
// Notes:
// - it would be nice to unify the data structures used in this code.
//   Doing so would save memory + execution time + code, though perhaps
//   only slightly. 
// - it would be much cleaner to implement a circular vector data structure,
//   and then use that throughout. We spend a lot of code right now checking
//   bounds and wrapping indices in order to simulate a circular data structure.
//   But the current implementation is probably just as efficient.


#include "Iem/IemImageIO.h"
#include "Iem/Iem.h"
#include "contourclass.h"
#include "LabelPlane.h"
#include <math.h>
#include "contour_pt.h"
#include "ConSeg.h"
#include "Pt.h"

const int    small_area_thresh=150;
const double initial_dist_thresh_ratio=0.5;

int
writeRawPlane(char *baseName, IemTPlane<short>& p)
{
    int  width = p.cols();
    int  height = p.rows();
    char fileName[1024];

    short *pData = p.getBasePtr();
    sprintf(fileName, "E:\\Temp\\%s-%dX%d.raw", baseName, width, height);
    FILE *f = fopen(fileName, "wb");
    if (NULL != f)
    {
        fwrite((void *)pData, sizeof(short), width * height, f);
        fclose(f);
        return 0;
    }
    else
    {
        return 1;
    }
}

//
// find contour points within labeled "segseg", creating "contourbuf"
static void
find_contour_points(const IemTPlane<short> &segseg,
  IemTPlane<short> &contourbuf)
{
    int i;
    int j;

    short rows = segseg.rows();
    short cols = segseg.cols();

    // to avoid border tests in the loop below, we copy the top and bottom
    // rows as well as the left and right columns to contourbuf

    // top and bottom rows
    int rMinus1 = rows - 1;
    for (i = 0; i < cols; ++i)
    {
        contourbuf[0][i] = segseg[0][i];
        contourbuf[rMinus1][i] = segseg[rMinus1][i];
    }

    // left and right colums
    int cMinus1 = cols - 1;
    for (j = 0; j < rows; ++j)
    {
        contourbuf[j][0] = segseg[j][0];
        contourbuf[j][cMinus1] = segseg[j][cMinus1];
    }

    // note: outer loop starts at 1 and ends at rows-1
    // note: inner loop starts at 1 and ends at cols-1
    for (i = 1; i < rMinus1; i++)
    {
        for (j = 1; j < cMinus1; j++)
        {
            short codeval = segseg[i][j];
            
            if (0 == codeval) 
            {
                contourbuf[i][j] = 0;
                continue;
            }
            
            if (segseg[i-1][j] != codeval ||
              segseg[i-1][j-1] != codeval ||
              segseg[i-1][j+1] != codeval ||
              segseg[i][j-1] != codeval ||
              segseg[i][j+1] != codeval ||
              segseg[i+1][j] != codeval ||
              segseg[i+1][j-1] != codeval ||
              segseg[i+1][j+1] != codeval)
            {
                // one of my 8-connected neighbors isn't the same as me
                // it must be a boundary, copy it into contourbuf
                contourbuf[i][j] = codeval;
            }
            else
            {
                contourbuf[i][j] = 0;
            }
        }
    }
}

int sign(double d)
{
    if (d < 0)
        return -1;
    else
        return 1;
}

template<class T>
T min(T a, T b)
{
	if (a < b)
		return a;
	else
		return b;
}

// startrow & startcol are an estimated location of a boundary pixel
// a boundary must occur in row startrow to the right of startcol
//
// the boundary chain code returned in contour is (supposed to be)
// guaranteed to be in a clockwise direction.

// we fill in the "contour" vector
// contourPlane is a plane containing outlines of all the contours,
//     the values are region labels
// contourPlaneFilled is a plane containing contours as above, though the
//     interiors are filled
// labelValue is really an index into the labels.
//    And, it's also the value of the label.
// startrow, startcol specify the origin of the polar coordinate system
//           currently, these are always (0, 0)
//

// templated? why? (input T is always short here!)

template <class T>
bool FollowContour(contour_vector &contour,
  IemTPlane<T> contourPlane, IemTPlane<T> contourPlaneFilled,
  T labelValue, int originRow, int originCol)
{
    int steps=0;
    int lastRow;
    int lastCol;
    int startRow = 0;
    int startCol = 0;
    int numPixels = contourPlane.rows() * contourPlane.cols();

    //
    // Find the leftmost pixel that has the label we're interested in
    // on the topmost scanline of the region
    //
    int nRows = contourPlane.rows();
    int nCols = contourPlane.cols();
    int row;
    int col;
    bool found = false;

#if 0
static int contourCount = 1;
if (contourCount <= 15)
{
char fileName[1024];
sprintf(fileName, "FollowContour%d", contourCount++);
writeRawPlane(fileName, contourPlane);
}
#endif
    
    for (row = 0; row < nRows; row++)
    {
        for (col = 0; col < nCols; col++)
        {
            if (contourPlane[row][col] == labelValue)
            {
                found = true;
                goto FOUND;
            }
        }
    }

FOUND:
    if (!found)
        return false;

    startCol = col;
    startRow = row + 1;   // not startRow = row, per Dave C. 

    int dir = 0; // up
 
    contourclass<T> con(contourPlane,
      contourPlaneFilled, labelValue, dir, startRow, startCol);
 
    bool done = false;
    bool first = true;
 
    lastRow = startRow;
    lastCol = startCol;
 
    while (!done)
    {
        // [lastRow, lastCol] is the point we looked at in the last
        // iteration. The first time through, it matches [startRow, startCol]
        // Subrtracting originRow, originCol allows for an offset
        // into a bigger image, though we're always [0,0] currently.

        int x = lastCol - originCol;
        int y = lastRow - originRow;
        
        // too high. go back to the bottom row
        if (lastRow >= nRows)
            y = nRows - 1 - originRow;

        // too wide. go back to the last pixel in the row
        if (lastCol >= nCols)
            x = nCols - 1 - originCol;

        // zero pixels are not part of the contour
        // if we find a non-zero, it's part of the contour, add it
        if (contourPlane[y][x] > 0)
            contour.push_back(contour_pt(x, y));
        
        lastRow = con.GetRow();
        lastCol = con.GetCol();

        //
        // The block4 array maintains a notion of direction whether or not
        // the pixel at the current location matches the label
        //
        // block4[0] ==> "above left" matches
        // block4[1] ==> "above" matches
        // block4[2] ==> "left" matches
        // block4[3] ==> "current" matches
        //
        char *block4 = con.Get4Block();

        if (block4[0] && block4[1] && !block4[2] && block4[3] ||
           !block4[0] && !block4[1] && block4[2] && !block4[3])
        {
            con.TurnLeft();
        }
        else if (!block4[0] && !block4[1] && !block4[2] && block4[3] ||
                block4[0] && block4[1] && block4[2] && !block4[3])
        {
            con.TurnRight();
        }
        
        //
        // If we're not the first time through, we check to see if
        // we've moved back to the origin. If we have, then we're done.
        //

        // 4/22/2002, crandall.
        // I think this may prevent the infinite loops.
        // The problem seem to happen when a region includes pixel 0,0. 
        if (!first && con.GetRow() == startRow && con.GetCol() == startCol &&
          (con.GetDir() == 0 || (con.GetRow() == 0 && con.GetCol() == 0)))
            done=true;
        
        first = false;
        
        con.GoStraight();
        steps++;

        if (steps > numPixels)
        {
            return false;
        }
    }

    // make sure we follow the contour in a clockwise fashion
    if (con.left_count > con.right_count)
        reverse(contour.begin(), contour.end());

    return true;
}

double ComputeCritLevel(contour_vector &contour, int ind1, int ind0, int ind2, int shapemethod_t)
{
    double x1=contour[ind1].get_x();
    double y1=contour[ind1].get_y();
    double x2=contour[ind2].get_x();
    double y2=contour[ind2].get_y();
    double x0=contour[ind0].get_x();
    double y0=contour[ind0].get_y();
    
    double dii=1;
    if(shapemethod_t==0)
    {
        double x12d=(x1-x2),y12d=(y1-y2);
        
        dii=sqrt(x12d*x12d+y12d*y12d);
    }
    else if(shapemethod_t==1)
    {
        double x10d=(x1-x0),y10d=(y1-y0),x20d=(x2-x0),y20d=(y2-y0);
        dii=sqrt(x10d*x10d+y10d*y10d)+sqrt(x20d*x20d+y20d*y20d);
    }
    else // why is this here???
        dii=2;
    
    double di;

      // we could use the A=, B=, C=, D= bit implemented in FindCriticalPoints
      // (and suggested in the paper) to compute di, but this (I think) is faster,
      // or at least as fast.
    if(y1==y2)
        di = fabs(y0-y1);
    else if(x1==x2)
        di = fabs(x0-x1);
    else
    {
        double m = (y2-y1)/(x2-x1);
        double t = (m * x1 - y1 + x0/m + y0) / (m + 1/m);
        double y = m * (t - x1) + y1;
        di = sqrt((y-y0)*(y-y0) + (t-x0)*(t-x0));
    }

    return(0.5*dii*di);
}

vector<int> FindCriticalPoints(contour_vector &contour, double critlevel_t,int shapemethod_t)
{
    vector<int> critind, Lpoints;
    int j;
    int jlo;
    int jhi;

      // find the pseudocritical M points 
      // (i.e. identify all local maxima in either angle or radii, then put the indices in critind).
      // note that radii and angle are supposed to be circular arrays, so special cases 
      // are needed for the two end points
    for(j=0,jlo=contour.size()-1,jhi=1;j<int(contour.size());jlo++,j++,jhi++)
    {
        if(jlo>=(int)contour.size())
            jlo=jlo-contour.size();
        if(jhi>=(int)contour.size())
            jhi=jhi-contour.size();

        if(sign(contour[j].get_angle() - contour[jlo].get_angle()) != sign(contour[jhi].get_angle() - contour[j].get_angle()) ||
           sign(contour[j].get_radius() - contour[jlo].get_radius()) != sign(contour[jhi].get_radius() - contour[j].get_radius()))
            critind.push_back(j);
    }

      // Now find pseudocritical chi points
      // step through adjacent pairs of M points
    for(j=0; j<int(critind.size()); j++)
    {
        int ind1=critind[j];
          // special case: last point has to be treated separately for wraparound
        int ind2;
        if(j==(int)critind.size()-1)
            ind2=critind[0];
        else
            ind2=critind[j+1];
        
          // make sure there is at least one boundary point between these two 
          // m points
        if(!(ind2 - ind1 > 1 || (ind1>ind2 && ind2+(critind.size()-ind1) > 1)))
            continue;

          // compute A, B, C, D (as defined in paper)
          // with a few shortcuts to avoid extra computation
        double r1=contour[ind1].get_radius(), r2=contour[ind2].get_radius();
        double a1=contour[ind1].get_angle(), a2=contour[ind2].get_angle();
        double r1_x_r2=r1 * r2;

        double A=contour[ind1].get_y() - contour[ind2].get_y();
        double B=contour[ind2].get_x() - contour[ind1].get_x();
        double C=r1_x_r2*sin(a2-a1);
        double D=r1*r1 + r2*r2 - 2*r1_x_r2*cos(a1-a2);

        double sqrt_D=sqrt(D);
        
        vector< pair<int, double> > pdist;
        
          // note: these distances can turn out to be negative, which doesn't make sense.
          // is that good or bad? i think it is okay, because the sign 
          // indicates which side of the point the line segment is on

          // special case for wrap around
        int i2;
        if(j==(int)critind.size()-1) {
            for(i2=ind1+1;i2<(int)contour.size();i2++)
                pdist.push_back(pair<int,double>(i2, (A*contour[i2].get_x()+B*contour[i2].get_y()+C)/sqrt_D));
            for(i2=0;i2<ind2;i2++)
                pdist.push_back(pair<int,double>(i2, (A*contour[i2].get_x()+B*contour[i2].get_y()+C)/sqrt_D));
        }
        else {
            for(i2=ind1+1;i2<ind2;i2++)
                pdist.push_back(pair<int,double>(i2, (A*contour[i2].get_x()+B*contour[i2].get_y()+C)/sqrt_D));
        }
        
        for(int i=0,ilo=pdist.size()-1,ihi=1;i<int(pdist.size());ilo++,i++,ihi++)
        {
              // special case for wrap around
            if(ilo>=(int)pdist.size())
                ilo=ilo-pdist.size();
            if(ihi>=(int)pdist.size())
                ihi=ihi-pdist.size();
            if(sign(pdist[i].second-pdist[ilo].second) != sign(pdist[ihi].second-pdist[i].second)) {
                Lpoints.push_back(pdist[i].first);
            }
        }
    }
    

      // merge together the chi and M points into one set.
      //
      // this used to be a merge sort (which is faster - the two lists
      // are already almost sorted) but due to 
      // wraparound, indices may be out of order. This could pretty 
      // easily be fixed so that merge sort is still possible.
      
    critind.insert(critind.end(),Lpoints.begin(),Lpoints.end());
    sort(critind.begin(), critind.end());

    vector<double> critlevel;
    
      // compute critical level of first pseudocritical point
    critlevel.push_back(ComputeCritLevel(contour,critind[critind.size()-1],critind[0],critind[1],shapemethod_t)); 
  
    for(j=1;j<(int)critind.size()-1;j++) 
        critlevel.push_back(ComputeCritLevel(contour,critind[j-1],critind[j],critind[j+1],shapemethod_t));
    
       // compute critical level of last pseudocritical point
    critlevel.push_back(ComputeCritLevel(contour,critind[critind.size()-2],critind[critind.size()-1],*(critind.begin()),shapemethod_t));


    double mincritlevel=-1;
    int minind=-1;

      // this really should be reimplemented by a priority queue. That
      // way we wouldn't have to rescan the list every time to find the
      // minimum critical level. (At worst, the current implementation is O(n^2)).
      //
      // 4/12/01 - crandall - I tried to do this, but it is difficult.
      // Since the critical levels of the neighbors change with each iteration,
      // you have to fish around the priority queue to find the neighbors and
      // then move them to the appropriate new place. Or you have to invalidate
      // the entries for the neighbors already in the queue and know to ignore them
      // when you pop them out. The overhead of this seems to overshadow any
      // performance gains, and I don't think a significant amount of cpu time is
      // spent here anyway.
      // I also tried implementing critind and critlevel as linked lists, since
      // we do frequent deletions of middle elements, which is not efficient with
      // vectors, and we only refer to the next and previous elements anyway. 
      // This actually increased execution time - why? Not sure.
      // But quoth the STL manual: "In almost all situations, vector is more efficient [than lists]."

    do
    {
#if 0 // KRD
        mincritlevel=-1;
        minind=-1;
        unsigned int size = critind.size();
        for (j = 0; j < size; j++)
        {
            if (critlevel[j] < mincritlevel || mincritlevel==-1)
            {
                minind=j;
                mincritlevel=critlevel[j];
            }
        }
#else
        mincritlevel = critlevel[0];
        minind = 0;
        int size = (int)critind.size();
        for (j = 1; j < size; j++)
        {
            if (critlevel[j] < mincritlevel)
            {
                minind = j;
                mincritlevel = critlevel[j];
            }
        }

#endif // KRD

        if (mincritlevel < critlevel_t && critlevel.size()>2)
        {
            critlevel.erase(critlevel.begin()+minind);
            critind.erase(critind.begin()+minind);
            
              // have to recompute the level for the items with indices 
              // minind (which is the one after the one we just deleted) and
              // minind-1 (which is the one before the one we just deleted)
            int ind1=minind-1; if(ind1 < 0) ind1 += critind.size();
            int ind2=minind;
            int ind1l=ind1-1; if(ind1l < 0) ind1l += critind.size();
            int ind1h=ind1+1; if(ind1h >= (int)critind.size()) ind1h -= critind.size();
            int ind2l=ind2-1; if(ind2l < 0) ind2l += critind.size();
            int ind2h=ind2+1; if(ind2h >= (int)critind.size()) ind2h -= critind.size();
            
            critlevel[ind1]=ComputeCritLevel(contour,critind[ind1l],critind[ind1],critind[ind1h],shapemethod_t);
            critlevel[ind2]=ComputeCritLevel(contour,critind[ind2l],critind[ind2],critind[ind2h],shapemethod_t);
      }

    } while(mincritlevel < critlevel_t && critlevel.size()>2);

    return(critind);
}




template<class T>
void draw_line(IemTPlane<T> &plane, Pt p1, Pt p2, T color=255)
{
    float dh,dw,r,c;
    int height = p2.first - p1.first,
        width  = p2.second - p1.second,
        n,i;

    if (height == 0 && width == 0) {
       plane[p1.first][p1.second]=color;
       return;
   }
   if (abs(height) > abs(width)) {
      dh = 1.0 * (height>0?1.0:-1.0);
      n  = abs(height);
      dw = 1.0 * width / abs(height);
   }
   else {
     dw = 1.0 * (width>0?1.0:-1.0);
      n  = abs(width);
      dh = 1.0 * height / abs(width);
   }
   r = p1.first;
   c = p1.second;
 
   for (i=0; i <= n; i++) {
       plane[int(r)][int(c)] = color;
       r += dh;   c += dw;
   }
 
   return;
}






// smooth_contour
//
// convolves a gaussian with in, 
// returns smoothed contour in out.
//
// this function could probably be optimized by taking the FFT of angles and vectors,
// multiplying by the FFT of the gaussian, and then taking the IFFT. However since
// the convolution isn't that big anyway, I'm not sure whether it is worth it.
void ConSeg::smooth_contour(contour_vector &in, contour_vector &out)
{
    int mid_point = conv_width / 2;

    int inSize = (int)in.size();

    // across the vector
    for (int r = 0; r < inSize; r++)
    {
        double angle = 0.0;
        double radius = 0.0;

        // across the width of the kernel, which is all 1's. (we're averaging)
        for (int q = 0; q < conv_width; q++)
        {
            int index = r + q - mid_point;

            while (index < 0)
                index += inSize;

            while (index >= inSize)
                index = index - inSize;
            
            angle += in[index].get_angle();
            radius += in[index].get_radius();
        }

        contour_pt new_pt(angle / conv_width, radius / conv_width);

        // save where this point was in the original contour
        // the smoothed result has fewer points
        new_pt.orig_index = r;

        out.push_back(new_pt);
    }

    // erase from end of unique stuff to end of array
    out.erase(unique(out.begin(), out.end()), out.end());
}

  // For each ncp, find contour points that are nearby
  // This distance test is (relatively) fast, so we do it first.
  // It also excludes most of the contour points so other tests
  // (which take longer) won't have to look at them.
  // warning: this has order O(n^2), figure out something better!
  //
  // Note: this could be optimized, e.g. using the following strategy:
  // we know that two adjacent contour points are at most sqrt(2) away
  // from one another. So if we know that the distance between the ncp point
  // and a given cp is x, and that dist_thresh2-x=y, with y>0, then we know
  // immediately that the y/sqrt(2) points on either side are also too close.
  // Similarly if y<0, we know that abs(y)/sqrt(2) contour points on either side
  // are of the correct distance.
  //
  // 4/16/02 - crandall - I have partially implemented this using the "skip" 
  // variable. Something better could be done. It may, for example, be worthwhile
  // to first do a 2-D sort based on location and then do the selection based on 
  // distance.

void ConSeg::select_by_euclidean_distance(contour_vector &sm_contour, vector<int> &ncps, vector<int> &cps, candidate_matrix &nearby_pts)
{
    int con_length=sm_contour.size();

    for(unsigned t=0; t<ncps.size(); t++) {
        double tx=sm_contour[ncps[t]].get_x();
        double ty=sm_contour[ncps[t]].get_y();
        candidate_vector this_nearby_pts;
        
          // arrange things so that we only look at contour points that are at least
          // as far away as the nearest critical points on either side
          // and also that they are at least dist_threshold2 away
        int end_index=find(cps.begin(),cps.end(),ncps[t])-cps.begin()-1;
        if(end_index<0)
            end_index+=cps.size();
        int beg_index=end_index+2;
        if(beg_index>=(int)cps.size())
            beg_index-=(int)cps.size();
        beg_index=cps[beg_index];
        end_index=cps[end_index];
        
        if(debug)
            printf("orig pt %d beg %d end %d contourleng %d\n",ncps[t],beg_index,end_index,con_length);
        
        int dist;
        dist=beg_index-ncps[t];
        if(dist < 0)
            dist+=con_length;
        if(dist < dist_threshold2)
            beg_index+=int(dist_threshold2)-dist;
        beg_index=beg_index % con_length;
        
        dist=ncps[t]-end_index;
        if(dist < 0)
            dist=ncps[t]+con_length-end_index;
        if(dist < dist_threshold2)
            end_index-=int(dist_threshold2)-dist;
        if(end_index < 0)
            end_index=(con_length+end_index);
        
        if(debug)
            printf("next pt %d beg %d end %d\n",ncps[t],beg_index,end_index);
        
        int skip=0;

        for(int s=beg_index; s!=end_index; s++) {
            if(s >= con_length)
                s-=con_length;
              // this is needed for the case end_index==0
            if(s==end_index)
                break;
            
            if(skip) {
                skip--;
                continue;
            }

            double x=sm_contour[s].get_x();
            double y=sm_contour[s].get_y();
              // took out sqrt to save time
            double dist=(x-tx)*(x-tx)+(y-ty)*(y-ty);
            if(dist < dist_threshold) {
                  // now check (cut length)/(contour separation) ratio.
                  // if this is > 0.333, do not allow the cut. 
                double d1=(s-ncps[t]);
                double d2=-d1;
                if(d1<0) d1+=con_length;
                if(d2<0) d2+=con_length;
                double ratio = sqrt(dist) / (min(d1,d2));
                if(ratio < 0.3333)
                    this_nearby_pts.push_back(candidate(s,dist,x,y));
                    
            }
            else {
                double diff=sqrt(dist)-dist_threshold_sqrt;
                skip=(int)(diff/1.4142135623);
            }
        }
        
        nearby_pts.push_back(this_nearby_pts);
    }
}


  // now scan through and remove points that are not within some angle threshold
  // of the perpendicular from the ncp

void ConSeg::filter_by_normal_angle(contour_vector &sm_contour, vector<int> &ncps, candidate_matrix &in, candidate_matrix &out)
{
    for(unsigned t=0; t<ncps.size(); t++) {
        candidate_vector this_nearby_ang_pts;
        
        int n=ncps[t];

          // compute derivatives at the ncp
        pair<double,double> first_deriv, second_deriv;
        sm_contour[n].get_derivatives(sm_contour, n, first_deriv, second_deriv);
          // this is the slope of the tangent line
        double m;
        if(first_deriv.second==0)
            m=10000000; // a Big Number
        else if(first_deriv.first==0)
            m=0;
        else
            m=(first_deriv.first/first_deriv.second);
        
          // figure out the angle of the perpendicular
        double perp_ang=atan(m);
        if(-first_deriv.second < 0)
            perp_ang+=M_PI;
        
        double x1=sm_contour[n].get_x();
        double y1=sm_contour[n].get_y();

          // adjust threshold based on local curvature
          // if curvature is high, the tangent/normal values are probably noisy
          // so we want to increase the acceptable angle range
        double ang_thresh;
        if(fabs(sm_contour[n].get_curvature(sm_contour, n)) >= fabs(hi_curve_thresh)) 
            ang_thresh=hi_curv_ang_thresh;
        else
            ang_thresh=usual_ang_thresh;
            
        
          // now for all contour point candidates
        for(unsigned s=0; s<in[t].size(); s++) {
              // find slope between ncp and this point
            double x2=in[t][s].x;
            double y2=in[t][s].y;
            
            double m2;
            if(x2-x1==0)
                m2=10000000*sign(-(y2-y1)); // a Big Number
            else
                m2=-(y2-y1)/(x2-x1);
            
              // figure out the angle 
            double pt_ang=atan(m2);
            
              // correct the sign
            if(x2-x1 < 0)
                pt_ang+=M_PI;
            
            
            if(fabs(perp_ang-pt_ang) < ang_thresh || fabs(perp_ang-pt_ang) > M_PI*2-ang_thresh) {
                this_nearby_ang_pts.push_back(in[t][s]);
            }
        }
        
        out.push_back(this_nearby_ang_pts);
    }
}
    
    
    
  // now scan through and remove points whose tangents are not within some angle threshold
  // of the tangent to the ncp

void ConSeg::filter_by_tangent_angle(contour_vector &sm_contour, vector<int> &ncps, candidate_matrix &in, candidate_matrix &out)
{
    
    for(int t=0; t<(int)ncps.size(); t++) {
        candidate_vector this_nearby_tan_pts;
        
        double tan_ang=sm_contour[ncps[t]].get_tangent_angle(sm_contour,ncps[t]);
        
          // what we really want to do is check that the two lines are almost 180 degrees apart
        tan_ang+=M_PI;
        if(tan_ang >= 3.0*M_PI/2.0)
            tan_ang-=M_PI*2.0;

          // adjust threshold based on local curvature
          // if curvature is high, the tangent/normal values are probably noisy
          // so we want to increase the acceptable angle range
        double ang_thresh;
        if(fabs(sm_contour[ncps[t]].get_curvature(sm_contour, ncps[t])) >= fabs(hi_curve_thresh)) 
            ang_thresh=hi_curv_ang_thresh;
        else
            ang_thresh=usual_ang_thresh;
        
          // now for all contour point candidates
        for(int s=0; s<(int)in[t].size(); s++) {
            
            double pt_ang = sm_contour[in[t][s].index].get_tangent_angle(sm_contour, in[t][s].index);
            

            if(fabs(tan_ang-pt_ang) < ang_thresh || fabs(tan_ang-pt_ang) > M_PI*2-ang_thresh)
                this_nearby_tan_pts.push_back(in[t][s]);
        }
        
        out.push_back(this_nearby_tan_pts);
    }
}

vector<int> ConSeg::perform_local_nonmax_suppression(contour_vector &sm_contour, vector<int> &ncps, vector<int> &cps)
{
    vector<int> sncps;

      // handle first and last separately.
    vector<int>::iterator iter, iter2;
    int i=0;
    int beg_ncp_i=0;
    int ncpsSize = (int)ncps.size();
    while(beg_ncp_i+1 < ncpsSize)
    {
          // find a consecutive string of ncps, without other cps inbetween
          // (also if the distance between the ncps is pretty long, pretend like there is a cp in between)
        for(i=beg_ncp_i; i+1<(int)ncps.size(); i++) {
            iter=find(cps.begin(),cps.end(),ncps[i]);
            iter2=find(cps.begin(),cps.end(),ncps[i+1]);
            if(distance(iter,iter2) > 1 || *iter2-*iter > dist_threshold_sqrt/2.0) {
                  // there is a pcp between.
                break;
            }
        }
        
          // if i and beg_ncp_i are the same, we have a group of only 1 adjacent ncp.
        if(beg_ncp_i == i) {
              // push it as a salient ncp
            beg_ncp_i++;
            sncps.push_back(ncps[i]);
            continue;
        }

        int end_ncp_i=i;
        
        vector<int> sncp_temp;
          // note: insert() is kind of semi-quasi-anti-intuitive in that it adds items in
          // the range [first,last), that is the item pointed to by the second iterator isn't really added.
        sncp_temp.insert(sncp_temp.end(),ncps.begin()+beg_ncp_i, ncps.begin()+end_ncp_i+1);

        for(i=0; i<(int)sncp_temp.size(); i++) {
            double this_curv = fabs(sm_contour[sncp_temp[i]].get_curvature(sm_contour, sncp_temp[i]));
            if((i > 0 && fabs(sm_contour[sncp_temp[i-1]].get_curvature(sm_contour, sncp_temp[i-1])) <= this_curv || i==0) &&
               (i+1 < (int)sncp_temp.size() && fabs(sm_contour[sncp_temp[i+1]].get_curvature(sm_contour, sncp_temp[i+1])) 
                <= this_curv || i+1==(int)sncp_temp.size())) {
                sncps.push_back(*(sncp_temp.begin()+i));
            }
        }
        beg_ncp_i=end_ncp_i+1;
    }

      // need to add last one
    if(ncps.size()>0)
        sncps.push_back(*(ncps.end()-1));

    return(sncps);
}



void ConSeg::process_region(short labelValue)
{
    contour_vector in_contour;
    int startrow = 0;
    int startcol = 0;
    
    // follow and store contour of region
    if (!FollowContour(in_contour, contourbuf, segseg, labelValue,
      startrow, startcol))
        return;

    if (debug)
        printf("follow contour done, points=%d.\n",in_contour.size());
        
    // ignore regions with short contours
    if (in_contour.size() < min_contour_length)
        return;

    contour_vector sm_contour;

    // perform gaussian smoothing
    smooth_contour(in_contour, sm_contour);

    int con_length = (int)sm_contour.size();
 
    if (con_length < dist_threshold2 + dist_threshold2) 
		return;

    // compute the critical points
    vector<int> cps = FindCriticalPoints(sm_contour,crit_thresh,crit_method);
    if(debug)
        printf("critical points found, count=%d\n",cps.size());

      // of the critical points, figure out which ones are negative curvature points
    vector<int> ncps;
        
    for (unsigned c=0; c < cps.size(); c++) {
        double curvature = sm_contour[cps[c]].get_curvature(sm_contour, cps[c]);

        if(curvature < -0.05)
        {
            ncps.push_back(cps[c]);
        }
    }

    // find "salient" negative curvature points. This is supposed to prevent
    // multiple ncps very close together.
    vector<int> sncps=perform_local_nonmax_suppression(sm_contour, ncps, cps);

    // Okay so we have a collection of critical points and negative curvature
    // critical points. Now what?
    //
    // For each negative curvature point, we have to decide whether to create
    // a cut or not. We also have to decide the other endpoint of the cut. 
    //
    // Try this:
    // - for each ncp, find a normal line to the curve,
    //     based on local derivatives.
    // - Locate contour points that are within some distance threshold
    //     dist_thresh of each ncp and that are within some angle threshold
    //     from the normal.  These contour points may not be located between
    //     the ncp and the two critical points on either side (this ensures
    //     that there is some significant shape feature that we're trying to
    //     "cut out"). Also, if the normal line cuts through more than
    //     one contour point, include only the closest one. 
    // - If there are no such points, don't make a cut.
    // - If there are such points, and one of them is a ncp, great!
    //     Draw a cut to there.
    //     If there's more than one ncp within the range,
    //     do something to resolve the conflict.
    //     (The following step was removed; I don't think it makes as much
    //     sense as I did before).
    //     [ - If there are no ncp points but there is/are a cp point,
    //     use it instead. (It's still supposed to be a significant shape
    //     feature, so this makes sense hopefully.)]
    // [ - (old rule) If there are no cp points or ncp points, just draw the
    //     cut along the normal line until you reach the contour. ]
    // - If there are no ncp points, draw the cut towards the closest
    //    candidate point.

        candidate_matrix nearby_pts;

        // sncps ==> salient (significant) negative curvature critical points 
        //   cps ==> critical points 
        select_by_euclidean_distance(sm_contour, sncps, cps, nearby_pts);

        candidate_matrix nearby_ang_pts;
        
        filter_by_normal_angle(sm_contour, sncps, nearby_pts, nearby_ang_pts);

        candidate_matrix nearby_tan_pts;

        filter_by_tangent_angle(sm_contour, sncps, nearby_ang_pts,
          nearby_tan_pts);

        // now select a point from the candidates for each ncp, and join them. 
        vector< pair<int, candidate> > selected;
        
        for(int t=0; t<(int)sncps.size(); t++)
        {
            // if there's no candidates left, skip this ncp
            if(nearby_tan_pts[t].size() == 0)
            {
                continue;
            }
            
            candidate *selected_pt=0;
            
            // finally, just choose the closest
            if (selected_pt==0)
            {
                double min_dist=9999999999.9998;
                for (int z=0; z<(int)nearby_tan_pts[t].size(); z++)
                {
                    if (nearby_tan_pts[t][z].distance < min_dist)
                    {
                        min_dist=nearby_tan_pts[t][z].distance;
                        selected_pt=&(nearby_tan_pts[t][z]);
                    }
                }

                // if possible, we would like to choose an ncp.
                // But don't do it if the ncp is much further away
                // (distance > 10% more) than the closest point we found.
                //
                // sometimes a nearby ncp is NOT a candidate due to high
                // curvature or noisy values. In this case we might like to
                // choose a point nearby the ncp, as long as that point
                // satisfies our distance criteria and IS a candidate.
                //
                // to resolve between multiple ncps, we keep track of the
                // closest distance so far and choose the closest one at
                // the end

                double min_ncp_dist=9999999999.9998; // a Big Number
                int min_ncp_index=-1;
                for(int s=0; s<(int)sncps.size(); s++)
                {
                    for(int u=-5; u<=5; u++)
                    {
                        int v=u+sncps[s];
                        if(v < 0)
                            v+=sm_contour.size();
                        if(v >= (int)sm_contour.size())
                            v-=sm_contour.size();
                    
                        candidate_vector::iterator iter;

                        // distance+abs(u) is just an approximation of the
                        // distance of the actual ncp, since that distance
                        // might not be known
                        if((iter=find(nearby_tan_pts[t].begin(),
                          nearby_tan_pts[t].end(), v)) !=
                          nearby_tan_pts[t].end() &&
                           iter->distance <= 1.2 * min_dist &&
                           iter->distance+abs(u) < min_ncp_dist)
                        {
                            min_ncp_index=s;
                            min_ncp_dist=iter->distance+abs(u);
                        }
                    }
                }

                if (min_ncp_index > -1)
                {
                    candidate_vector::iterator iter;
                    // if the other ncp we selected wasn't originally a
                    // candidate, then we have to add it as one
                    if((iter=find(nearby_tan_pts[t].begin(),
                      nearby_tan_pts[t].end(), sncps[min_ncp_index])) ==
                      nearby_tan_pts[t].end())
                    {
                        nearby_tan_pts[t].push_back(
                          candidate(sncps[min_ncp_index],
                            sm_contour[sncps[min_ncp_index]],
                            sm_contour[sncps[t]]));
                        iter = nearby_tan_pts[t].end()-1;
                    }
                        
                    selected_pt=iter;
                }
            }

            selected.push_back(pair<int,candidate>(sncps[t],*selected_pt));
        }

        if(debug) 
        {
              // this code just generates debugging images.

            int   r;
            int   u;
            int   v;

            for(r=0;r<con_length;r++)
            {
                int x1=sm_contour[r].get_x()+startcol;
                int y1=sm_contour[r].get_y()+startrow;
                
                if(x1>=0 && y1>=0 && x1<segseg.cols() && y1<segseg.rows())
                    shapebuf[0][y1][x1]=shapebuf[1][y1][x1]=shapebuf[2][y1][x1]=labelValue;
            }

            for(r=0;r<(int)cps.size();r++)
            {
                int t=cps[r];
                int x1=sm_contour[t].get_x()+startcol;
                int y1=sm_contour[t].get_y()+startrow;
                
                for(u=-2; u<=2; u++)
                    for(v=-2; v<=2; v++)
                    {
                        if((abs(u)==2 || abs(v)==2) && y1+u >= 0 && x1+v >= 0 &&
                           y1+u < shapebuf.rows() && x1+v < shapebuf.cols()) {
                            shapebuf[0][y1+u][x1+v]=0;
                            shapebuf[1][y1+u][x1+v]=0xff;
                            shapebuf[2][y1+u][x1+v]=0;
                        }
                    }
            }

            for(r=0;r<(int)ncps.size();r++)
            {
                int t=ncps[r];
                int x1=sm_contour[t].get_x()+startcol;
                int y1=sm_contour[t].get_y()+startrow;
                
                for(u=-2; u<=2; u++)
                    for(v=-2; v<=2; v++)
                    {
                        if((abs(u)==2 || abs(v)==2) && y1+u >= 0 && x1+v >= 0 &&
                           y1+u < shapebuf.rows() && x1+v < shapebuf.cols()) {
                            shapebuf[0][y1+u][x1+v]=0xff;
                            shapebuf[1][y1+u][x1+v]=r;
                            shapebuf[2][y1+u][x1+v]=0xff;
                        }
                    }
  
                  // this just draws a pretty line showing the perpendicular
                double m;
                pair<double,double> deriv, deriv2;
                
                sm_contour[t].get_derivatives(sm_contour, t, deriv, deriv2);

                if(deriv.first!=0)
                    m=deriv.second/deriv.first;
                else
                    m=1000000;

                int x2=(int)sqrt(100/(1+m*m));
                int y2=(int)sqrt(100-x2*x2);
                x2=x1+x2*int(sign(deriv.first));
                y2=y1+y2*int(sign(deriv.second));
//              m=-1/m;

                int y3=int(sqrt(100/(1+m*m))*sign(deriv.first));
                int x3=int(sqrt(100-(y3*y3))*sign(deriv.second));
                x3=x1-x3;
                y3=y1+y3;
                
                if(x2>=0 && y2>=0 && x2<shapebuf.cols() && y2<shapebuf.rows())
                    draw_line(shapebuf[1],Pt(y2,x2),Pt(y1,x1));
                if(x3>=0 && y3>=0 && x3<shapebuf.cols() && y3<shapebuf.rows())
                    draw_line(shapebuf[2],Pt(y3,x3),Pt(y1,x1));
                
            }

            for(r=0;r<(int)sncps.size();r++)
            {
                int t=sncps[r];
                int x1=sm_contour[t].get_x()+startcol;
                int y1=sm_contour[t].get_y()+startrow;
                
                for(u=-2; u<=2; u++)
                    for(v=-2; v<=2; v++)
                    {
                        if((abs(u)==2 || abs(v)==2) && y1+u >= 0 && x1+v >= 0 &&
                           y1+u < shapebuf.rows() && x1+v < shapebuf.cols()) {
                            shapebuf[0][y1+u][x1+v]=0x3f;
                        }
                    }
            }


            for(r=0;r<(int)nearby_tan_pts.size();r++)
            {
                for(int s=0; s<(int)nearby_tan_pts[r].size(); s++) {
                    int t=nearby_tan_pts[r][s].index;

                    int x1=sm_contour[t].get_x()+startcol;
                    int y1=sm_contour[t].get_y()+startrow;
                    
                    if(y1 >= 0 && x1 >= 0 && y1 < shapebuf.rows() && x1 < shapebuf.cols()) {
                        shapebuf[0][y1][x1]=0xff;
                        shapebuf[1][y1][x1]=0;
                        shapebuf[2][y1][x1]=0;
                    }
                }
                
            }
        }

        vector< pair<int, candidate> >::iterator sel_iter;

        for(sel_iter=selected.begin(); sel_iter != selected.end(); ++sel_iter) 
        {
              // 4/17/2002 - crandall - not sure why this was working before. Should not be
              // looking at smooth contour here, but original contour. Otherwise cuts may
              // not appear properly in final segmentation map. Note the small recordkeeping
              // headache that results, since there is not a 1:1 relationship between smoothed contour
              // points and original contour points.
            int x1=in_contour[sm_contour[sel_iter->first].orig_index].get_x()+startcol;
            int y1=in_contour[sm_contour[sel_iter->first].orig_index].get_y()+startrow;
            int x2=in_contour[sm_contour[sel_iter->second.index].orig_index].get_x()+startcol;
            int y2=in_contour[sm_contour[sel_iter->second.index].orig_index].get_y()+startrow;
            
            if(debug) {

                  // these are lines on the smoothed contour, not the original (like above)
                int x1=sm_contour[sel_iter->first].get_x()+startcol;
                int y1=sm_contour[sel_iter->first].get_y()+startrow;
                int x2=sm_contour[sel_iter->second.index].get_x()+startcol;
                int y2=sm_contour[sel_iter->second.index].get_y()+startrow;

                if((y1 >= 0 && x1 >= 0 && y1 < shapebuf.rows() && x1 < shapebuf.cols()) &&
                   (y2 >= 0 && x2 >= 0 && y2 < shapebuf.rows() && x2 < shapebuf.cols())) {
                    draw_line(shapebuf[0], Pt(y2,x2), Pt(y1,x1));
                    draw_line(shapebuf[1], Pt(y2,x2), Pt(y1,x1));
                    draw_line(shapebuf[2], Pt(y2,x2), Pt(y1,x1));
                }
            }
            
            if((y1 >= 0 && x1 >= 0 && y1 < contourbuf.rows() && x1 < contourbuf.cols()) &&
               (y2 >= 0 && x2 >= 0 && y2 < contourbuf.rows() && x2 < contourbuf.cols())) {
//                if(debug) {
// i think this should always be done. 4/3/2002
                    draw_line(segseg, Pt(y2,x2), Pt(y1,x1), short(0));
//                }
                
                draw_line(cuts, Pt(y2,x2), Pt(y1,x1), short(0));
            }
        }
}


// in_image is really a plane, it's IemTPlane<short>
//
IemTPlane<short> ConSeg::contour_segmentation(IemTPlane<short> &inImage,
  char *debug_name)
{
    int height = inImage.rows();
    int width = inImage.cols();
    IemTPlane<short> returnPlane(height, width);

    try
    {
        int i;
        int j;

//writeRawPlane("contour_segmentation-input", inImage);

        // force 8-connectivity to be 4-connectivity by adding appropriate
        // pixels
        // if a pixel is 8-connected but not 4-connected, make it 4-connected
        //
        // IE
        //
        // region description   this becomes this or        this
        // 8 4 8              255 0 0        255 255 0       255  0  0
        // 4 P 4                0 P 0          0  P  0       255  P  0
        // 8 4 8                0 - 0          0  0  0         0  0  0
        //
        // previous steps require on 8-connected connectivity
        // subsequence steps require 4-connected connectivity
        // This loop prevents lots of tiny little segments that we'd get if we
        // didn't change 8-connected to 4-connected.
        //

        int iMinus1;
        int iPlus1;
        int jMinus1;
        int jPlus1;
        int height = inImage.rows();
        int width = inImage.cols();

        for (i = 1, iMinus1 = 0, iPlus1 = 2;
          i < height - 1;
          ++i, ++iMinus1, ++iPlus1)
        {
            for (j = 1, jMinus1 = 0, jPlus1 = 2;
              j < width - 1; ++j, ++jMinus1, ++jPlus1)
            {
                if (0 == inImage[i][j])
                {
                    // current pixel is 0, look around it to see if we
                    // need to create 4-connectivity by adding a non-zero
                    // pixel note we are not making 4-connectivity with the
                    // current pixel
                    short leftPixel = inImage[iMinus1][j];
                    short rightPixel = inImage[iPlus1][j];

                    //
                    // if either of the following occurs:
                    //
                    //        0   X   .      .  .  .
                    //        X  [0]  .  or  X [0] .
                    //        .   .   .      0  X  .
                    //
                    //  We set the current pixel to be the one left of it,
                    //  creating 4-connectivity, ending up with:
                    //
                    //        0   X   .      .  .  .
                    //        X  [X]  .  or  X [X] .
                    //        .   .   .      0  X  .
                    //
                    if (
                      (0 == inImage[iMinus1][jMinus1] && 0 != leftPixel &&
                       leftPixel == inImage[i][jMinus1]) ||

                      (0 == inImage[iMinus1][jPlus1] && 0 != leftPixel &&
                       leftPixel == inImage[i][jPlus1])
                       )
                    {
                        inImage[i][j] = leftPixel;
                    }
                    else if (

                      //
                      // If either of the following occurs:
                      //
                      //  .  .  .        .   X   0
                      //  . [0] X  or    .  [0]  X 
                      //  .  X  0        .   .   .
                      //
                      //  We set the current pixel to be the one right of it,
                      //  creating 4-connectivity, ending up with:
                      //
                      //  .  .  .        .   X   0
                      //  . [X] X  or    .  [X]  X 
                      //  .  X  0        .   .   .

                      (0 == inImage[iPlus1][jPlus1] &&
                       0 != inImage[i][jPlus1] &&
                       inImage[i][jPlus1] == rightPixel) ||

                      (0 == inImage[iPlus1][jMinus1] &&
                       0 != inImage[i][jMinus1] &&
                         rightPixel == inImage[i][jMinus1])
                      )
                    {
                            inImage[i][j] = rightPixel;
                    }
                }
            }
        }

        // do connected component labelling
        // This can now throw THROW_AU_LIB_ERROR("Label overflow error.", 1);
        LabelPlane<short, short> regionLabels(inImage, 4, 0, true, true);

        // 
        // now, fill in holes
        // according to the documentation, this is very efficient
        // countour segmentation is dealing with the outlines of regions
        // we want simple outlines, not one region "containing" another one
        // the FillHole() will change the label of "inner" region to match that
        // of the outer one.
        //
        //  Here's the idea:
        //  XXXXXXXXXX
        //  XXYYYYYYZZ
        //  XXXYYYYYZZ
        //  XXXYYYYYZZ  ==> all X's
        //  XXXYYYYYZZ
        //  XXXYYYYYZZ
        //  XXXXXXXXXX
        //
        int nLabels = regionLabels.size();
        for (i = 0; i < nLabels; i++)
        {
            if (0 == regionLabels[i].getPixelValue())        
            {
                regionLabels.FillHole(i);
            }
        }
        
        segseg = regionLabels.GetLabelPlane();
        
        if(debug)
            printf("region labels done=%d\n", regionLabels.size());

//writeRawPlane("contour_segmentation-segseg", segseg);

          // initialize some buffers used to dump debug images
        if (debug)
        {
            shapebuf = IemTImage<unsigned char>(3, height, width);
            shapebuf=0;
        }

        // this eventually will store the segmentation cuts
        // all pixels initially have value "1"
        cuts = IemTPlane<short>(height, width);
        cuts = 1;

        // this buffer stores contour points
        // KRD -- yes, IemTPlanes are constructed "height" first
        contourbuf = IemTPlane<short>(height, width);

        // figure out the contour points, using the "integer" version of
        // the "floating point" labelPlane
        find_contour_points(segseg, contourbuf);

        // now process each region
//writeRawPlane("contour_segmentation-contourBuf", contourbuf);

        nLabels = regionLabels.size();
        for(i = 1; i < nLabels; i++)
        {
            // Ignore background and invalid regions.
            // pixel value of 0 implies background
            // an invalid region occurs in the FillHoles() routine above; labels
            // on the interior that get merged to a bigger region are marked
            // invalid
            //
            // also, ignore any region that's area is to small
            
            if (0 == regionLabels[i].getPixelValue() ||
              !regionLabels[i].isValid() ||
              (int)regionLabels[i].getCount() < small_area_thresh)
                continue;

            if (debug)
                printf("processing region %d...\n",i);

            iuBoundingBox region_box = regionLabels[i].getBoundingBox();

            // dist_threshold is our "first line of defense" when choosing potential cuts.
            // It is kind of an arbitrary feature (really it shouldn't be needed or wanted),
            // but it prevents us from wasting our time considering contour points that are
            // obviously too far away to be cut to, and prevents us from making really strange cuts.
            //
            // Normally we set it to a fraction of the segment's lesser dimension. Compact segments
            // (wrt bounding box) deserve a higher threshold than non-compact segments (probably thin
            // squiggly things). It is bad if dist_threshold_sqrt gets too big.
            // Very strange cuts can start taking place.
            // This usually occurs only in background segments that are trash anyway.
            // To prevent this, saturate this threshold at a percentage of the image dimensions.

            double fill_factor =
              (double)regionLabels[i].getCount() / region_box.area();
            double dist_thresh_ratio = initial_dist_thresh_ratio;
            if (fill_factor > 0.5)
                dist_thresh_ratio=0.6;

            if (debug)
                printf("%d %f\n",regionLabels[i].getCount(), fill_factor);

            dist_threshold_sqrt =
              min(region_box.right - region_box.left,
                  region_box.bottom - region_box.top) * dist_thresh_ratio;

            int min_img_dim = min(width, height);

            dist_threshold_sqrt = min(min_img_dim / 3.0, dist_threshold_sqrt);

            // dist_threshold2 specifies how far (along the contour) a candidate must be from the ncp.
            // A salience requirement in select_by_euclidean_distance would usually take care of this
            // anyway, but including this threshold saves time, and prevents extremely tiny cuts that
            // mathematically satisfy the salience criterion from taking place.

            dist_threshold2 = dist_threshold_sqrt + dist_threshold_sqrt;
            dist_threshold = dist_threshold_sqrt * dist_threshold_sqrt;

            process_region(i);
        }

        // remark, to apply changes to segmentation map.
        // regions that have been cut get new, different labels
        LabelPlane<short, short> regionLabels2(segseg,4,0,false,false);
        IemTPlane<short>         segseg2 = regionLabels2.GetLabelPlane();
        
        //if(debug) {
        //    floatseg=floatseg+10;
        //}

        //int   rows = segseg2.rows();   // height
        //int   cols = segseg2.cols();   // width
        
        // ensure background is still the background
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                if (cuts[i][j] == 0)
                {
                    // "cut" pixels will be part of the same region with
                    // label 1
                    //
                    // we increment labels if not part of a cut,
                    // so it won't collide
                    returnPlane[i][j] = 1;
                }
                else if (inImage[i][j] == 0)
                {
                    // background of original image ==>
                    //     background in labeled image
                    //
                    // commented out 6/19/02 (Dave Crandall)
                    // By commenting this out , we remove many holes from the
                    // output segmentation map. Initially we thought this was a
                    // bad idea, because it fills in eyes. However AREA 3.5
                    // searches all face pixels for red eyes and so there is no
                    // need to preserve the eye holes. By commenting these lines
                    // out, we reduce the number of regions that are sent
                    // downstream to AREA, thereby (hopefully) reducing
                    // processing time.
                    //
                    //returnPlane[i][j] = 0;
                    //
                    // KRD/Dave 7/11/2002 We copy from floatSeg2 so that
                    // we don't end up with a returnPlane full of
                    // uninitialized values
                    //
                    returnPlane[i][j] = segseg2[i][j] + 1;
                }
                else
                {
                    // shift non-background up from 0; want 0 to be background
                    returnPlane[i][j] = segseg2[i][j] + 1;
                }
            }
        }

#ifdef CAN_WRITE_IEM_IMAGES
        if(debug)
        {
            char temp[1024];
            sprintf(temp, "%s_segseg.tif",debug_name);
            iemWrite(convert(segseg,IemShort),temp);

            sprintf(temp, "%s_contourbuf.tif",debug_name);
            iemWrite(IemImage(contourbuf),temp);

            sprintf(temp, "%s_shapebuf.tif",debug_name);
            iemWrite(IemImage(shapebuf),temp);
        }
#endif

//writeRawPlane("contour_segmentation-returnPlane", returnPlane);
        
        return returnPlane;
    }
    catch (const auLibError &e)
    {
        // in the case of an error, just zero out the whole return plane
        // (it will be all background)
        returnPlane = 0;
        return returnPlane;
    }
    catch (...)
    {
        // in the case of an error, just zero out the whole return plane
        // (it will be all background)
        returnPlane = 0;
        return returnPlane;
    }
}
