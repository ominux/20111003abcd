CC 	 =	g++
LIBS	 = 	-lm 

mazeRoute: main.o Maze.o Point.o PrimMST.o Net.o Box.o random.o Segment.o RouteEdge.o RouteTracks.o Graph.o CoupleFree.o SteinerTree.o GridGraph.o
	g++ -o mazeRoute main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o random.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

mazeRoute.db: main.db.o Maze.db.o Point.db.o PrimMST.db.o Net.db.o Box.db.o CoupleFree.db.o Segment.db.o Graph.db.o SteinerTree.db.o GridGraph.db.o
	g++ -o mazeRoute.db main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

mazeRoute.prof: main.p.o Maze.p.o Point.p.o PrimMST.p.o Net.p.o Box.p.o CoupleFree.p.o Segment.p.o Graph.p.o SteinerTree.p.o GridGraph.p.o
	g++ -pg -o mazeRoute.prof main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

clean:
	rm *.o *~ mazeRoute mazeRoute.db mazeRoute.prof

temp: Net.db.o Segment.db.o temp.o Box.db.o CoupleFree.db.o Graph.db.o
	g++ -o temp temp.o Box.o Segment.o Net.o CoupleFree.o Graph.o

temp.o: temp.cc
	g++ -c -g temp.cc 

# ------------------ Debug compile ------------------------------

main.db.o: main.cc Maze.db.o Point.db.o 
	g++ -c -g main.cc

Maze.db.o: Maze.cc Maze.h MazePoint.h Point.db.o Pin.h Net.db.o PrimMST.db.o RouteTracks.db.o 
	g++ -c -g Maze.cc

Net.db.o: Net.h Net.cc Pin.h Point.db.o Box.db.o Segment.db.o SteinerTree.db.o
	g++ -c -g Net.cc

Point.db.o: Point.h Point.cc 
	g++ -c -g Point.cc

PrimMST.db.o: PrimMST.cc PrimMST.h Net.db.o Pin.h Point.db.o
	g++ -c -g PrimMST.cc

RouteEdge.db.o: Net.db.o RouteEdge.h RouteEdge.cc
	g++ -c -g RouteEdge.cc

RouteTracks.db.o: MazePoint.h RouteEdge.db.o PrimMST.db.o RouteTracks.cc RouteTracks.h
	g++ -c -g RouteTracks.cc

Box.db.o: Box.h Box.cc Point.db.o
	g++ -c -g Box.cc

CoupleFree.db.o: CoupleFree.cc CoupleFree.h Net.db.o Graph.db.o
	g++ -c -g CoupleFree.cc

Segment.db.o: Point.db.o Segment.h Segment.cc
	g++ -c -g Segment.cc

Graph.db.o: Graph.h Graph.cc
	g++ -c -g Graph.cc

SteinerTree.db.o: SteinerTree.h SteinerTree.cc GridGraph.db.o Pin.h Point.o
	g++ -c -g SteinerTree.cc

GridGraph.db.o: GridGraph.h GridGraph.cc Graph.db.o Pin.h Point.o
	g++ -c -g GridGraph.cc

# ----------------------- Optimized compile ----------------------------

main.o: main.cc Maze.o Point.o 
	g++ -c -O2 main.cc

Box.o: Box.h Box.cc Point.o
	g++ -c -O2 Box.cc

Maze.o:	Maze.cc Maze.h MazePoint.h Point.o Pin.h Net.o PrimMST.o RouteTracks.o
	g++ -c -O2 Maze.cc

Net.o: Net.h Net.cc Pin.h Point.o Box.o Segment.o SteinerTree.o
	g++ -c -O2 Net.cc

Point.o: Point.h Point.cc 
	g++ -c -O2 Point.cc

PrimMST.o: PrimMST.cc PrimMST.h Net.o Pin.h Point.o
	g++ -c -O2 PrimMST.cc

RouteEdge.o: Net.o RouteEdge.cc RouteEdge.h
	g++ -c -O2 RouteEdge.cc

RouteTracks.o: MazePoint.h RouteEdge.o PrimMST.o RouteTracks.cc RouteTracks.h
	g++ -c -O2 RouteTracks.cc

random.o: random.cc
	g++ -c -O2 random.cc

CoupleFree.o: CoupleFree.cc CoupleFree.h Net.o Graph.o
	g++ -c -O2 CoupleFree.cc

Segment.o: Point.o Segment.h Segment.cc
	g++ -c -O2 Segment.cc

Graph.o: Graph.h Graph.cc
	g++ -c -O2 Graph.cc

SteinerTree.o: SteinerTree.h SteinerTree.cc GridGraph.o Pin.h Point.o
	g++ -c -O2 SteinerTree.cc

GridGraph.o: GridGraph.h GridGraph.cc Graph.o Point.o Pin.h
	g++ -c -O2 GridGraph.cc

# --------------------- Profiling compile --------------------------

main.p.o: main.cc Maze.p.o Point.p.o 
	g++ -c -pg main.cc

Box.p.o: Box.h Box.cc Point.p.o
	g++ -c -pg Box.cc

Maze.p.o: Maze.cc Maze.h MazePoint.h Point.p.o Pin.h Net.p.o PrimMST.p.o RouteTracks.p.o
	g++ -c -pg Maze.cc

Net.p.o: Net.h Net.cc Pin.h Point.p.o Box.p.o Segment.p.o SteinerTree.p.o
	g++ -c -pg Net.cc

Point.p.o: Point.h Point.cc 
	g++ -c -pg Point.cc

PrimMST.p.o: PrimMST.cc PrimMST.h Net.p.o Pin.h Point.p.o
	g++ -c -pg PrimMST.cc

RouteEdge.p.o: Net.p.o RouteEdge.cc RouteEdge.h
	g++ -c -pg RouteEdge.cc

RouteTracks.p.o: MazePoint.h RouteEdge.p.o PrimMST.p.o RouteTracks.cc RouteTracks.h
	g++ -c -pg RouteTracks.cc

random.p.o: random.cc
	g++ -c -pg random.cc

CoupleFree.p.o: CoupleFree.h CoupleFree.cc Net.p.o Graph.p.o
	g++ -c -pg CoupleFree.cc

Segment.p.o: Point.p.o Segment.cc Segment.h
	g++ -c -pg Segment.cc

Graph.p.o: Graph.h Graph.cc
	g++ -c -pg Graph.cc

SteinerTree.p.o: SteinerTree.h SteinerTree.cc GridGraph.p.o Pin.h Point.p.o
	g++ -c -pg SteinerTree.cc

GridGraph.p.o: GridGraph.h GridGraph.cc Graph.p.o Pin.h Point.o
	g++ -c -pg GridGraph.cc
