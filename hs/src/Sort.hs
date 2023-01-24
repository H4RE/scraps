module Sort
  ( quickSort,
  )
where

quickSort :: (Ord a) => [a] -> [a]
quickSort [] = []
quickSort (x : xs) = quickSort smaller ++ [x] ++ quickSort bigger
  where
    smaller = [v | v <- xs, v < x]
    bigger = [v | v <- xs, v > x]
