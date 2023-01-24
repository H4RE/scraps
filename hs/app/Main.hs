module Main (main) where

import Sort(quickSort)

main :: IO ()
main = do
    print $ quickSort [10, 5, 8, 9, 0]
