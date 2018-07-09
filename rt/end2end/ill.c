int main()
{
  int c=6;
  ((void(*)())&c)();
  return 0;
}
