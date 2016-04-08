#include "ProgressBar.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

int setWidth(int barWidth)
{
  if (barWidth > 0)
    return barWidth;

  int cols = 80;
  if (isatty(2))
    {
      struct winsize w;
      ioctl(STDERR_FILENO, TIOCGWINSZ, &w);
      
      cols = w.ws_col;
    }
  return cols
}

ProgressBar::ProgressBar(int maxVal, int barWidth)
{
  m_maxVal   = maxVal;
  m_barWidth = setWidth(barWidth);
  m_unit     = 100/m_barWidth;
  m_fence    = m_unit;
  m_mark     = 10;
  m_count    = -1;

  for (int i = 0; i< 10; i++)
    m_symbolA[i] = '+';
  m_symbolA[4] = '|';
  m_symbolA[9] = '|';
}







int main (int argc, char **argv)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf ("lines %d\n", w.ws_row);
    printf ("columns %d\n", w.ws_col);
    return 0;  // make sure your main returns int
}
