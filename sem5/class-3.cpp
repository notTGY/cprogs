void printLin(Lin a) {
  cout <<a.cm << ' ' << a.mm << endl;
}

Lin convertToLin(int kletki) {
  Lin res;
  res.mm = 5*(kletki % 2);
  res.cm = (kletki - kletki % 2) / 2;
  return res;
}
