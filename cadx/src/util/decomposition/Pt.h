// Pt.h: header file for 'Pt' (point) class
// crandall, 10/2001

#ifndef _PT_H_
#define _PT_H_


typedef pair<int, int> Pt;

Pt operator-(Pt const &c1, Pt const &c2);
Pt operator+(Pt const &c1, Pt const &c2);
Pt operator*(Pt const &c1, Pt const &c2);
Pt operator*(Pt const &c1, double const &c2);
Pt operator*(double const &c2, Pt const &c1);
Pt operator/(Pt const &c1, int const &i);
bool operator<(Pt const &p1, Pt const &p2);
bool operator>(Pt const &p1, Pt const &p2);
bool operator==(Pt const &p1, Pt const &p2);
bool operator>=(Pt const &p1, Pt const &p2);
bool operator<=(Pt const &p1, Pt const &p2);
double norm(Pt const &p1);
Pt abs(Pt const &p1);
bool test_adjacent(Pt const &p1, Pt const &p2, int prox_thresh=1);

#endif
