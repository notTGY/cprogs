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

  int64_t cda2 = da2 - da1;
  int64_t cda3 = da3 - da1;

  int64_t cdb2 = db2 - db1;
  int64_t cdb3 = db3 - db1;

  if (cdb2 == cda2 && cdb3 == cda3) return 1;
  if (cdb3 == cda2 && cdb2 == cda3) return 1;

  return 0;
}
