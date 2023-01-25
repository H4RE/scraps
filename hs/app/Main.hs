module Main (main) where

import Sort(quickSort, mergeSort)

main :: IO ()
main = do
    print $ quickSort [10, 5, 8, 9, 0]
    print $ mergeSort [10, 5, 8, 9, 0]
