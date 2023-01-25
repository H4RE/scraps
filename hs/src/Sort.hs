module Sort
  ( quickSort,
    mergeSort,
  )
where

quickSort :: (Ord a) => [a] -> [a]
quickSort [] = []
quickSort (x : xs) = quickSort smaller ++ [x] ++ quickSort bigger
  where
    smaller = [v | v <- xs, v < x]
    bigger = [v | v <- xs, v > x]

mergeSort:: (Ord a) => [a]->[a]
mergeSort [] = []
mergeSort [x] = [x]
mergeSort xs = merge (mergeSort  first) (mergeSort second)
  where
    first = take n xs
    second = drop n xs
    n = length xs `div` 2
    merge:: (Ord a) => [a] -> [a] ->[a]
    merge xs [] = xs 
    merge [] ys = ys 
    merge (x:xs) (y:ys)
      |x < y = x : merge xs (y:ys)
      |otherwise = y: merge (x:xs) ys