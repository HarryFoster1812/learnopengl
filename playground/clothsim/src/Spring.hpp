#pragma once
class Point; // forward declaration

class Spring {
public:
  Spring(Point *p1, Point *p2, float restLength, int p1Index, int p2Index);
  Point *getP1() const { return p1; }
  Point *getP2() const { return p2; }
  int getP1Index() const { return p1Index; }
  int getP2Index() const { return p2Index; }
  float getRestLength() const { return restLength; }
  void Break() { active = false; }
  bool isActive() const { return active; }

private:
  int p1Index;
  int p2Index;
  Point *p1;
  Point *p2;
  float restLength;
  bool active = true;
};
