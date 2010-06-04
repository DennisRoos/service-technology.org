/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. AGPL3.0
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

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * Abstract representation of a system for which a branching process shall be
 * constructed using {@link DNodeBP}. The representation encodes system dynamics
 * in terms of events and their pre- and post-conditions in the spirit of Petri nets.
 * 
 * This abstract class provides that basic data structures used by {@link DNodeBP}.
 * Subclass to implement constructors for building a suitable representation for
 * your system.
 * 
 * The abstract representation uses {@link DNode}s to represent static and dynamic
 * parts of system behavior. {@link DNode#isEvent} distinguishes events and conditions. 
 * 
 * @author Dirk Fahland
 */
public abstract class DNodeSys {

  /**
   * The abstract representation uses numeric IDs instead of Strings to distinguish
   * system parts. This arrays maps the IDs back to their original strings. It
   * has to be filled by suitable constructors in a subclass.
   * Use {@link #finalize_setProperNames()} to set is values consistently with
   * {@link #nameToID}. 
   */
	public String				properNames[];		  // translate IDs to names
	
	/**
	 * The reverse map from original strings to numeric identifiers. It is the
	 * developers responsibility to keep {@link #properNames} and this field
	 * consistent.
	 */
	public Map<String, Short>	nameToID;			// and names to IDs
	
	/**
	 * The highest name ID that has been given.
	 */
  public short currentNameID = 0;
	
	/**
	 * A {@link DNodeSys} has two kinds of events. This set contains all events of
	 * the system that can be fired.
	 */
	public DNode.SortedLinearList fireableEvents;
	
	/**
	 * A total linear order of all IDs of all fireable events. Used for the
	 * lexicographic search strategy in the branching process construction.
	 */
	public HashMap<Short, Short> fireableEventIndex;
	
	/**
	 * A {@link DNodeSys} has two kinds of events. This set contains all events of
   * the system that occur in preconditions of {@link #fireableEvents}. These
   * precondition events cannot be fired by themselves.
	 */
	public DNode.SortedLinearList preConEvents;
	
	/**
	 * Stores all conditions that are pre-condition of an event. Use
	 * {@link #finalize_setPreConditions()} to set this field.
	 */
	public DNode[] 				preConditions;		// set of all preconditions of an event

	/**
	 * Every system has an initial state or more generally an initial run. The
	 * initial run is represented as an acylic set of {@link DNode}s.
	 */
	public DNodeSet				initialRun;
	
	/**
	 * The length of the longest history-based precondition in the system.
	 */
	public int					maxHistoryDepth;	// depth of the longest history of a transition 
	
	/**
	 * Default constructor for initializing standard fields. Needs to be called
	 * by every subclass.
	 */
	protected DNodeSys () {
		DNode.idGen = 0;				// reset IDs
		
		maxHistoryDepth = -1;
		DNode.translationTable = this;	// everything happens wrt. to this class
		
		fireableEvents = new DNode.SortedLinearList();
		preConEvents = new DNode.SortedLinearList();
		fireableEventIndex = new HashMap<Short, Short>();
		
		init_setProperNames();
	}

	public static final String NAME_TAU = "_tau_";
	
	/**
	 * Initialize the {@link #nameToID} map.  Is called in the
   * {@link Constructor} of {@link DNodeSys}.
	 */
	protected void init_setProperNames() {
	   nameToID = new HashMap<String, Short>();
	}
	
	/**
	 * Reverse the {@literal DNodeSys#nameToID} map and store it in
	 * {@literal DNodeSys#properNames}. Adds some default names like
	 * {@value #NAME_TAU} for later use.
	 * 
	 * Assumes that {@literal DNodeSys#nameToID} contains entries
	 * <code>(name_i, i)</code> with indices i=0,...,(nameToID.size() - 1)
	 */
	protected void finalize_setProperNames () {
	  
	  if (!nameToID.containsKey(NAME_TAU))
	    nameToID.put(NAME_TAU, currentNameID++);
	  
		properNames = new String[nameToID.size()];
		for (Entry<String,Short> line : nameToID.entrySet()) {
			properNames[line.getValue()] = line.getKey();
		}
	}

	/**
	 *  Update data structure that stores all pre-conditions of all
	 *  events in the system. This method must be called by an
	 *  implementation after filling the preConEvents and fireableEvents.
	 */
	protected void finalize_setPreConditions () {
		// remember all preconditions of an event for checking
		// equivalent cuts
		int preCondNum = 0;
		for (DNode e : preConEvents) {
			preCondNum += e.pre.length;
		}
		for (DNode e : fireableEvents) {
			preCondNum += e.pre.length;
		}
		preConditions = new DNode[preCondNum];
		preCondNum = 0;
		for (DNode e : preConEvents) {
			for (DNode b : e.pre)
				preConditions[preCondNum++] = b;
		}
		for (DNode e : fireableEvents) {
			for (DNode b : e.pre)
				preConditions[preCondNum++] = b;
		}
	}
	
	/**
	 * Update data structures that describe the initial run/state of the system.
	 * This method must be called by an implementation after building the
	 * representation of the intial run.
	 */
	protected void finalize_initialRun() {
	  for (DNode d : initialRun.allEvents) {
	    d.causedBy = new int[1]; d.causedBy[0] = d.globalId;
	  }
    for (DNode d : initialRun.allConditions) {
      d.causedBy = new int[1]; d.causedBy[0] = d.globalId;
    }
	}
	
	 /**
   * Generate all indexes for the construction of the branching process.
   * These are {@link #fireableEventIndex}.
   * 
   * This method must be called by an implementation as the last method after
   * all other fields have been set.
   */
  protected void finalize_generateIndexes() {
     
    short currentIndex = 0;
    for (DNode e : fireableEvents) {
      if (fireableEventIndex.containsKey(e.id)) continue;
      fireableEventIndex.put(e.id, currentIndex++);
    }
  }
	
	/**
	 * @param d
	 * @return
	 *   <code>true</code> iff the given node <code>d</code> represents a terminal
	 *   node of the system, the actual interpretation of 'terminal' depends on the
	 *   actual system. Implementations should override this method accordingly.
	 */
	public boolean isTerminal(DNode d) {
	  return false;
	}
	
	/**
	 * @param globalId
	 * @return
	 *   {@link DNode} that represents a fireable transition of the original
	 *   system that has the given globalId. Useful for retrieve the system
	 *   transition that caused an event in a branching process.
	 */
	public DNode getTransitionForID(int globalId) {
	  for (DNode d : fireableEvents) {
	    if (d.globalId == globalId) return d;
	  }
	  return null;
	}
	
	public String getInfo () {
	  return "";
	}
}
