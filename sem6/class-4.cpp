#include <iostream>
#include <algorithm>
#include <vector>

Coord getPoint() {
  Coord tmp;
  cin>>tmp.x>>tmp.y;
  return tmp;
}

void getTri(Triangle& tri) {
  tri.p1 = getPoint();
  tri.p2 = getPoint();
  tri.p3 = getPoint();
}

uint64_t getDistSq(Coord p1, Coord p2) {
  int dx = p1.x-p2.x;
  int dy = p1.y-p2.y;
  return dx*dx + dy*dy;
}

int cmp(Triangle a, Triangle b) {
  uint64_t da1 = getDistSq(a.p1, a.p2);
  uint64_t da2 = getDistSq(a.p1, a.p3);
  uint64_t da3 = getDistSq(a.p2, a.p3);

  uint64_t db1 = getDistSq(b.p1, b.p2);
  uint64_t db2 = getDistSq(b.p1, b.p3);
  uint64_t db3 = getDistSq(b.p2, b.p3);

  std::vector<uint64_t> da{da1, da2, da3};
  std::vector<uint64_t> db{db1, db2, db3};

  std::sort(da.begin(), da.end());
  std::sort(db.begin(), db.end());

  for (int i = 0; i < 3; i++) {
    if (da[i] != db[i]) {
      return 0;
    }
  }

  return 1;
}
