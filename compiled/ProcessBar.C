#include "ProgressBar.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

int setWidth()
{
  struct winsize w;
  ioctl(STDERR_FILENO, TIOCGWINSZ, &w);
  
  cols = w.ws_col;
  return cols
}

ProgressBar::ProgressBar(int maxVal, int barWidth)
{
  m_active = false;
  if (! isatty(2))
    return;
  
  m_active      = true;
  m_maxVal      = (double) maxVal;
  int termWidth = setWidth();
  if (barWidth < 1)
    m_barWidth = termWidth;
  if (m_barWidth > termWidth)
    m_barWidth = termWidth;
  if (m_barWidth < 40)
    barWidth = 40;
  

  m_unit     = 100.0/((double)m_barWidth);
  m_fence    = m_unit;
  m_mark     = 10;
  m_count    = -1;

  for (int i = 0; i < 10; i++)
    m_symbolA[i] = '+';
  m_symbolA[4] = '|';
  m_symbolA[9] = '|';
}

void ProcessBar::update(int i)
{
  if (! m_active)
    return;

  double j = 100.0*i/m_maxVal;
  double k = 100.0*(i+1)/m_maxVal;

  if ( j <= m_fence && k > m_fence )
    {
      char symbol = '-';
      if ( j <= m_mark && k > m_mark)
        {
          m_count++;
          
          m_mark += 10;
          symbol = m_symbolA[m_count];
        }
      fprintf(stderr,"%c",symbol);
      m_fence += m_unit;
    }
}

void ProcessBar::fini()
{
  if (! m_active)
    return;
  fprintf(stderr,"\n");
}
