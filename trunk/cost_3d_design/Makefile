CC 	 =	g++
LIBS	 = 	-lm 

costDesign: main.o Area.o Module.o
	g++ -o costDesign main.o Area.o Module.o

costDesign.db: main.db.o Area.db.o Module.db.o
	g++ -o costDesign.db main.o Area.o Module.o

clean: rm *.o *~ costDesign costDesign.db

#mazeRoute: main.o Maze.o Point.o PrimMST.o Net.o Box.o random.o Segment.o RouteEdge.o RouteTracks.o Graph.o CoupleFree.o SteinerTree.o GridGraph.o
#	g++ -o mazeRoute main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o random.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

#mazeRoute.db: main.db.o Maze.db.o Point.db.o PrimMST.db.o Net.db.o Box.db.o CoupleFree.db.o Segment.db.o Graph.db.o SteinerTree.db.o GridGraph.db.o
#	g++ -o mazeRoute.db main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

#mazeRoute.prof: main.p.o Maze.p.o Point.p.o PrimMST.p.o Net.p.o Box.p.o CoupleFree.p.o Segment.p.o Graph.p.o SteinerTree.p.o GridGraph.p.o
#	g++ -pg -o mazeRoute.prof main.o Maze.o Point.o PrimMST.o RouteEdge.o RouteTracks.o  Net.o Box.o CoupleFree.o Segment.o Graph.o SteinerTree.o GridGraph.o

#clean:
#	rm *.o *~ mazeRoute mazeRoute.db mazeRoute.prof

#temp: Net.db.o Segment.db.o temp.o Box.db.o CoupleFree.db.o Graph.db.o
#	g++ -o temp temp.o Box.o Segment.o Net.o CoupleFree.o Graph.o

#temp.o: temp.cc
#	g++ -c -g temp.cc 

# ------------------ Debug compile ------------------------------

main.db.o: main.cc Area.db.o Module.db.o
	g++ -c -g main.cc

Area.db.o: Area.h Area.cc
	g++ -c -g Area.cc

Module.db.o: Module.h Module.cc
	g++ -c -g Module.cc

#main.db.o: main.cc Maze.db.o Point.db.o 
#	g++ -c -g main.cc

#Maze.db.o: Maze.cc Maze.h MazePoint.h Point.db.o Pin.h Net.db.o PrimMST.db.o RouteTracks.db.o 
#	g++ -c -g Maze.cc

#Net.db.o: Net.h Net.cc Pin.h Point.db.o Box.db.o Segment.db.o SteinerTree.db.o
#	g++ -c -g Net.cc

#Point.db.o: Point.h Point.cc 
#	g++ -c -g Point.cc
# ----------------------- Optimized compile ----------------------------
main.o: main.cc Area.o Module.o
	g++ -c -O2 main.cc

Area.o: Area.h Area.cc
	g++ -c -O2 Area.cc

Module.o: Module.h Module.cc
	g++ -c -O2 Module.cc


#main.o: main.cc Maze.o Point.o 
#	g++ -c -O2 main.cc

#Box.o: Box.h Box.cc Point.o
#	g++ -c -O2 Box.cc

#Maze.o:	Maze.cc Maze.h MazePoint.h Point.o Pin.h Net.o PrimMST.o RouteTracks.o
#	g++ -c -O2 Maze.cc

#Net.o: Net.h Net.cc Pin.h Point.o Box.o Segment.o SteinerTree.o
#	g++ -c -O2 Net.cc
