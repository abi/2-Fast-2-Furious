import sys

sys.setrecursionlimit(100000)
"""
   
   Tarjan's algorithm and topological sorting implementation in Python
   
   by Paul Harrison
   
   Public domain, do with it as you will

"""

def strongly_connected_components(graph):
    """ Find the strongly connected components in a graph using
        Tarjan's algorithm.
        
        graph should be a dictionary mapping node names to
        lists of successor nodes.
        """
    
    result = [ ]
    stack = [ ]
    low = { }
        
    def visit(node):
        if node in low: return
	
	num = len(low)
        low[node] = num
        stack_pos = len(stack)
        stack.append(node)
	
        for successor in graph[node]:
            visit(successor)
            low[node] = min(low[node], low[successor])
        
        if num == low[node]:
	    component = tuple(stack[stack_pos:])
            del stack[stack_pos:]
            result.append(component)
	    for item in component:
	        low[item] = len(graph)
    
    for node in graph:
        visit(node)
    
    return result


def topological_sort(graph):
    count = { }
    for node in graph:
        count[node] = 0
    for node in graph:
        for successor in graph[node]:
            count[successor] += 1

    ready = [ node for node in graph if count[node] == 0 ]
    
    result = [ ]
    while ready:
        node = ready.pop(-1)
        result.append(node)
        
        for successor in graph[node]:
            count[successor] -= 1
            if count[successor] == 0:
                ready.append(successor)
    
    return result


def robust_topological_sort(graph):
    """ First identify strongly connected components,
        then perform a topological sort on these components. """

    components = strongly_connected_components(graph)

    node_component = { }
    for component in components:
        for node in component:
            node_component[node] = component

    component_graph = { }
    for component in components:
        component_graph[component] = [ ]
    
    for node in graph:
        node_c = node_component[node]
        for successor in graph[node]:
            successor_c = node_component[successor]
            if node_c != successor_c:
                component_graph[node_c].append(successor_c) 

    return topological_sort(component_graph)


if __name__ == '__main__':
    somefile = ([l[:-1] for l in file(sys.argv[1])])[2:]
    graf = {}
    biggest = 0
    for x in somefile:
      vals = [int(s) for s in x.split(" ") if s != ""]
      if vals[0] not in graf:
        graf[vals[0]] = []
      graf[vals[0]].append(vals[1])
      if vals[0] > biggest: biggest = vals[0]
      if vals[1] > biggest: biggest = vals[1]

    for x in range(1, biggest+1):
      if x not in graf: graf[x] = []


    print "\t".join([str(x) for x in (sorted([len(x) for x in
      robust_topological_sort(graf)])[::-1])][:5]),
