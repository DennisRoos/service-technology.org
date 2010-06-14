/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

/**
 * A set of {@link DNode}s that represents a branching process.
 * 
 * Firstly, a {@link DNodeSet} presents a collection of {@link DNode}s with
 * index structures for accessing specific nodes in the set. Specifically the
 * 'maximal' nodes that have no successor nodes.
 * 
 * Secondly, a {@link DNodeSet} is interpreted as an acyclic Petri net that
 * represents a branching process. This branching process can be extended at
 * its 'maximal' nodes by firing events.  
 * 
 * @author Dirk Fahland
 */
public class DNodeSet {
	
  /**
   * This shall become an efficient data structure for storing and indexing
   * sets of {@link DNode}s.
   * 
   * TODO implement
   * 
   * @author Dirk Fahland
   */
	public static class DNodeSetElement extends java.util.LinkedList<DNode>  {

		private static final long serialVersionUID = -9020905669586738898L;

	}

	/**
	 * nodes without successor
	 */
	public DNodeSetElement maxNodes = null;
	
	/**
	 * The conditions in this {@link DNodeSet} that are maximal before the first
	 * event is fired in this set.
	 */
	public DNodeSetElement initialConditions = null;
	
	public DNodeSetElement allConditions = null;
	public DNodeSetElement allEvents = null;
	
	/**
	 * Auxiliary representation of {@link DNodeSet#initialConditions} that is
	 * used in {@link DNodeBP} for finding cut-off events.
	 */
	public DNode[] initialCut = null;
	
	/**
	 * Initialize new {@link DNodeSet}
	 * @param num  initial number of nodes in this set
	 */
	public DNodeSet (int num) {
		maxNodes = new DNodeSetElement();	// initialize hash-table size
		allConditions = new DNodeSetElement();
		allEvents = new DNodeSetElement();
	}

	/**
	 * Set the {@link DNodeSet#initialConditions} of this {@link DNodeSet},
	 * this are not its minimal nodes but the conditions that are maximal
	 * before the first event is fired in this set.
	 * 
	 * This method must be called before {@link DNodeSet#fire(DNode, DNode[])}
	 * or {@link DNodeSet#fire(DNode[], DNode[])} is called. 
	 */
	public void setInitialConditions () {
		initialConditions = new DNodeSetElement();	// initialize hash-table size
		// all maximal nodes of the branching process before firing any event
		// represent the initial conditions of this branching process, store them
		initialConditions.addAll(maxNodes);
		
		// and translate into the array for detecting cut-off events 
		initialCut = new DNode[initialConditions.size()];
		int i=0;
		for (DNode b : initialConditions)
			initialCut[i++] = b;
		DNode.sortIDs(initialCut);
	}
	
	/**
	 * Add the nodes of the initial process to the data structures that contain
	 * all nodes of the process.
	 * 
   * This method must be called before {@link DNodeSet#fire(DNode, DNode[])}
   * or {@link DNodeSet#fire(DNode[], DNode[])} is called. 
	 */
	public void addInitialNodesToAllNodes() {
	  HashSet<DNode> seen = new HashSet<DNode>();
	  LinkedList<DNode> queue = new LinkedList<DNode>();
	  // add all initial nodes and all their transitive predecessors to the
	  // set of all nodes of this set
	  queue.addAll(initialConditions);
	  while (!queue.isEmpty()) {
	    DNode d = queue.removeFirst();
	    seen.add(d);
	    
	    if (d.isEvent) allEvents.add(d);
	    //if (!d.isEvent && !initialConditions.contains(d)) allConditions.add(d);
	    
	    if (d.pre != null)
	      for (DNode pre : d.pre) {
	        if (!seen.contains(pre)) queue.add(pre);
	      }
	  }
	}
	
	/**
	 * Add a new node to this {@link DNodeSet} (without firing an event) and update
	 * all internal data structures. This method is used to construct a
	 * {@link DNodeSet} from scratch or by copying.
	 * 
	 * @param newNode
	 */
	public void add(DNode newNode) {
		
		assert newNode != null : "Error! Trying to insert null node into DNodeSet";
		
		// the predecessors of newNode are no longer maxNodes in this set
		// remove all nodes from this set's maxNodes that are in the pre-set of newNode
		DNodeSetElement toRemove = new DNodeSetElement();
		for (DNode d : maxNodes) {
			if (newNode.preContains_identity(d)) {
				toRemove.add(d);
			}
		}
		maxNodes.removeAll(toRemove);
		// and add the newNode to maxNodes
		maxNodes.add(newNode);
		
		// and store the newNode as event or condition
		if (newNode.isEvent) allEvents.add(newNode);
		else allConditions.add(newNode);
	}
	
	/**
	 * @return all nodes in this {@link DNodeSet} by backwards search from its
	 * {@link DNodeSet#maxNodes}
	 */
	public HashSet<DNode> getAllNodes() {
		LinkedList<DNode> queue = new LinkedList<DNode>(maxNodes);
		HashSet<DNode> allNodes = new HashSet<DNode>();
		while (!queue.isEmpty()) {
			DNode n = queue.removeFirst();
			if (n == null) continue;
			allNodes.add(n);
			for (int i=0; i<n.pre.length; i++) {
			  if (!allNodes.contains(n.pre[i]))
			    queue.add(n.pre[i]);
			}
		}
		return allNodes;
	}
	
	/**
	 * @return all conditions
	 */
	public DNodeSetElement getAllConditions() {
		/*
		LinkedList<DNode> queue = new LinkedList<DNode>(maxNodes);
		HashSet<DNode> allNodes = new HashSet<DNode>();
		while (!queue.isEmpty()) {
			DNode n = queue.removeFirst();
			if (n == null) continue;
			if (!n.isEvent)	// only add conditions
				allNodes.add(n);
			queue.addAll(Arrays.asList(n.pre));
		}
		*/
		return allConditions;
	}

  /**
   * @return all events
   */
	public DNodeSetElement getAllEvents() {
		return allEvents;
	}
	
	/**
	 *  This is a private field that gets updated in every call of
	 *  {@link DNodeSet#getPrimeCut(DNode, boolean)}
	 *  The field contains the number of predecessor events of the
	 *  parameter 'event', not including 'event' itself.
	 */
	public int getPrimeConfiguration_size;
	
	/**
   *  This is a public field that gets updated in every call of
   *  {@link DNodeSet#getPrimeCut(DNode, boolean)}
   *  if parameter 'collectPredecessors' is set to <code>true</code>.
   *
   *  The field contains all predecessor events of the parameter 'event',
   *  unsorted and iterable.
	 */
	public HashSet<DNode> getPrimeConfiguration;
	
	/**
	 * Compute the prime cut and all predecessor events of the given event. 
	 * 
	 * @param event
	 *           the event for which the prime cut and
	 *           the predecessors shall be computed
	 * @param collectPredecessors
	 *           a list that gets filled with all predecessor events of
	 *           <code>event</code>. Can be <code>null</code>, then the
	 *           predecessors of <code>event</code> are not stored.
	 * @param configIncludeEvent
	 *           whether the prime configuration shall contain 'event'
	 * @return
	 *         a sorted array of {@link DNode}s representing the prime cut of
	 *         <code>event</code>
	 */
	public DNode[] getPrimeCut(DNode event, boolean collectPredecessors, boolean configIncludeEvent) {
		
		// the set of post-conditions of all predecessors of 'event', this set gets
		// updated as the predecessors of 'event' are discovered
		HashSet<DNode> postConditions = new HashSet<DNode>();

    // find all predecessor events of 'event' and their post-conditions
    // by backwards breadth-first search from 'event'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.add(event);
    
		// the events of the prime configuration of 'event'
		HashSet<DNode> primeConfiguration = new HashSet<DNode>();
		
		// conditions with a post-event that is a predecessor of 'event', these
		// conditions cannot be in the prime cut of 'event'
		HashSet<DNode> consumedConditions = new HashSet<DNode>();

		// run breadth-first search
		while (!queue.isEmpty()) {
			
			DNode first = queue.removeFirst();
	    primeConfiguration.add(first);
			
			// add all post-conditions to the prime-cut that are not consumed by successors
			for (DNode post : first.post) {
				if (!consumedConditions.contains(post))
					postConditions.add(post);
			}
			
			for (DNode preCondition : first.pre)
			{
				// all pre-conditions of this node are consumed
				consumedConditions.add(preCondition);
				
				// add all predecessor events to the queue
				for (DNode preEvent : preCondition.pre) {
					if (!primeConfiguration.contains(preEvent)) {
					  // only add to the queue if not queued already
						queue.add(preEvent);
					}
				} // all predecessor events of preCondition
			} // all preconditions of the current event
		} // breadth-first search

		// the initialConditions of this braching process potentially belong
		// to the prime cut of this 'event'
    postConditions.addAll(initialConditions);
    
		// as we are doing simple breadth-first search, we may have added
		// some conditions to the post-conditions that turned out later to
		// be consumed by other events, remove these from the postConditions
		postConditions.removeAll(consumedConditions);
		
		// transform the prime cut into a sorted array 
		DNode[] primeCut = new DNode[postConditions.size()];
		postConditions.toArray(primeCut);
		DNode.sortIDs(primeCut);

		// remove the 'event' from the prime configuration if desired
    if (!configIncludeEvent) primeConfiguration.remove(event);
		getPrimeConfiguration_size = primeConfiguration.size();
		  
		if (collectPredecessors) {
		  /*
  		getPrimeCut_predecessors = new DNode[primeConfiguration.size()];
  		primeConfiguration.toArray(getPrimeCut_predecessors);
  		DNode.sortIDs(getPrimeCut_predecessors);
  		*/
      getPrimeConfiguration = primeConfiguration;

		} else {
		  //getPrimeCut_predecessors = null;
		  getPrimeConfiguration = null;
		}
		
		return primeCut;
	}
	
  public int getConfigSize(DNode[] extensionLocation) {
    
    // find all predecessor events of 'extensionLocation'
    // by backwards breadth-first search from 'extensionLocation'
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (int i=0;i<extensionLocation.length;i++)
      queue.add(extensionLocation[i]);
    
    // the events of the configuration before 'extensionLocation'
    HashSet<DNode> configuration = new HashSet<DNode>();
    
    // run breadth-first search
    while (!queue.isEmpty()) {
      
      DNode first = queue.removeFirst();
      configuration.add(first);
      
      for (DNode preCondition : first.pre)
      {
        // add all predecessor events to the queue
        for (DNode preEvent : preCondition.pre) {
          if (!configuration.contains(preEvent)) {
            // only add to the queue if not queued already
            queue.add(preEvent);
          }
        } // all predecessor events of preCondition
      } // all preconditions of the current event
    } // breadth-first search

    return configuration.size();
  }
	
	/**
	 * Fire ocletEvent in this set at the given location. This
	 * appends a new event with the same id as <code>ocletEvent</code> to
	 * this set, and corresponding post-conditions.
	 * 
	 * @param ocletEvent
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode ocletEvent, DNode[] fireLocation) {

		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvent.id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// remember the oclet event that caused this node, required for determining
		// cutOff events
		newEvent.causedBy = new int[1];
		newEvent.causedBy[0] = ocletEvent.globalId;
		
		// set post-node of conditions
		for (DNode preNode : fireLocation)	
			preNode.addPostNode(newEvent);
		
		// instantiate the post-conditions of the oclet event
		DNode postConditions[] = new DNode[ocletEvent.post.length];
		for (int i=0; i<postConditions.length; i++) {
			postConditions[i] = new DNode(ocletEvent.post[i].id, 1);
			allConditions.add(postConditions[i]);

			// predecessor is the new event
			postConditions[i].pre[0] = newEvent;

			assert postConditions[i] != null : "Error, created null post-condition!";
		}
		
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		// add post-set of newEvent to the maxNodes
		maxNodes.addAll(Arrays.asList(postConditions));
		return postConditions;
	}
	
	/**
	 * Synchronously fire a set of ocletEvents in this set at
	 * the given location. This appends ONE new event with the
	 * same id as all ocletEvents to this set, and corresponding
	 * post-conditions for all the given ocletEvents 
	 * 
	 * @param ocletEvents
	 * @param fireLocation
	 * 
	 * @return an array with the new post-conditions
	 */
	public DNode[] fire(DNode[] ocletEvents, DNode[] fireLocation) {
		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvents[0].id, fireLocation);
		newEvent.isEvent = true;
		allEvents.add(newEvent);
		
		// set post-node of conditions
		for (DNode preNode : fireLocation)
			preNode.addPostNode(newEvent);

    // remember the oclet event that caused this node, required for determining
    // cutOff events
		newEvent.causedBy = new int[ocletEvents.length];
		
		// create a sorted list of all post-conditions of all events
		DNode.SortedLinearList list = new DNode.SortedLinearList();
		for (int i=0; i<ocletEvents.length; i++) {
			
	    // remember the oclet event that caused this node, required for determining
	    // cutOff events
			newEvent.causedBy[i] = ocletEvents[i].globalId;
			
			for (int j=0; j<ocletEvents[i].post.length; j++)
				list = list.add(ocletEvents[i].post[j]);
		}
		
		// remove duplicate post-conditions (these are merged)
		list.removeDuplicateIDs();
		
		// instantiate the remaining post-conditions of the new oclet event
		DNode postConditions[] = new DNode[list.length()];
		int i = 0;
		for (DNode post : list) {
			postConditions[i] = new DNode(post.id, 1);
			allConditions.add(postConditions[i]);

			// predecessor is the new event
			postConditions[i].pre[0] = newEvent;
			
			assert postConditions[i] != null : "Error, created null post-condition!";
			
			i++;
		}
		
		// set post-nodes of the event
		newEvent.post = postConditions;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		// add post-set of newEvent to the maxNodes
		maxNodes.addAll(Arrays.asList(postConditions));
		return postConditions;
	}
	
	/**
	 * Compute the maximal distance of each node from the {@link #initialConditions}
	 * of this branching process. The result is returned as a {@link HashMap} from
	 * {@link DNode}s to shorts. The depth of a node can be used see whether two 
	 * events of this branching process are direct successor events. In this case
	 * they are separated by only one condition and the difference of their depths
	 * is exactly 2. If the difference is larger, then there are more events in
	 * between, even if there is a direct dependency between the two events via a
	 * condition.
	 * 
	 * @return
	 */
	
  public HashMap<DNode, Short> getDepthMap() {
    // initialize depth map
    HashMap<DNode, Short> depth = new HashMap<DNode, Short>();
    
    // then run a breadth-first search beginning at the initial conditions
    LinkedList<DNode> queue = new LinkedList<DNode>();
    for (DNode d : this.initialConditions) {
      depth.put(d, (short)0); // begin with depth 0
      queue.addLast(d);
    }
    
    while (!queue.isEmpty()) {
      DNode d = queue.remove();
      if (d.post == null) continue;
      for (DNode dPost : d.post) {
        // for each successor, take the maximum of the depths + 1
        Short dPostDepth = depth.get(dPost);
        if (dPostDepth == null) depth.put(dPost, (short)(depth.get(d)+1));
        else {
          if (dPostDepth < depth.get(d)) dPostDepth = (short)(depth.get(d)+1);
          depth.put(dPost, (short)(depth.get(d)+1));
        }
        queue.addLast(dPost);
      }
    }
    
    return depth;
  }
	
	/*
	public void fire(LinkedList<DNode> ocletEvents, DNode[] fireLocation) {
		// instantiate the oclet event
		DNode newEvent = new DNode(ocletEvents.get(0).id, fireLocation);
		newEvent.isEvent = true;
		
		// remove pre-set of newEvent from the maxNodes
		maxNodes.removeAll(Arrays.asList(fireLocation));
		
		// collect all post conditions of all events
		DNode[] mergedPostConditions = new DNode[0];
		for (DNode ocletEvent : ocletEvents)
			mergedPostConditions = DNode.merge(null, mergedPostConditions, ocletEvent.post);
		
		// instantiate one condition for each ID and add it
		// as post-condition of newEvent to the maxNodes
		int lastID = -1;
		for (int i=0; i<mergedPostConditions.length; i++) {
			if (mergedPostConditions[i].id == lastID)
				continue;
			DNode postCondition = new DNode(mergedPostConditions[i].id, 1);
			// predecessor is the new event
			postCondition.pre[0] = newEvent;
			maxNodes.add(postCondition);
		}
	}
	*/
	
	/*
	public boolean contains(DNode n) {
		
		if (maxNodes[n.id] == null)
			return false;
		else {
			return contents[n.id].contains(n);
		}
	}
	*/

	/**
	 * Option: Print nodes of the branching process that are removed due to
	 * anti-oclets. 
	 */
	public static boolean option_printAnti = true;
	
	/**
	 * Create a GraphViz' dot representation of this branching process.
	 * 
	 * @return 
	 */
	public String toDot () {
		StringBuilder b = new StringBuilder();
		b.append("digraph BP {\n");
		
		// standard style for nodes and edges
		b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
		b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
		b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

		//String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
		String cutOffFillString = "fillcolor=gold";
		String antiFillString = "fillcolor=red";
		String impliedFillString = "fillcolor=violet";
		String hiddenFillString = "fillcolor=grey";
		
		// first print all conditions
		b.append("\n\n");
		b.append("node [shape=circle];\n");
		for (DNode n : getAllConditions()) {
			
			if (!option_printAnti && n.isAnti)
				continue;
			/* - print current marking
			if (cutNodes.contains(n))
				b.append("  c"+n.localId+" ["+tokenFillString+"]\n");
			else
			*/
			if (n.isAnti && n.isHot)
				b.append("  c"+n.globalId+" ["+antiFillString+"]\n");
      else if (n.isCutOff)
        b.append("  c"+n.globalId+" ["+cutOffFillString+"]\n");
			else
				b.append("  c"+n.globalId+" []\n");
			
			String auxLabel = "";
				
			b.append("  c"+n.globalId+"_l [shape=none];\n");
			b.append("  c"+n.globalId+"_l -> c"+n.globalId+" [headlabel=\""+n+" "+auxLabel+"\"]\n");
		}

    // then print all events
		b.append("\n\n");
		b.append("node [shape=box];\n");
		for (DNode n : getAllEvents()) {
			
			if (!option_printAnti && n.isAnti)
				continue;
			
			
			if (n.isAnti && n.isHot)
				b.append("  e"+n.globalId+" ["+antiFillString+"]\n");
			else if (n.isAnti && !n.isHot)
        b.append("  e"+n.globalId+" ["+hiddenFillString+"]\n");
			else if (n.isImplied)
        b.append("  e"+n.globalId+" ["+impliedFillString+"]\n");
			else if (n.isCutOff)
				b.append("  e"+n.globalId+" ["+cutOffFillString+"]\n");
			else
				b.append("  e"+n.globalId+" []\n");
			
      String auxLabel = "";
      
			b.append("  e"+n.globalId+"_l [shape=none];\n");
			b.append("  e"+n.globalId+"_l -> e"+n.globalId+" [headlabel=\""+n+" "+auxLabel+"\"]\n");
		}
		
		/*
		b.append("\n\n");
		b.append(" subgraph cluster1\n");
		b.append(" {\n  ");
		for (CNode n : nodes) {
			if (n.isEvent) b.append("e"+n.localId+" e"+n.localId+"_l ");
			else b.append("c"+n.localId+" c"+n.localId+"_l ");
		}
		b.append("\n  label=\"\"\n");
		b.append(" }\n");
		*/
		
		// finally, print all edges
		b.append("\n\n");
		b.append(" edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
		for (DNode n : allConditions) {
			String prefix = n.isEvent ? "e" : "c";
			for (int i=0; i<n.pre.length; i++) {
				if (n.pre[i] == null) continue;
				if (!option_printAnti && n.isAnti) continue;
				
				if (n.pre[i].isEvent)
					b.append("  e"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0]\n");
				else
					b.append("  c"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0]\n");
			}
		}
    for (DNode n : allEvents) {
      String prefix = n.isEvent ? "e" : "c";
      for (int i=0; i<n.pre.length; i++) {
        if (n.pre[i] == null) continue;
        if (!option_printAnti && n.isAnti) continue;
        
        if (n.pre[i].isEvent)
          b.append("  e"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0]\n");
        else
          b.append("  c"+n.pre[i].globalId+" -> "+prefix+n.globalId+" [weight=10000.0]\n");
      }
    }

		
		b.append("}");
		return b.toString();
	}
}
