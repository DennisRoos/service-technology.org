package hub.top.mia;

import hub.top.petrinet.Place;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.BitSet;

public class StateTree {
	/** root of the state tree **/
	private SafeMarking root;
	private MarkingComparator comp;
	private SafeMarking successor;

	/** set with all forward and backward successors **/
	private HashSet<SafeMarking> successors;

	/** reference to sorted places of nets No and Nn, needed for converting from SafeMarking to Marking representation **/
	private Place[] sortedPlaces_No, sortedPlaces_Nn;
	
	/** some constants **/
	public static final int INTERSECTION_EMPTY = 0;
	public static final int INTERSECTION_SINGLE = 1;
	public static final int INTERSECTION_MORE = 2;

	public StateTree() {
		root = new SafeMarking();
		root.setDepth(0);
		comp = new MarkingComparator();
	}

	/**
	 * find smallest ancestor that contains cluster
	 * 
	 * @param current
	 * @param cluster
	 */
	public void findSuccessor(SafeMarking current, SafeMarking cluster) {
		HashSet<SafeMarking> fathers = current.getFathers();

		System.out.println("Current: " + current + " " + cluster);
		if (fathers.isEmpty()) {
			System.out.println("Fathers is empty! ");
			return;
		}

		for (SafeMarking father : fathers) {
			if (father.contains(cluster) && comp.compare(father, successor) > 0
					&& !cluster.equals(father)) {
				successor = father;
				System.out.println("Current: " + successor + " " + cluster);
				findSuccessor(father, cluster);
			}
		}
	}

	/**
	 * find smallest ancestor of leaf that contains current cluster
	 * 
	 * @param current
	 * @param cluster
	 */
	public void findSuccessors(SafeMarking current, SafeMarking cluster) {
		HashSet<SafeMarking> fathers = current.getFathers();
		boolean found = false;

		for (SafeMarking father : fathers) {
			if (father.contains(cluster)) {
				findSuccessors(father, cluster);
				found = true;
			}
		}

		if (!found) {
			successors.add(current);
		}
	}

	/**
	 * build state tree from global_states Step 1: compute intersections between
	 * all pairs of states - gives all inner nodes Step 2: sort inner nodes in
	 * descending order Step 3: traverse inner nodes in descending order and add
	 * link between each smallest ancestor of each leaf and current cluster
	 * 
	 * @param global_states
	 */
	public void buildTree(ArrayList<SafeMarking> global_states) {
		// compute all intersections
		ArrayList<SafeMarking> clusters = new ArrayList<SafeMarking>();
		for (int i = 0; i < global_states.size(); i++) {
			for (int j = 0; j < i; j++) {
				SafeMarking inner = global_states.get(i).intersect(
						global_states.get(j));

				// check if inner is already
				SafeMarking new_inner = null;
				for (SafeMarking cluster : clusters) {
					if (cluster.equals(inner)) {
						new_inner = cluster;
					}
				}

				// if inner is already in the clusters set, add leafs new global
				// states
				if (new_inner != null) {
					new_inner.addLeaf(global_states.get(i));
					new_inner.addLeaf(global_states.get(j));
				} else {
					// add new inner to the cluster set
					inner.addLeaf(global_states.get(i));
					inner.addLeaf(global_states.get(j));
					clusters.add(inner);
				}
			}
		}

		// sort clusters in descending order
		SafeMarking[] arrayClusters = new SafeMarking[clusters.size()];
		MarkingComparator comp = new MarkingComparator();
		clusters.toArray(arrayClusters);
		Arrays.sort(arrayClusters, comp);
		successors = new HashSet<SafeMarking>();

		// hashset with all successors, needed at the end to merge leaf sets
		HashSet<SafeMarking> allSuccessors = new HashSet<SafeMarking>();

		// iterate over all clusters in descending order
		for (SafeMarking cluster : arrayClusters) {

			// System.out.println("Current cluster: " + cluster);
			HashSet<SafeMarking> leafs = cluster.getLeafs();

			// iterate over all leafs
			for (SafeMarking leaf : leafs) {

				// System.out.println("Leaf: " + leaf);
				// get fathers of leaf
				HashSet<SafeMarking> fathers = leaf.getFathers();

				if (fathers.isEmpty()) {
					// System.out.println("Leaf Link: " + leaf + " " + cluster);
					leaf.addFather(cluster);
					cluster.addChild(leaf);
				} else {
					// compute intersection between all fathers of all leafs
					successors.clear();
					findSuccessors(leaf, cluster);

					if (successors.isEmpty()) {
						// add link between leaf and cluster
						leaf.addFather(cluster);
						cluster.addChild(leaf);
					} else {
						for (SafeMarking successor : successors) {
							successor.addFather(cluster);
							cluster.addChild(successor);
							allSuccessors.add(successor);
						}
					}
				}
			}

			// merge leafs of all successors with leafset of current node
			for (SafeMarking node : allSuccessors) {
				cluster.mergeLeafs(node);
			}
			allSuccessors.clear();
		}

		// check if we have empty intersection already
		if (arrayClusters.length > 0
				&& arrayClusters[arrayClusters.length - 1].isEmpty()) {
			root = arrayClusters[arrayClusters.length - 1];
		} else {
			// add clusters with no fathers as children of root
			for (SafeMarking cluster : arrayClusters) {
				if (cluster.getFathers().isEmpty()) {
					root.addChild(cluster);
					cluster.addFather(root);
				}
			}
		}

		minimizeTree();
	}

	/**
	 * compute intersection between leaf set of cluster1 and leaf set of cluster
	 * 2
	 * 
	 * @param cluster1
	 * @param cluster2
	 */
	private HashSet<SafeMarking> computeIntersection(
			HashSet<SafeMarking> leafs1, HashSet<SafeMarking> leafs2) {
		HashSet<SafeMarking> result = new HashSet<SafeMarking>();

		for (SafeMarking leaf1 : leafs1) {
			if (leafs2.contains(leaf1)) {
				result.add(leaf1);
			}
		}

		return result;
	}

	/**
	 * compute intersection between all subsets of clusters array
	 * 
	 * @param clusters
	 */
	private void computeIntersections(ArrayList<SafeMarking> clusters) {
		// System.out.println("Processing Clusters...");

		// generate all subsets of clusters
		int clustersSize = clusters.size();
		int noBits = 1;
		BitSet generator = new BitSet();
		generator.set(0);
		HashSet<SafeMarking> validClusters = new HashSet<SafeMarking>();
		HashSet<SafeMarking> result = new HashSet<SafeMarking>();

		while (!generator.get(clustersSize)) {
			// System.out.println("Generator: " + generator);
			// System.out.println("Processing subsets...");

			boolean isFirst = true;
			for (int i = 0; i < noBits; i++) {
				if (generator.get(i) == true) {
					SafeMarking cluster = clusters.get(i);

					// discard combination if one of the sides of the cluster is
					// empty
					if (cluster.isSideEmpty())
						break;

					if (isFirst) {
						isFirst = false;
						result.addAll(cluster.getLeafs());
					} else {
						result = computeIntersection(result, cluster.getLeafs());
					}
					// System.out.println(cluster);
					validClusters.add(cluster);
				}
			}

			if (result.size() == StateTree.INTERSECTION_SINGLE) {
				// we have a candidate!
				System.out.println("Found combination with generator: "
						+ generator);
				for (SafeMarking cluster : validClusters) {
					cluster.setDecomposed(true);
					System.out.println(cluster + " " + cluster.hashCode());

					HashSet<SafeMarking> subcone = cluster.getSubcone();
					
					// convert to marking notation
					HashSet<Marking> subconeOld = new HashSet<Marking>();
					HashSet<Marking> subconeNew = new HashSet<Marking>();
					
					for (SafeMarking node : subcone) {
						BitSet b_Old = node.getM_Old();
						BitSet b_New = node.getM_New();
						
						Marking m_Old = new Marking(b_Old, sortedPlaces_No);
						Marking m_New = new Marking(b_Old, sortedPlaces_Nn);
						
						//subconeOld = 
					}
					
				}

				// compute subcones 
				
				
				// TODO: implement cone property check
				// compute subcones of cluster
				// convert elements of subcones to marking of a petri net
				// check strongly connected components criteria

				System.out.println();
			}

			// System.out.println("Common intersection:");
			// for (SafeMarking node : result) {
			// System.out.println(node);
			// }
			// System.out.println();

			// generate next subset, simulate addition with 1
			int i = 0;
			while (generator.get(i) == true) {
				i++;
			}

			// if all noBits are set, increase noBits
			if (i == noBits) {
				generator.clear();
				noBits++;
			}
			generator.set(i);

			// reset clusters
			validClusters.clear();
			result.clear();
		}
		System.out.println();
	}

	/**
	 * breadth first search traversal of the state tree
	 */
	public void traverseTree() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);

		do {
			SafeMarking current = queue.poll();

			// output current node
			System.out.println("Current: " + current + " d:"
					+ current.getDepth());

			// push children in the queue
			HashSet<SafeMarking> children = current.getChildren();

			// System.out.println("Children of current:");
			for (SafeMarking child : children) {
				queue.addLast(child);
			}
		} while (!queue.isEmpty());
	}

	/**
	 * compute intersections between all possible subtrees having the same depth
	 * if intersection is a single element - mark as valid candidate for
	 * combination of independent branches
	 */
	public void computeCombinations() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);

		// array with all clusters having same depth
		ArrayList<SafeMarking> currentClusters = new ArrayList<SafeMarking>();
		int currentDepth = 0;
		do {
			SafeMarking current = queue.poll();

			// check if depth is increased, if so, compute intersections
			if (currentDepth < current.getDepth()) {
				// compute intersections between clusters
				computeIntersections(currentClusters);

				// reset clusters array
				currentClusters.clear();
				currentDepth++;
			}

			// add current cluster to list of cluster having depth currentDepth
			if (!currentClusters.contains(current)) {
				currentClusters.add(current);
			}

			// push children in the queue
			HashSet<SafeMarking> children = current.getChildren();
			// System.out.println("Children of current:");
			for (SafeMarking child : children) {
				queue.addLast(child);
			}

		} while (!queue.isEmpty());

		// call compute intersections for last set of clusters
		computeIntersections(currentClusters);
	}

	/**
	 * traverse tree in breadth first search and compute subtrees having the
	 * same set of leafs traverse the tree for the second time in order to
	 * remove duplicates
	 */
	public void minimizeTree() {
		LinkedList<SafeMarking> queue = new LinkedList<SafeMarking>();
		queue.add(root);

		// array with all clusters having same depth
		ArrayList<SafeMarking> currentClusters = new ArrayList<SafeMarking>();
		int currentDepth = 0;
		root.setDepth(0);

		do {
			SafeMarking current = queue.poll();

			// check if depth is increased, if so, compute intersections
			if (currentDepth < current.getDepth()) {
				// compute intersections between clusters
				removeDuplicates(currentClusters);

				// reset clusters array
				currentClusters.clear();
				currentDepth++;
			}
			currentClusters.add(current);

			HashSet<SafeMarking> children = current.getChildren();
			for (SafeMarking child : children) {
				child.setDepth(current.getDepth() + 1);
				queue.add(child);
			}

		} while (!queue.isEmpty());

		// second breath first search traversal - remove duplicates
		queue.add(root);
		do {
			SafeMarking current = queue.poll();

			// add children to queue only if not marked for deletion
			HashSet<SafeMarking> children = (HashSet<SafeMarking>) current
					.getChildren().clone();
			for (SafeMarking child : children) {
				if (child.isRemoved()) {
					// remove link between current and child
					// System.out.println("Removing: " + child);
					current.removeChild(child);
					child.removeFather(current);
				} else {
					queue.add(child);
				}
			}

		} while (!queue.isEmpty());
	}

	/**
	 * keep only one subtree for each subcone
	 * 
	 * @param currentClusters
	 */
	private void removeDuplicates(ArrayList<SafeMarking> currentClusters) {
		LexicMarkingComparator lexComp = new LexicMarkingComparator();
		for (int i = 0; i < currentClusters.size(); i++) {
			SafeMarking ci = currentClusters.get(i);
			for (int j = 0; j < currentClusters.size(); j++) {
				SafeMarking cj = currentClusters.get(j);

				if (ci.isEquivalent(cj) && lexComp.compare(ci, cj) > 0) {
					// System.out.println("remove! " + cj + " because of " + ci
					// + " ");
					cj.setRemoved(true);
				}
			}
		}
	}

	/**
	 * breadth first search traversal of the state tree if direct successors of
	 * tree can be further refined, go further, otherwise add it to the final
	 * result output should be distributed migration relation
	 */
	public void gatherResult() {
		// TODO: implement gather results
		// traverse tree in breadth first search order
		// if a child can be further refined,
	}

	public void setSortedPlaces_No(Place[] sortedPlaces_No) {
		this.sortedPlaces_No = sortedPlaces_No;
	}

	public Place[] getSortedPlaces_No() {
		return sortedPlaces_No;
	}

	public void setSortedPlaces_Nn(Place[] sortedPlaces_Nn) {
		this.sortedPlaces_Nn = sortedPlaces_Nn;
	}

	public Place[] getSortedPlaces_Nn() {
		return sortedPlaces_Nn;
	}
}