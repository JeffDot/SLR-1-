SLR-1-
=================================
author:JeffDot
enviroment:VisualStudio2010
about:
  this is a slr syntax analyzer ,
  the algorithm came from dragon-
  -book .
  Actually this is my complier-le-
  -sson's homework ,i try to use 
  the method of slr ,i finish it.
  pretty happy for me
=================================
=================================
what i learn from this work?
first
  how the slr work:
      the key point was to create
      a analysis table ,the anal-
      -yzer read in character and
      check the ACTION[state,cha-
      -racter],according to the 
      result to decide what to do
      there are four possibility
      with it
        movin
        rank
        err
        acc
  how to create a slr analysis t-
  -able:
  i think we must get the FIRST(-
  -to get the FOLLOW set) set and
  the FOLLOW set#this has puzzle 
  me for a long time ,at first i 
  think these two set just use 4
  the 'from top to bottom' method
  -in my book it is ,and i use t-
  he algorithm in book ,i got a 
  wrong result-it get dead-loop)
  to get these two set-just look 
  at book but do some small chan-
  -ge : if the set isn't empty ,
  so we think it has get all its
  elem (because we use Recursive
  So this is the truth,it will 
  find all the clues like holmes
  )
  when we get the two set ,the 
  work is easy to finish ,convert
  the algorithm in book to your 
  own code,test and test ,Done!
  second:
  learn the STL template like
    pair<T1 ,T2>
    vector<T1>
    map<T1 ,T2>
  third :
  try to code a program using STL
  thoughts:
    use iterator but Array[]
    use size_type but int i
    maybe it is helpful 4 transp-
    -lantation
  
==================================
