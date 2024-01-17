# search.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Kelvin Ma (kelvinm2@illinois.edu) on 01/24/2021

"""
This is the main entry point for MP1. You should only modify code
within this file -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""
# Search should return the path.
# The path should be a list of tuples in the form (row, col) that correspond
# to the positions of the path taken by your search algorithm.
# maze is a Maze object based on the maze from the file specified by input filename
# searchMethod is the search method specified by --method flag (bfs,dfs,astar,astar_multi,fast)

from collections import deque
from collections import namedtuple
import heapq
import time

def bfs(maze):
    """
    Runs BFS for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    
    """
    frontier = deque()
    explored = deque()
    
    frontier.append(maze.start)
    explored.append(maze.start)
   
    location = namedtuple('location',['parent','dist'])
    
    routes = {}
    
    routes[maze.start] =  location((-1,-1),0)
    
    #add the start point to the explored routes, pop it from the left of frontier and extend frontier on the right with neighbors to explore
    
    dad = frontier.popleft()
    kids = maze.neighbors(dad[0],dad[1])
    for i in range(0,len(kids)):
        if kids[i] not in maze.waypoints:
            routes[kids[i]] = location(dad,routes[dad].dist+1)
            frontier.append(kids[i])
              
    #create infinite loop that will break only when the frontier is empty
    
    while(len(frontier)!=0):     
        dad=frontier.popleft()
        kids = maze.neighbors(dad[0],dad[1])
        explored.append(dad)
        for i in range(0,len(kids)):
            if (kids[i] in routes):
                # this means that this place has been seen before and currently is in frontier or explored so update its details 
                if(routes[kids[i]].dist > routes[dad].dist+1):
                    routes[kids[i]]=routes[kids[i]]._replace(parent= dad)                  
                    routes[kids[i]]=routes[kids[i]]._replace(dist= routes[dad].dist+1)
                                       
            else:
               # if this place has been reached for the first time make note of it
                routes[kids[i]] = location(dad,routes[dad].dist+1)
                # if it isnt the final ending then add this to the frontier
                if kids[i] not in maze.waypoints:     
                    frontier.append(kids[i])
                else:
                    # if this is final dest then add it to explored and use remaining things in frontier to find alternate path
                    explored.append(kids[i])
   
                    

    cell= maze.waypoints[0]
    path = [cell]
    while(cell!=maze.start):
        path.append(routes[cell].parent)
        cell = routes[cell].parent
    path.reverse()
    
    return path

def astar_single(maze):
    """
    Runs A star for part 2 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    st = time.time()
        
    frontier = deque()
    explored = deque()    
   
    location = namedtuple('location',['parent','dist','hdist'])
    
    routes = {}
    
    starth = abs(maze.start[0] - maze.waypoints[0][0])+abs(maze.start[1] - maze.waypoints[0][1])
    
    routes[maze.start] =  location((-1,-1),0,starth )
    
    frontier.append(maze.start)
    explored.append(maze.start)
    
    #add the start point to the explored routes, pop it from the left of frontier and extend frontier on the right with neighbors to explore
    
    dad = frontier.popleft()
    kids = maze.neighbors(dad[0],dad[1])
    for i in range(0,len(kids)):
        if kids[i] not in maze.waypoints:
            routes[kids[i]] = location(dad,routes[dad].dist+1, routes[dad].dist+1+abs(kids[i][0] - maze.waypoints[0][0])+abs(kids[i][1] - maze.waypoints[0][1]))
            frontier.append(kids[i])
              
    #create infinite loop that will break only when the destination is found
    
    while(maze.waypoints[0] not in routes):  
        #edit this part to pop the one with smallest hdist
        h=[]
        for fro in frontier:
            heapq.heappush(h,(routes[fro].hdist, fro))
        
        dad=heapq.heappop(h)[1]
        
        frontier.remove(dad)
        kids = maze.neighbors(dad[0],dad[1])
        explored.append(dad)
        for i in range(0,len(kids)):
            if (kids[i] in routes):
                # this means that this place has been seen before and currently is in frontier or explored so update its details
                #a waypoint can never be here as the loop would break when the below else statement adds the waypoint
                if(routes[kids[i]].dist > routes[dad].dist+1):
                    routes[kids[i]]=routes[kids[i]]._replace(parent= dad)                  
                    routes[kids[i]]=routes[kids[i]]._replace(dist= routes[dad].dist+1)
                    routes[kids[i]]=routes[kids[i]]._replace(hdist= routes[kids[i]].dist+abs(kids[i][0] - maze.waypoints[0][0])+abs(kids[i][1] - maze.waypoints[0][1]))
                                       
            else:
               # if this place has been reached for the first time make note of it
                dreach = routes[dad].dist+1 
                routes[kids[i]] = location(dad,dreach, dreach+abs(kids[i][0] - maze.waypoints[0][0])+abs(kids[i][1] - maze.waypoints[0][1]))
                # if it isnt the final ending then add this to the frontier
                if kids[i] not in maze.waypoints:     
                    frontier.append(kids[i])
                else:
                    # if this is final dest then add it to explored and break out
                    explored.append(kids[i])
                    break
   
                    

    cell= maze.waypoints[0]
    path = [cell]
    while(cell!=maze.start):
        path.append(routes[cell].parent)
        cell = routes[cell].parent
    path.reverse()
    
    tend = time.time()
    
    print(tend-st, "time taken")
  
    return path
 

def astar_corner(maze):
    """
    Runs A star for part 3 of the assignment in the case where there are four corner objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
        """
    tbegin = time.time()
    frontier = deque()
    explored = deque()    
   
    location = namedtuple('location',['parent','dist','hdist'])
    
    routes = {}
    
    objectives = []
    
    mstree= {}
    
    finalpath=[maze.start]
    
    objectives.extend(maze.waypoints)
    obj = tuple(objectives)
    
    mstree[obj] = findMST(objectives)
    
    st = maze.start
    
    closest = closestwaypoint(st,objectives)
    
    mstcost = mstree[obj]
        
    starth = abs(st[0] - closest[0])+abs(st[1] - closest[1])+mstcost
    
    routes[maze.start] =  location((-1,-1),0,starth )
    
    frontier.append(maze.start)
    explored.append(maze.start)
    
    #add the start point to the explored routes, pop it from the left of frontier and extend frontier on the right with neighbors to explore
    
    dad = frontier.popleft()
    kids = maze.neighbors(dad[0],dad[1])
    
    
    for i in range(0,len(kids)):
        if kids[i] not in objectives:
            closest = closestwaypoint(kids[i],objectives)
            heur = routes[dad].dist+1+abs(kids[i][0] - closest[0])+abs(kids[i][1] - closest[1])+mstree[obj]
            routes[kids[i]] = location(dad,routes[dad].dist+1, heur)
            frontier.append(kids[i])
        else:
            finalpath.append(kids[i])
            st = kids[i]
            objectives.remove(kids[i])
            obj = tuple(objectives)
            if(len(objectives)>1):
                mstree[obj] = findMST(objectives)
            else:
                mstree[obj] = 0
            mstcost = mstree[obj]
            routes.clear()
            routes[st] =  location((-1,-1),0,0 )
            frontier.clear()
            explored.clear()
            frontier.append(st)
            explored.append(st)
            break
            
    #create infinite loop that will break only when there are no more remaining objectives
 
    while(len(objectives)!=0):  
        #this part to pop the one with smallest hdist
        h=[]
        for fro in frontier:
            heapq.heappush(h,(routes[fro].hdist, fro))
        
        dad=heapq.heappop(h)[1]  
        frontier.remove(dad)
        kids = maze.neighbors(dad[0],dad[1])
        explored.append(dad)
        
        for i in range(0,len(kids)):
            
            obj = tuple(objectives)
           
            mstcost = mstree[obj]
            
            if(len(objectives)>1):
                closest = closestwaypoint(kids[i],objectives)
            else:
                closest = objectives[0]
            
            distclosest = abs(kids[i][0] - closest[0])+abs(kids[i][1] - closest[1])
            
            if (kids[i] in routes):
                # this means that this place has been seen before and currently is in frontier or explored so update its details
                #a waypoint can never be here as the loop would break when the below else statement adds the waypoint
                if(routes[kids[i]].dist > routes[dad].dist+1):
                    routes[kids[i]]=routes[kids[i]]._replace(parent= dad)                  
                    routes[kids[i]]=routes[kids[i]]._replace(dist= routes[dad].dist+1)
                    routes[kids[i]]=routes[kids[i]]._replace(hdist= routes[kids[i]].dist + distclosest + mstcost)
                                       
            else:
               # if this place has been reached for the first time make note of it
                dreach = routes[dad].dist+1 
                routes[kids[i]] = location(dad,dreach, dreach + distclosest + mstcost )
                # if it isnt the final ending then add this to the frontier
                if kids[i] not in objectives:     
                    frontier.append(kids[i])
                else:
                    # if this is final dest then add it to explored and break out
                    cell = kids[i]
                    explored.append(cell)
                    path = [cell]
                    while(cell!=st):
                        path.append(routes[cell].parent)
                        cell = routes[cell].parent
                    path.remove(st)
                    path.reverse()
                    finalpath.extend(path)
#                   now reset everything to find math to next waypoint from this waypoint
                    st = kids[i]
                    objectives.remove(kids[i])
                    obj = tuple(objectives)
                    if(len(objectives)>1):
                        mstree[obj] = findMST(objectives)
                    else:
                        mstree[obj] = 0
                    mstcost = mstree[obj]
                    routes.clear()
                    routes[st] =  location((-1,-1),0,0 )
                    frontier.clear()
                    explored.clear()
                    frontier.append(st)
                    explored.append(st)
                    break
   
                   
     
    tend = time.time()    
    print(tend-tbegin, "time taken")
#     print(finalpath)
    
    far = astar_final(maze)
    
    if(len(far)<len(finalpath)):
        lol=editpath(far)
        print(lol)
        return lol
    else:
        lol=editpath(finalpath)
        print(lol)
        return lol
        

def astar_multiple(maze):
    """
    Runs A star for part 4 of the assignment in the case where there are
    multiple objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
#     final = astar_corner(maze)
    
#     far= astar_far(maze)
    
#     mst = astar_corner(maze)
    
# #     print(len(lol))
#     if(len(mst)<len(far)):
#         return mst
#     else:
#         return far
    return astar_final(maze)

def fast(maze):
    """
    Runs suboptimal search algorithm for part 5.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    return []











""" HELPER FUNCTIONS USED FOR ABOVE FUNCTIONS"""

"""finds the closest waypoint given a list of remaining objectives"""

def closestwaypoint(pt,remaining):
    h=[]
    for r in remaining:
        heapq.heappush(h,(abs(pt[0]-r[0])+abs(pt[1]-r[1]), r))
    return heapq.heappop(h)[1]
        

"""finds the length of mstree using manhatten distance given a list of remaining objectives"""
def findMST(rema):
    ##first compute all edges
    remaining=[]
    remaining.extend(rema)
    h=[]
    vertices = len(remaining)
    while(len(remaining)>1):
        edge = remaining.pop()
        for e in remaining:
            d = abs(e[0] - edge[0])+abs(e[1] - edge[1])
            heapq.heappush(h, (d, [edge,e]))
    
    mstree = []
    
    cost = 0
    x=heapq.heappop(h)
    mstree.append(x[1])
    cost = cost+x[0]
    while(len(mstree[0])!= vertices):
        edge = heapq.heappop(h)
#         print(edge)
        x = edge[1][0]
        y = edge[1][1]
        c = edge[0]
        mergey=0
        mergex=0
        xtree=[]
        ytree=[]
        discard = 0
        for t in mstree:
            if(x in t and y not in t):     
                xtree.extend(t) 
                mergex=1
                mstree
            elif(y in t and x not in t):
                ytree.extend(t)
                mergey=1
            elif(y in t and x in t):
                discard = 1
        if(discard!=1):
            if(mergex ==1 and mergey == 1):
                mstree.remove(xtree)
                mstree.remove(ytree)
                xtree.extend(ytree)
                mstree.append(xtree)
                cost = cost + c
            elif(mergex==1 and mergey==0):
                mstree.remove(xtree)
                xtree.append(y)
                mstree.append(xtree)
                cost = cost + c
            elif(mergex==0 and mergey ==1):
                mstree.remove(ytree)
                ytree.append(x)
                mstree.append(ytree)
                cost = cost + c
            elif(mergex==0 and mergey==0):
                mstree.append([x,y])
                cost = cost +c
    return cost
        

    

def farthest(pt,remaining):
    h=[]
    for r in remaining:
        heapq.heappush(h,(abs(pt[0]-r[0])+abs(pt[1]-r[1]), r))
    while(len(h)>1):
        heapq.heappop(h)
    lol = heapq.heappop(h)[0]  
    return lol
          

"""another attempt to use farthest waypoint among those the point gets closest to"""   
    
def astar_far(maze):
    """
    Runs A star for part 3 of the assignment in the case where there are four corner objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
        """
    tbegin = time.time()
    frontier = deque()
    explored = deque()    
   
    location = namedtuple('location',['parent','dist','hdist'])
    
    routes = {}
    
    objectives = []
    
    mstree= {}
    
    finalpath=[maze.start]
    
    objectives.extend(maze.waypoints)
    obj = tuple(objectives)
    
    
    st = maze.start
    
    starth = farthest(st,objectives)
    
    routes[maze.start] =  location((-1,-1),0,starth )
    
    frontier.append(maze.start)
    explored.append(maze.start)
    
    #add the start point to the explored routes, pop it from the left of frontier and extend frontier on the right with neighbors to explore
    
    dad = frontier.popleft()
    kids = maze.neighbors(dad[0],dad[1])
    
    mstree={}
    obj = tuple(objectives)
    
    mstree[obj] = findMST(objectives)
    
    daddist = {}
    for o in objectives:
        daddist[o] = abs(o[0] - dad[0])+abs(o[1] - dad[1])
        
    for i in range(0,len(kids)):
        if kids[i] not in objectives:
            
            if(len(objectives)>1):
                far = farwaypoint(kids[i], daddist, objectives) 
            else:
                far = farthest(kids[i], objectives)
            routes[kids[i]] = location(dad,routes[dad].dist+1, far)
            frontier.append(kids[i])
            
        else:       
            obj = tuple(objectives)
            currmst = mstree[obj]
            rem = []
            rem.extend(objectives)
            rem.remove(kids[i])
            remm = tuple(rem)
            if(len(rem)>1):
                msremove = findMST(rem)
                mstree[remm] = msremove
            else:
                mstree[remm] = 0
            
            # mark this waypoint now only if removing it will result in a reduced MST
            
            if(msremove<currmst or currmst==0):
                finalpath.append(kids[i])
                st = kids[i]
                objectives.remove(kids[i])
                obj = tuple(objectives)
                
                if(len(objectives)>1):
                    mstree[obj] = findMST(objectives)
                else:
                    mstree[obj] = 0
                    
                mstcost = mstree[obj]
                routes.clear()
                routes[st] =  location((-1,-1),0,0 )
                frontier.clear()
                explored.clear()
                frontier.append(st)
                explored.append(st)
                break
            else:
                if(len(objectives)>1):
                    far = farwaypoint(kids[i], daddist, objectives) 
                else:
                    far = farthest(kids[i], objectives)
                routes[kids[i]] = location(dad,routes[dad].dist+1, far)
                frontier.append(kids[i])
            
    #create infinite loop that will break only when there are no more remaining objectives
     
    while(len(objectives)!=0):  
        #this part to pop the one with smallest hdist
        h=[]
        for fro in frontier:
            heapq.heappush(h,(routes[fro].hdist, fro))
        
        dad=heapq.heappop(h)[1]  
        frontier.remove(dad)
        kids = maze.neighbors(dad[0],dad[1])
        explored.append(dad)
        
        daddist = {}
        for o in objectives:
            daddist[o] = abs(o[0] - dad[0])+abs(o[1] - dad[1])
        
        for i in range(0,len(kids)):
            
            if(len(objectives)>1):
                far = farwaypoint(kids[i], daddist, objectives)
            else:
                far = abs(objectives[0][0] - kids[i][0])+abs(objectives[0][1] - kids[i][1])
            
            if (kids[i] in routes):
                # this means that this place has been seen before and currently is in frontier or explored so update its details
                #a waypoint can never be here as the loop would break when the below else statement adds the waypoint
                if(routes[kids[i]].dist > routes[dad].dist+1):
                    routes[kids[i]]=routes[kids[i]]._replace(parent= dad)                  
                    routes[kids[i]]=routes[kids[i]]._replace(dist= routes[dad].dist+1)
                    routes[kids[i]]=routes[kids[i]]._replace(hdist= far)
                                       
            else:
               # if this place has been reached for the first time make note of it
                dreach = routes[dad].dist+1 
                routes[kids[i]] = location(dad,dreach, far )
                # if it isnt the final ending then add this to the frontier
                if kids[i] not in objectives:
                    frontier.append(kids[i])
                else:
                    # if this is final dest then add it to explored and break out
                    cell = kids[i]
                    explored.append(cell)
                    path = [cell]
                    while(cell!=st):
                        path.append(routes[cell].parent)
                        cell = routes[cell].parent
                    path.remove(st)
                    path.reverse()
                    finalpath.extend(path)
                    #now reset everything to find math to next waypoint from this waypoint
                    st = kids[i]
                    objectives.remove(kids[i])
                    obj = tuple(objectives)
                    if(len(objectives)>1):
                        mstree[obj] = findMST(objectives)
                    else:
                        mstree[obj] = 0
                    mstcost = mstree[obj]
                    routes.clear()
                    if(len(objectives)>0):
                        far = farthest(st, objectives)
                        routes[st] =  location((-1,-1),0,far )
                    frontier.clear()
                    explored.clear()
                    frontier.append(st)
                    explored.append(st)
#                     print("check point found = ", st)
                    break
   
                   
     
    tend = time.time()    
    print(tend-tbegin, "time taken")
#     print(finalpath)
    
    return editpath(finalpath)



"""Function that returns distance to the farthest waypoint amongst those this location gets closer to"""
def farwaypoint(pt, distdict, remaining):
    near={}
    for r in remaining:
        objdist = abs(r[0]-pt[0])+abs(r[1]-pt[1])
        if(objdist < distdict[r]):
            near[r] = objdist
    if(len(near)==0):
        return farthest(pt, remaining)
    else:
        maxobj = max(near.values()) 
        return maxobj

"""function to edit repeats"""
def editpath(path):
    changeind=[]
    for i in range(0,len(path)-5):
        x = path[i]
        if(x==path[i+2]):
            if(x==path[i+4]):
                if(path[i+1]==path[i+5]):
                    changeind.append(i)
                
    if(len(changeind)==0):
        return path
                
    if len(changeind)>0:
        changed = []
        i=0
        while (i < len(path)):
            if i not in changeind:
                changed.append(path[i])
                i=i+1
            else:
                changed.append(path[i])
                changed.append(path[i+3])
                changed.append(path[i])
                changed.append(path[i+1])
                changeind.remove(i)
                i=i+6
        return changed
    
    
    
"""heuristic that sorts them by - out of the things it gets closer to, the mst cost of the remaining + dist to objective + mst cost of remaining without objective """


    
"""returns mst of the things we get closer to - farthest thing we got closer to"""
def heucalc(pt, distdict, remaining):
    
    if(len(remaining)>1):
        temp=[]
        for r in remaining:
            objdist = abs(r[0]-pt[0])+abs(r[1]-pt[1])
            if(objdist < distdict[r]):
                temp.append(r)
        if(len(temp)>0):
            if(len(temp)==1):
                tempmst = 0
            else:
                tempmst = findMST(temp)
            closest = closestwaypoint(pt,temp)
            heu = tempmst - farthest(pt, temp) + abs(closest[0]-pt[0]) + abs(closest[1]-pt[1])
            
        else:
            heu = findMST(remaining)

    else:
        heu = abs(remaining[0][0]-pt[0])+abs(remaining[0][1]-pt[1])
    
    return heu
    
    
"""trying one final heuristic"""

def astar_final(maze):
    """
    Runs A star for part 3 of the assignment in the case where there are four corner objectives.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
        """
    tbegin = time.time()
    frontier = deque()
    explored = deque()    
   
    location = namedtuple('location',['parent','dist','hdist'])
    
    routes = {}
    
    objectives = []
    
    mstree= {}
    
    finalpath=[maze.start]
    
    objectives.extend(maze.waypoints)
    obj = tuple(objectives)
    
    
    st = maze.start
    
    starth = farthest(st,objectives)
    
    routes[maze.start] =  location((-1,-1),0,findMST(objectives) )
    
    frontier.append(maze.start)
    explored.append(maze.start)
    
    #add the start point to the explored routes, pop it from the left of frontier and extend frontier on the right with neighbors to explore
    
    dad = frontier.popleft()
    kids = maze.neighbors(dad[0],dad[1])
    
    mstree={}
    obj = tuple(objectives)
    
    mstree[obj] = findMST(objectives)
    
    daddist = {}
    for o in objectives:
        daddist[o] = abs(o[0] - dad[0])+abs(o[1] - dad[1])
        
    for i in range(0,len(kids)):
        if kids[i] not in objectives:
            
            if(len(objectives)>1):
                far = heucalc(kids[i], daddist, objectives) 
            else:
                far = abs(objectives[0][0] - kids[i][0])+abs(objectives[0][1] - kids[i][1])
            routes[kids[i]] = location(dad,routes[dad].dist+1, far)
            frontier.append(kids[i])
            
        else:       
            obj = tuple(objectives)
            currmst = mstree[obj]
            rem = []
            rem.extend(objectives)
            rem.remove(kids[i])
            remm = tuple(rem)
            if(len(rem)>1):
                msremove = findMST(rem)
                mstree[remm] = msremove
            else:
                mstree[remm] = 0
            
            # mark this waypoint now only if removing it will result in a reduced MST
            
            if(msremove<currmst or currmst==0):
                finalpath.append(kids[i])
                st = kids[i]
                objectives.remove(kids[i])
                obj = tuple(objectives)
                
                if(len(objectives)>1):
                    mstree[obj] = findMST(objectives)
                else:
                    mstree[obj] = 0
                    
                mstcost = mstree[obj]
                routes.clear()
                routes[st] =  location((-1,-1),0,0 )
                frontier.clear()
                explored.clear()
                frontier.append(st)
                explored.append(st)
                break
            else:
                if(len(objectives)>1):
                    far = heucalc(kids[i], daddist, objectives) 
                else:
                    far = abs(objectives[0][0] - kids[i][0])+abs(objectives[0][1] - kids[i][1])
                routes[kids[i]] = location(dad,routes[dad].dist+1, far)
                frontier.append(kids[i])
            
    #create infinite loop that will break only when there are no more remaining objectives
     
    while(len(objectives)!=0):  
        #this part to pop the one with smallest hdist
        h=[]
        for fro in frontier:
            heapq.heappush(h,(routes[fro].hdist, fro))
#         if(len(objectives)==1):
#             print(h)
#             print(frontier)
        dad=heapq.heappop(h)[1]  
        frontier.remove(dad)
        kids = maze.neighbors(dad[0],dad[1])
        explored.append(dad)
        
        daddist = {}
        for o in objectives:
            daddist[o] = abs(o[0] - dad[0])+abs(o[1] - dad[1])
        
        
        for i in range(0,len(kids)):
            
            if(len(objectives)>1):
                far = heucalc(kids[i], daddist, objectives)
            else:
                far = abs(objectives[0][0] - kids[i][0])+abs(objectives[0][1] - kids[i][1]) + routes[dad].dist+1
            
            
            
            if (kids[i] in routes):
                # this means that this place has been seen before and currently is in frontier or explored so update its details
                #a waypoint can never be here as the loop would break when the below else statement adds the waypoint
                if(routes[kids[i]].dist > routes[dad].dist+1):
                    routes[kids[i]]=routes[kids[i]]._replace(parent= dad)                  
                    routes[kids[i]]=routes[kids[i]]._replace(dist= routes[dad].dist+1)
                    routes[kids[i]]=routes[kids[i]]._replace(hdist= far)
                                       
            else:
               # if this place has been reached for the first time make note of it
                dreach = routes[dad].dist+1 
                routes[kids[i]] = location(dad,dreach, far )
                # if it isnt the final ending then add this to the frontier
#                 if(len(objectives)==1):
#                     print(frontier)
                if kids[i] not in objectives:
                    frontier.append(kids[i])
                else:
                    # if this is final dest then add it to explored and break out
                    cell = kids[i]
                    explored.append(cell)
                    path = [cell]
                    while(cell!=st):
                        path.append(routes[cell].parent)
                        cell = routes[cell].parent
                    path.remove(st)
                    path.reverse()
                    finalpath.extend(path)
                    #now reset everything to find math to next waypoint from this waypoint
                    st = kids[i]
                    objectives.remove(kids[i])
                    obj = tuple(objectives)
                    if(len(objectives)>1):
                        mstree[obj] = findMST(objectives)
                    else:
                        mstree[obj] = 0
                    mstcost = mstree[obj]
                    routes.clear()
                    if(len(objectives)>0):
                        if(len(objectives)==1):
                            far = abs(objectives[0][0] - kids[i][0])+abs(objectives[0][1] - kids[i][1])
                            print("path till last objective ", finalpath)
                        else:
                            far = findMST(objectives)
                        routes[st] =  location((-1,-1),0,far )
                    frontier.clear()
                    explored.clear()
                    frontier.append(st)
                    explored.append(st)
#                     print("check point found = ", st)
                    break
   
                   
     
    tend = time.time()    
    print(tend-tbegin, "time taken")
    print(finalpath)
    
    return editpath(finalpath)

