-- Stanislas Gueniffey (ULB, 000377223)

import qualified System.Environment
import Debug.Trace
import Data.List
import Data.Maybe

-- MAIN

main :: IO ()
main = do [path] <- System.Environment.getArgs
          maze <- readFile path
          putStr $ unlines $ escape $ lines maze

-- convert text to maze, solve it and convert it back to text
escape :: [[Char]] -> [[Char]]
escape maze = case solution of
                Nothing -> ["I'm A-Mazednt (FAIL)"] -- pretty error message
                Just sol -> mazeToText sol
              where solution = solveMaze $ textToMaze maze -- convert and solve

-- SOLVING

solveMaze :: Maze ->  Maybe Maze
solveMaze maze = case solution of 
                   Nothing -> Nothing
                   Just sol -> Just $ markPath maze $ tail sol  -- mark the solution except the exit
                 where solution = solveMazeIter maze (mazeFind maze Exit) [[mazeFind maze Start]]

-- solves by iteratively expanding a list of possible paths until reaching exit coordinates
solveMazeIter :: Maze -> Coord -> [Path] -> Maybe Path
solveMazeIter maze exit [] = Nothing -- no more paths: no solution
solveMazeIter maze exit (best:rest) = if minDistance (head best) exit == 0 -- exit reached
                                      then Just best -- just the best path
                                      else solveMazeIter maze exit (expand maze exit best rest)

-- DATA TYPES

-- cells are maze units, each is one of five things
data Cell = Wall | Space | Start | Path | Exit deriving (Eq)

charToCell :: Char -> Cell
charToCell 'X' = Wall
charToCell ' ' = Space
charToCell '*' = Start
charToCell '@' = Exit

cellToChar :: Cell -> Char
cellToChar Wall = 'X'
cellToChar Space = ' '
cellToChar Start = '*'
cellToChar Path = '.'
cellToChar Exit = '@'

-- mazes are two dimensional list of cells
type Maze = [[Cell]]

textToMaze :: [[Char]] -> Maze
textToMaze = map (map charToCell)

mazeToText :: Maze -> [[Char]]
mazeToText = map (map cellToChar)

-- index structure for Mazes: (row, col)
type Coord = (Int, Int)
row :: Coord -> Int
row (r, c) = r

col :: Coord -> Int
col (r, c) = c

-- sequence of coordinates
type Path = [Coord]

-- HELPER FUNCTIONS

-- generate all possible coordinates
mazeCoords :: Maze -> [Coord]
mazeCoords m = let rows = length m - 1
                   cols = (length $ head m) - 1
               in [(row, col) | row <- [0..rows], col <- [0..cols]]

-- get cell value from coordinates
mazeGet :: Maze -> Coord -> Cell
mazeGet maze coord = (maze !! row coord) !! col coord

-- replace cell value at coordinates by splitting and joining along two axis
mazeSet :: Maze -> Coord -> Cell -> Maze
mazeSet maze coord cell = let (rowsBefore,rowCell:rowsAfter) = splitAt (row coord) maze
                          in let (colsBefore,colCell:colsAfter) = splitAt (col coord) rowCell
                             in rowsBefore ++ (colsBefore ++ cell : colsAfter) : rowsAfter

-- find first coordinates with cell value (throws error if none)
mazeFind :: Maze -> Cell -> Coord
mazeFind maze cell = let result = let coords = mazeCoords maze
                                  in find (\x -> (mazeGet maze x == cell)) coords
                     in case result
                        of Nothing -> error "No such cell found"
                           Just res -> res

-- generate all possible neighbour coordinates
neighbours :: Coord -> [Coord]
neighbours coord = let (r, c) = (row coord, col coord)
                   in [(r+1, c), (r-1, c), (r, c+1), (r, c-1)]

accessible :: Maze -> Coord -> [Coord]
accessible maze coord = filter (\x -> case mazeGet maze x of 
                                        Space -> True -- spaces are accessible
                                        Exit -> True -- so is the exit
                                        otherwise -> False) (neighbours coord) -- nothing else is

-- manhattan distance between two coordinates, assuming no obstacles (admissible heuristic for A*)
minDistance :: Coord -> Coord -> Int
minDistance c1 c2 = abs (row c2 - row c1) + abs (col c2 - col c1)

-- path cost based on cost so far (path length) and min. remaining cost (minDistance from exit)
pathCost :: Path -> Coord -> Int
pathCost [] exit = 0
pathCost path exit = length path + minDistance (head path) exit

-- insert into ordered list (used as priority queue for path selection)
insertOrd :: Path -> [Path] -> Coord -> [Path]
insertOrd path [] exit = [path]
insertOrd path (p:ps) exit
  | pathCost path exit > pathCost p exit = p : insertOrd path ps exit
  | otherwise = path : (p:ps)

-- multiple insertions into ordered list
insertOrdIter :: [Path] -> [Path] -> Coord -> [Path]
insertOrdIter [] paths exit = paths
insertOrdIter (p:ps) paths exit = insertOrdIter ps (insertOrd p paths exit) exit

-- takes a maze, exit coordinates, a current path, a list of paths, and extends the latter with all new possible paths from the current path (TODO optimize with open/closed set as in real A*)
expand :: Maze -> Coord -> Path -> [Path] -> [Path]
expand maze exit best rest = 
  insertOrdIter (filter (\x -> not $ elem x rest) 
                        [a:best | a <- filter (\x -> not $ elem x best) (accessible maze $ head best)]) 
                 rest exit


-- displays path onto maze
markPath :: Maze -> Path -> Maze
markPath maze [] = maze -- no more path
markPath maze (c:[]) = maze -- last step is exit
markPath maze (c:cs) = markPath (mazeSet maze c Path) cs
