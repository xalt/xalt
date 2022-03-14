real*8 function random(a, b, c)
   implicit none
   real*8 a, b, c
   print *, "In random(a, b, c)"
   random = a*b + c
end function random
   
