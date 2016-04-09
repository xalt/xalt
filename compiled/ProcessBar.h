#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

class ProgressBar
{
public:
  ProgessBar(int maxVal, int barWidth = 0);
  ~ProgessBar() {}
  void update(int i);
  void fini();
  
private:
  bool   m_active;
  int    m_barWidth;
  int    m_count
  int    m_mark;
  double m_maxVal;
  double m_unit;
  double m_fence;
  char   m_symbolA[10];
};




#endif //PROGRESSBAR_H
