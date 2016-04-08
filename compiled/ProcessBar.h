#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

class ProgressBar
{
public:
  ProgessBar(int maxVal, int barWidth = 0);
  ~ProgessBar() {}
  
private:
  int m_maxVal;
  int m_barWidth;
  int m_unit;
  int m_fence;
  int m_mark;
  int m_count
  char m_symbolA[10];
};




#endif //PROGRESSBAR_H
