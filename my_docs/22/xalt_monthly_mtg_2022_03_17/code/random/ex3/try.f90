! -*- f90 -*-
program tryMe
   implicit none 
   real*8 a,b,c,d, random
   a = 1.0
   b = 2.0
   c = 3.0

   print *, "Hello World!"
   d = random(a,b,c)
   print *, "d: ",d
   
   
end program tryMe
